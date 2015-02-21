/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "refineshoppinglistdialog.h"

#include <KPushButton>
#include <QLabel>
#include <q3header.h>
#include <QPointer>
//Added by qt3to4:
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <KVBox>
#include <klocale.h>
#include <kapplication.h>
#include <kcursor.h>
#include <kconfig.h>
#include <kglobal.h>

#include "backends/recipedb.h"
#include "widgets/krelistview.h"
#include "widgets/ingredientlistview.h"
#include "shoppinglistviewdialog.h"
#include "shoppingcalculator.h"
#include "datablocks/mixednumber.h"

RefineShoppingListDialog::RefineShoppingListDialog( QWidget* parent, RecipeDB *db, const ElementList &recipeList )
		: KDialog( parent),
		database( db )
{
	setCaption( i18nc( "@title:window", "Refine Shopping List" ) );
	setButtons(KDialog::Ok);
	setDefaultButton(KDialog::Ok);
	setModal( true );

	KVBox *page = new KVBox( this );
	setMainWidget( page );

	setButtonText( KDialog::Ok, i18nc( "@action:button Finished refining shopping list", "&Done" ) );


	helpLabel = new QLabel( page );
	helpLabel->setObjectName( "helpLabel" );
	helpLabel->setTextFormat( Qt::RichText );
	helpLabel->setWordWrap(true);
	QWidget *layout2Widget = new QWidget(page);

	QHBoxLayout *layout2 = new QHBoxLayout( layout2Widget );
	layout2->setObjectName( "layout2" );
	layout2->setMargin( 0 );
	layout2->setSpacing( 6 );

	allIngListView = new KreListView( layout2Widget, QString(), true, 0 );
	StdIngredientListView *list_view = new StdIngredientListView(allIngListView,database);
	list_view->reload();
	allIngListView->setListView(list_view);
	allIngListView->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
	allIngListView->listView()->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
	layout2->addWidget( allIngListView );

	layout1 = new QVBoxLayout( );
	layout1->setObjectName( "layout1" );
	layout1->setMargin( 0 );
	layout1->setSpacing( 6 );

	layout1->addStretch();

	addButton = new KPushButton( layout2Widget );
	addButton->setObjectName( "addButton" );
	addButton->setIcon( KIcon( "arrow-right" ) );
	addButton->setFixedSize( QSize( 32, 32 ) );
	layout1->addWidget( addButton );

	removeButton = new KPushButton( layout2Widget );
	removeButton->setObjectName( "removeButton" );
	removeButton->setIcon( KIcon( "arrow-left" ) );
	removeButton->setFixedSize( QSize( 32, 32 ) );
	layout1->addWidget( removeButton );

	layout1->addStretch();
	layout2->addLayout( layout1 );

	ingListView = new KreListView( layout2Widget, QString(), true );
	ingListView->listView() ->addColumn( i18nc( "@title:column", "Ingredients in Shopping List" ) );
	ingListView->listView() ->addColumn( i18nc( "@title:column", "Amount" ) );
	ingListView->listView() ->addColumn( i18nc( "@title:column", "Unit" ) );
	ingListView->listView() ->setItemsRenameable( true );
	ingListView->listView() ->setRenameable( 0, false );
	ingListView->listView() ->setRenameable( 1, true );
	ingListView->listView() ->setRenameable( 2, true );
	ingListView->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
	ingListView->listView()->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
	layout2->addWidget( ingListView );

	languageChange();

	//clearWState( WState_Polished );

	connect( addButton, SIGNAL( clicked() ), this, SLOT( addIngredient() ) );
	connect( removeButton, SIGNAL( clicked() ), this, SLOT( removeIngredient() ) );
	connect( ingListView->listView(), SIGNAL( itemRenamed( Q3ListViewItem*, const QString &, int ) ), SLOT( itemRenamed( Q3ListViewItem*, const QString &, int ) ) );

	KApplication::setOverrideCursor( Qt::WaitCursor );
	calculateShopping( recipeList, &ingredientList, database );
	KApplication::restoreOverrideCursor();

	loadData();
}

RefineShoppingListDialog::~RefineShoppingListDialog()
{}

