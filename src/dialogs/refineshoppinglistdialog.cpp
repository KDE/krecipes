/***************************************************************************
*   Copyright (C) 2004 by Jason Kivlighn                                  *
*   (jkivlighn@gmail.com)                                                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "refineshoppinglistdialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <q3header.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

#include <klocale.h>
#include <kiconloader.h>
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
		: KDialog( parent, "refinedialog", true, QString::null,
		    KDialog::Ok, KDialog::Ok ),
		database( db )
{
	setButtonText( KDialog::Ok, i18n( "&Done" ) );

	KVBox *page = makeVBoxMainWidget();

	helpLabel = new QLabel( page, "helpLabel" );
	helpLabel->setTextFormat( QLabel::RichText );

	QWidget *layout2Widget = new QWidget(page);

	Q3HBoxLayout *layout2 = new Q3HBoxLayout( layout2Widget, 0, 6, "layout2" );

	allIngListView = new KreListView( layout2Widget, QString::null, true, 0 );
	StdIngredientListView *list_view = new StdIngredientListView(allIngListView,database);
	list_view->reload();
 	allIngListView->setListView(list_view);
	layout2->addWidget( allIngListView );

	layout1 = new Q3VBoxLayout( 0, 0, 6, "layout1" );

	KIconLoader *il = KIconLoader::global();

	addButton = new QPushButton( layout2Widget, "addButton" );
	addButton->setIconSet( il->loadIconSet( "go-next", KIcon::Small ) );
	addButton->setFixedSize( QSize( 32, 32 ) );
	layout1->addWidget( addButton );

	removeButton = new QPushButton( layout2Widget, "removeButton" );
	removeButton->setIconSet( il->loadIconSet( "go-previous", KIcon::Small ) );
	removeButton->setFixedSize( QSize( 32, 32 ) );
	layout1->addWidget( removeButton );
	spacer1 = new QSpacerItem( 51, 191, QSizePolicy::Minimum, QSizePolicy::Expanding );
	layout1->addItem( spacer1 );
	layout2->addLayout( layout1 );

	ingListView = new KreListView( layout2Widget, QString::null, true );
	ingListView->listView() ->addColumn( i18n( "Ingredients in Shopping List" ) );
	ingListView->listView() ->addColumn( i18n( "Amount" ) );
	ingListView->listView() ->addColumn( i18n( "Unit" ) );
	ingListView->listView() ->setItemsRenameable( true );
	ingListView->listView() ->setRenameable( 0, false );
	ingListView->listView() ->setRenameable( 1, true );
	ingListView->listView() ->setRenameable( 2, true );
	layout2->addWidget( ingListView );

	languageChange();

	clearWState( WState_Polished );

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
	helpLabel->setText( i18n( "On the right are the ingredients needed for the recipes you selected.  You may now add additional ingredients, remove ingredients you do not need, or modify the amounts of existing ingredients." ) );
	allIngListView->listView() ->header() ->setLabel( 0, i18n( "Ingredients" ) );
	ingListView->listView() ->header() ->setLabel( 0, i18n( "Ingredients in Shopping List" ) );
	ingListView->listView() ->header() ->setLabel( 1, i18n( "Amount" ) );
	ingListView->listView() ->header() ->setLabel( 2, i18n( "Unit" ) );
}

void RefineShoppingListDialog::accept()
{
	hide();

	ShoppingListViewDialog view( this, ingredientList );
	view.exec();

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
			KConfig * config = KGlobal::config();
			config->setGroup( "Formatting" );
	
			if ( config->readEntry( "Fraction" ) )
				amount_str = MixedNumber( ( *it ).amount ).toString();
			else
				amount_str = beautify( KGlobal::locale() ->formatNumber( ( *it ).amount, 5 ) );
		}

		Q3ListViewItem *new_item = new Q3ListViewItem( ingListView->listView(), ( *it ).name, amount_str, ( *it ).amount > 1 ? ( *it ).units.plural : ( *it ).units.name );
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

		item_ing_map.insert( new_item, ingredientList.append( Ingredient( item->text( 0 ), 0, Unit() ) ) );
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

		bool ok;
		MixedNumber amount = MixedNumber::fromString( new_text, &ok );
		if ( ok ) {
			( *found_it ).amount = amount.toDouble();
		}
		else { //revert back to the valid amount string
			QString amount_str;
			if ( ( *found_it ).amount > 0 ) {
				KConfig * config = KGlobal::config();
				config->setGroup( "Formatting" );
		
				if ( config->readEntry( "Fraction" ) )
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
			( *found_it ).units.plural = new_text;
		else
			( *found_it ).units.name = new_text;
	}
}

#include "refineshoppinglistdialog.moc"
