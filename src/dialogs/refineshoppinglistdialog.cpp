/***************************************************************************
 *   Copyright (C) 2004 by Jason Kivlighn                                  *
 *   mizunoami44@users.sourceforge.net                                     *
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
#include <qheader.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kapplication.h>
#include <kcursor.h>

#include "DBBackend/recipedb.h"
#include "widgets/krelistview.h"
#include "shoppinglistviewdialog.h"
#include "shoppingcalculator.h"
#include "mixednumber.h"

RefineShoppingListDialog::RefineShoppingListDialog( QWidget* parent, RecipeDB *db, const ElementList &recipeList )
    : QDialog( parent, "refinedialog", true ),
      database(db)
{
	refineShoppingListDialogLayout = new QVBoxLayout( this, 11, 6 ); 
	
	helpLabel = new QLabel( this, "helpLabel" );
	helpLabel->setTextFormat( QLabel::RichText );
	refineShoppingListDialogLayout->addWidget( helpLabel );
	
	layout2 = new QHBoxLayout( 0, 0, 6, "layout2"); 
	
	allIngListView = new KreListView( this, QString::null, true );
	allIngListView->listView()->addColumn( i18n( "Ingredients" ) );
	layout2->addWidget( allIngListView );
	
	layout1 = new QVBoxLayout( 0, 0, 6, "layout1"); 

	KIconLoader il;

	addButton = new QPushButton( this, "addButton" );
	addButton->setIconSet(il.loadIconSet("forward", KIcon::Small));
	addButton->setFixedSize(QSize(32,32));
	layout1->addWidget( addButton );
	
	removeButton = new QPushButton( this, "removeButton" );
	removeButton->setIconSet(il.loadIconSet("back", KIcon::Small));
	removeButton->setFixedSize(QSize(32,32));
	layout1->addWidget( removeButton );
	spacer1 = new QSpacerItem( 51, 191, QSizePolicy::Minimum, QSizePolicy::Expanding );
	layout1->addItem( spacer1 );
	layout2->addLayout( layout1 );
	
	ingListView = new KreListView( this, QString::null, true );
	ingListView->listView()->addColumn( i18n( "Ingredients in Shopping List" ) );
	ingListView->listView()->addColumn( i18n( "Amount" ) );
	ingListView->listView()->addColumn( i18n( "Unit" ) );
	ingListView->listView()->setItemsRenameable(true);
	ingListView->listView()->setRenameable(0,false);
	ingListView->listView()->setRenameable(1,true);
	ingListView->listView()->setRenameable(2,true);
	layout2->addWidget( ingListView );
	refineShoppingListDialogLayout->addLayout( layout2 );
	
	layout3 = new QHBoxLayout( 0, 0, 6, "layout3"); 
	spacer2 = new QSpacerItem( 391, 31, QSizePolicy::Expanding, QSizePolicy::Minimum );
	layout3->addItem( spacer2 );
	
	doneButton = new QPushButton( this, "doneButton" );
	doneButton->setDefault( TRUE );
	layout3->addWidget( doneButton );
	refineShoppingListDialogLayout->addLayout( layout3 );
	languageChange();

	clearWState( WState_Polished );

	connect( doneButton, SIGNAL(clicked()), SLOT(accept()) );
	connect( addButton,SIGNAL(clicked()),this,SLOT(addIngredient()));
	connect( removeButton,SIGNAL(clicked()),this,SLOT(removeIngredient()));
	connect( ingListView->listView(),SIGNAL(itemRenamed(QListViewItem*,const QString &,int)), SLOT( itemRenamed(QListViewItem*,const QString &,int) ));

	KApplication::setOverrideCursor( KCursor::waitCursor() );
	calculateShopping(recipeList,&ingredientList,database);
	KApplication::restoreOverrideCursor();

	loadData();
}

RefineShoppingListDialog::~RefineShoppingListDialog()
{
}

void RefineShoppingListDialog::languageChange()
{
	helpLabel->setText( i18n( "On the right are the ingredients needed for the recipes you selected.  You may now add additional ingredients, remove ingredients you do not need, or modify the amounts of existing ingredients." ) );
	allIngListView->listView()->header()->setLabel( 0, i18n( "Ingredients" ) );
	ingListView->listView()->header()->setLabel( 0, i18n( "Ingredients in Shopping List" ) );
	ingListView->listView()->header()->setLabel( 1, i18n( "Amount" ) );
	ingListView->listView()->header()->setLabel( 2, i18n( "Unit" ) );
	doneButton->setText( i18n( "&Done" ) );
}

void RefineShoppingListDialog::accept()
{
	ShoppingListViewDialog *view = new ShoppingListViewDialog(0,ingredientList);
	view->show();

	QDialog::accept();
}

void RefineShoppingListDialog::loadData()
{
	ElementList allIngs; database->loadIngredients(&allIngs);
	for ( ElementList::const_iterator it = allIngs.begin(); it != allIngs.end(); ++it ) {
		new QListViewItem( allIngListView->listView(), (*it).name );
	}

	for ( IngredientList::iterator it = ingredientList.begin(); it != ingredientList.end(); ++it ) {
		QString amount_str;
		if ( (*it).amount > 0 )
			amount_str = MixedNumber((*it).amount).toString();

		QListViewItem *new_item = new QListViewItem( ingListView->listView(), (*it).name, amount_str, (*it).amount>1?(*it).units.plural:(*it).units.name );
		item_ing_map.insert( new_item, it );
	}
}

void RefineShoppingListDialog::addIngredient()
{
	QListViewItem *item = allIngListView->listView()->selectedItem();
	if (item) {
		QListViewItem *new_item = new QListViewItem(ingListView->listView(),item->text(0));
		ingListView->listView()->setSelected(new_item,true);
		ingListView->listView()->ensureItemVisible(new_item);
		allIngListView->listView()->setSelected(item,false);

		item_ing_map.insert( new_item, ingredientList.append(Ingredient(item->text(0),0,Unit())) );
	}
}

void RefineShoppingListDialog::removeIngredient()
{
	QListViewItem *item = ingListView->listView()->selectedItem();
	if ( item ) {
		for ( IngredientList::iterator it = ingredientList.begin(); it != ingredientList.end(); ++it ) {
			if ( *item_ing_map.find(item) == it ) {
				ingredientList.remove(it);
				item_ing_map.remove(item);
				break;
			}
		}
		delete item;
	}
}

void RefineShoppingListDialog::itemRenamed( QListViewItem* item, const QString &new_text, int col )
{
	if ( col == 1 ) {
		IngredientList::iterator found_it = *item_ing_map.find(item);

		bool ok;
		MixedNumber amount = MixedNumber::fromString(new_text,&ok);
		if ( ok ) {
			(*found_it).amount = amount.toDouble();
		}
		else { //revert back to the valid amount string
			QString amount_str;
			if ( (*found_it).amount > 0 )
				amount_str = MixedNumber((*found_it).amount).toString();

			item->setText(1,amount_str);
		}
	}
	else if ( col == 2 ) {
		IngredientList::iterator found_it = *item_ing_map.find(item);

		if ( (*found_it).amount > 1 )
			(*found_it).units.plural = new_text;
		else
			(*found_it).units.name = new_text;
	}
}

#include "refineshoppinglistdialog.moc"
