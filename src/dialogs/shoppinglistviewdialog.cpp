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

ShoppingListViewDialog::ShoppingListViewDialog(QWidget *parent, RecipeDB *db):QWidget(parent)
{

// Initialize UI Elements
shoppingListView=new KHTMLPart(this);

// Store/Initialize local variables
database=db; // Store the database pointer.


//----------Load  the list --------

showShoppingList();

}


ShoppingListViewDialog::~ShoppingListViewDialog()
{
}

void ShoppingListViewDialog::showShoppingList(void)
{

}

void ShoppingListViewDialog::calculateShopping(void)
{
}

void ShoppingListViewDialog::display(void)
{
}