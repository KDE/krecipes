/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "shoppinglistdialog.h"


ShoppingListDialog::ShoppingListDialog(QWidget *parent,RecipeDB *db):QWidget(parent)
{
   // Store pointer to database
    database=db;

    // Design dialog

    layout = new QGridLayout( this, 1, 1, 0, 0);
    QSpacerItem* spacer_left = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout->addMultiCell( spacer_left, 1,4,0,0 );
    QSpacerItem* spacer_top = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout->addMultiCell(spacer_top,0,0,1,4);


    recipeListView=new KListView (this);
    layout->addWidget(recipeListView,1,1);
    recipeListView->addColumn("Id");
    recipeListView->addColumn("Recipe Title");
    QSpacerItem* spacer_toButtons = new QSpacerItem(30,10,QSizePolicy::Fixed, QSizePolicy::Minimum);
    layout->addItem(spacer_toButtons,1,2);
    addRecipeButton=new QPushButton(this);
    addRecipeButton->setText("->");
    addRecipeButton->setFixedSize(QSize(32,32));
    addRecipeButton->setFlat(true);

    QSpacerItem* buttonSpacer = new QSpacerItem(10,10,QSizePolicy::Minimum, QSizePolicy::Fixed);
    layout->addItem(buttonSpacer,1,2);

    removeRecipeButton=new QPushButton(this);
    removeRecipeButton->setText("<-");
    removeRecipeButton->setFixedSize(QSize(32,32));
    removeRecipeButton->setFlat(true);
    layout->addWidget(removeRecipeButton,3,3);

    layout->addWidget(addRecipeButton,1,3);
    QSpacerItem* spacerFromButtons = new QSpacerItem(30,10,QSizePolicy::Fixed, QSizePolicy::Minimum);
    layout->addItem(spacerFromButtons,1,4);



    // Populate UI with data
    reloadRecipeList();

    // Connect signals & slots

}

ShoppingListDialog::~ShoppingListDialog()
{
}

void ShoppingListDialog::reloadRecipeList(void)
{
ElementList recipeList;
recipeListView->clear();
database->loadRecipeList(&recipeList);

for ( Element *recipe =recipeList.getFirst(); recipe; recipe =recipeList.getNext() )
	QListViewItem *it=new QListViewItem (recipeListView,QString::number(recipe->id),recipe->name);
}

void ShoppingListDialog::reload(void)
{
reloadRecipeList (); // Missing: check if there's non-existing recipes in the list now, and if so, delete.
}