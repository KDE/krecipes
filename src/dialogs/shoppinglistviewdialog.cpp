/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "shoppinglistviewdialog.h"

ShoppingListViewDialog::ShoppingListViewDialog(QWidget *parent, RecipeDB *db, ElementList *recipeList):QWidget(parent)
{

 // Store pointer to database
 database=db;

 // Design dialog

 layout = new QGridLayout( this, 1, 1, 0, 0);
 QSpacerItem* spacer_left = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
 layout->addMultiCell( spacer_left, 1,4,0,0 );
 QSpacerItem* spacer_top = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
 layout->addMultiCell(spacer_top,0,0,1,4);


 htmlBox=new QVBox (this);
 shoppingListView=new KHTMLPart(htmlBox);
 layout->addMultiCellWidget(htmlBox,1,4,1,4);



//---------- Calculate & Load  the list --------

showShoppingList(recipeList);

}


ShoppingListViewDialog::~ShoppingListViewDialog()
{
}

void ShoppingListViewDialog::showShoppingList(ElementList *recipeList)
{

}

void ShoppingListViewDialog::calculateShopping(void)
{
}

void ShoppingListViewDialog::display(void)
{
}