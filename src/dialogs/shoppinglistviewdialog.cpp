/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
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
#include "DBBackend/recipedb.h"
#include "mixednumber.h"

#include <qpushbutton.h>

#include <klocale.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kiconloader.h>

ShoppingListViewDialog::ShoppingListViewDialog(QWidget *parent, RecipeDB *db, ElementList *recipeList):QWidget(parent)
{

 // Store pointer to database
 database=db;

 // Design dialog

 layout = new QGridLayout( this, 1, 1, 0, 0);
 QSpacerItem* spacer_left = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
 layout->addItem( spacer_left, 1,0 );
 QSpacerItem* spacer_top = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
 layout->addItem(spacer_top,0,1);
 QSpacerItem* spacer_right = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
 layout->addItem(spacer_right,1,5);


 htmlBox=new QVBox (this);
 shoppingListView=new KHTMLPart(htmlBox);

 KIconLoader *il = new KIconLoader;
 QHBox *buttonsBox=new QHBox(htmlBox);
 QPushButton *okButton = new QPushButton(il->loadIconSet("ok",KIcon::Small),i18n("&OK"),buttonsBox);
 QPushButton *printButton = new QPushButton(il->loadIconSet("fileprint",KIcon::Small),i18n("&Print"),buttonsBox);

 connect( okButton, SIGNAL(clicked()), SLOT(close()) );
 connect( printButton, SIGNAL(clicked()), SLOT(print()) );

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
	recipeHTML+="<center><div STYLE=\"width: 80%\">";
	recipeHTML+=QString("<center><h1>%1</h1></center>").arg(i18n("Shopping List"));


	// Ingredient List

	recipeHTML+="<div STYLE=\"border:medium solid blue; width:90%\"><table cellspacing=0px width=100%><tbody>";
	int counter=0;
	for (Ingredient *i=ingredientList->getFirst();i;i=ingredientList->getNext())
	{

	QString color;
	if (counter) color="#CBCEFF";
	else color="#BFC2F0";
	counter=1-counter;

	KConfig *config = kapp->config(); config->setGroup("Units");
	MixedNumber::Format number_format = (config->readBoolEntry("Fraction")) ? MixedNumber::MixedNumberFormat : MixedNumber::DecimalFormat;
	QString amount_str = MixedNumber(i->amount).toString( number_format );

		recipeHTML+=QString("<tr bgcolor=\"%1\"><td>- %2:</td><td>%3 %4</td></tr>").arg(color).arg(i->name).arg(amount_str).arg(i->units);
        }
	recipeHTML+="</tbody></table></div>";
	// Close
	recipeHTML+="</div></center></body></html>";


// Display
shoppingListView->begin(KURL("file:/tmp/" )); // Initialize to /tmp, where photos and logos are stored
shoppingListView->write(recipeHTML);
shoppingListView->end();


}

void ShoppingListViewDialog::print()
{
	shoppingListView->view()->print();
}
