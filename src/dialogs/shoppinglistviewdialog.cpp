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
#include "shoppingcalculator.h"
#include "ingredientlist.h"
#include "elementlist.h"
#include "recipedb.h"
#include <klocale.h>

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

 this->setMinimumSize(QSize(500,600));



//---------- Calculate & Load  the list --------

showShoppingList(recipeList);

}


ShoppingListViewDialog::~ShoppingListViewDialog()
{
}

void ShoppingListViewDialog::showShoppingList(ElementList *recipeList)
{
IngredientList ingredientList;
calculateShopping(recipeList,&ingredientList,database);
display(&ingredientList);

}

void ShoppingListViewDialog::display(IngredientList *ingredientList)
{
QString recipeHTML;



// Create HTML Code

	// Headers
	recipeHTML=QString("<html><head><title>%1</title></head><body>").arg(i18n("Shopping List"));
	recipeHTML+="<div STYLE=\"position: absolute; top: 30px; left:10px; width: 80%\">";
	recipeHTML+=QString("<center><h1>%1</h1></center>").arg("Shopping List");


	// Ingredient List
	recipeHTML+="<p>";
	for (Ingredient *i=ingredientList->getFirst();i;i=ingredientList->getNext())
		recipeHTML+=QString("%1: %2 %3<br>").arg(i->name).arg(i->amount).arg(i->units);

	recipeHTML+="</p>";
	// Close
	recipeHTML+="</div></body></html>";


// Display
shoppingListView->begin(KURL("file:/tmp/" )); // Initialize to /tmp, where photos and logos are stored
shoppingListView->write(recipeHTML);
shoppingListView->end();


}