void RefineShoppingListDialog::languageChange()
{
	helpLabel->setText( i18nc( "@info", "On the right are the ingredients needed for the recipes you selected.  You may now add additional ingredients, remove ingredients you do not need, or modify the amounts of existing ingredients." ) );
	allIngListView->listView() ->header() ->setLabel( 0, i18nc( "@title:column", "Ingredients" ) );
	ingListView->listView() ->header() ->setLabel( 0, i18nc( "@title:column", "Ingredients in Shopping List" ) );
	ingListView->listView() ->header() ->setLabel( 1, i18nc( "@title:column", "Amount" ) );
	ingListView->listView() ->header() ->setLabel( 2, i18nc( "@title:column", "Unit" ) );
}

void RefineShoppingListDialog::accept()
{
	hide();

	QPointer<ShoppingListViewDialog> view = new ShoppingListViewDialog( this, ingredientList );
	view->exec();
	delete view;

	QDialog::accept();
}

void RefineShoppingListDialog::loadData()
{
	for ( IngredientList::iterator it = ingredientList.begin(); it != ingredientList.end(); ++it ) {
		//from here on, the shopping list will work with the upper value of the range (if exists)
		(*it).amount = (*it).amount+(*it).amount_offset;
		(*it).amount_offset = 0;

		QString amount_str;
		if ( ( *it ).amount > 0 ) {
		             KConfigGroup config( KGlobal::config(), "Formatting" );

			if ( !config.readEntry( "Fraction" ).isEmpty() )
				amount_str = MixedNumber( ( *it ).amount ).toString();
			else
				amount_str = beautify( KGlobal::locale() ->formatNumber( ( *it ).amount, 5 ) );
		}

		Q3ListViewItem *new_item = new Q3ListViewItem( ingListView->listView(), ( *it ).name, amount_str, ( *it ).units.determineName( (*it ).amount, false ) );
		item_ing_map.insert( new_item, it );
	}
}

void RefineShoppingListDialog::addIngredient()
{
	Q3ListViewItem * item = allIngListView->listView() ->selectedItem();
	if ( item ) {
		Q3ListViewItem * new_item = new Q3ListViewItem( ingListView->listView(), item->text( 0 ) );
		ingListView->listView() ->setSelected( new_item, true );
		ingListView->listView() ->ensureItemVisible( new_item );
		allIngListView->listView() ->setSelected( item, false );

		ingredientList.append( Ingredient( item->text( 0 ), 0, Unit() ) );
		item_ing_map.insert( new_item, ingredientList.fromLast() );
	}
}

void RefineShoppingListDialog::removeIngredient()
{
	Q3ListViewItem * item = ingListView->listView() ->selectedItem();
	if ( item ) {
		for ( IngredientList::iterator it = ingredientList.begin(); it != ingredientList.end(); ++it ) {
			if ( *item_ing_map.find( item ) == it ) {
				ingredientList.remove( it );
				item_ing_map.remove( item );
				break;
			}
		}
		delete item;
	}
}

void RefineShoppingListDialog::itemRenamed( Q3ListViewItem* item, const QString &new_text, int col )
{
	if ( col == 1 ) {
		IngredientList::iterator found_it = *item_ing_map.find( item );

		QValidator::State state;
		MixedNumber amount;
		state = MixedNumber::fromString( new_text, amount, true );
		if ( state == QValidator::Acceptable ) {
			( *found_it ).amount = amount.toDouble();
		}
		else { //revert back to the valid amount string
			QString amount_str;
			if ( ( *found_it ).amount > 0 ) {
		                     KConfigGroup config( KGlobal::config(), "Formatting" );

				if ( !config.readEntry( "Fraction" ).isEmpty() )
					amount_str = MixedNumber( ( *found_it ).amount ).toString();
				else
					amount_str = beautify( KGlobal::locale() ->formatNumber( ( *found_it ).amount, 5 ) );
			}

			item->setText( 1, amount_str );
		}
	}
	else if ( col == 2 ) {
		IngredientList::iterator found_it = *item_ing_map.find( item );

		if ( ( *found_it ).amount > 1 )
			( *found_it ).units.setPlural(new_text);
		else
			( *found_it ).units.setName(new_text);
	}
}

#include "refineshoppinglistdialog.moc"
