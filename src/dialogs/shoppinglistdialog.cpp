/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
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
    layout->addMultiCellWidget(recipeListView,1,4,1,1);
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

    shopRecipeListView=new KListView (this);
    layout->addMultiCellWidget(shopRecipeListView,1,4,5,5);
    shopRecipeListView->addColumn("Id");
    shopRecipeListView->addColumn("Recipe Title");

    QSpacerItem* spacerToButtonBar = new QSpacerItem(10,10,QSizePolicy::Minimum, QSizePolicy::Fixed);
    layout->addItem(spacerToButtonBar,5,1);

    buttonBar=new QHBox(this);
    layout->addMultiCellWidget(buttonBar,6,6,1,5);
    buttonBar->setFixedHeight(30);
    buttonBar->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);

    il=new KIconLoader;
    autoFillButton=new QPushButton(buttonBar);
    autoFillButton->setText("Diet Wizard");
    QPixmap pm=il->loadIcon("wizard", KIcon::NoGroup,16); autoFillButton->setPixmap(pm);

    okButton=new QPushButton(buttonBar);
    okButton->setText("Ok");
    pm=il->loadIcon("ok", KIcon::NoGroup,16); okButton->setPixmap(pm);

    clearButton=new QPushButton(buttonBar);
    clearButton->setText("Clear");
    clearButton->setMaximumWidth(40);
    pm=il->loadIcon("editclear", KIcon::NoGroup,16); clearButton->setPixmap(pm);



    // Populate UI with data
    reloadRecipeList();

    // Connect signals & slots
    connect(addRecipeButton,SIGNAL(clicked()),this,SLOT(addRecipe()));
    connect(removeRecipeButton,SIGNAL(clicked()),this,SLOT(removeRecipe()));
    connect(okButton, SIGNAL(clicked()),this,SLOT(showShoppingList()));
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

void ShoppingListDialog::addRecipe(void)
{
QListViewItem *it;
it=recipeListView->selectedItem();
if (it) {
	int recipeID=it->text(0).toInt();
	QString recipeTitle=it->text(1);
	QListViewItem *newIt=new QListViewItem (shopRecipeListView,QString::number(recipeID),recipeTitle);
	}
}

void ShoppingListDialog::removeRecipe(void)
{
QListViewItem *it;
it=shopRecipeListView->selectedItem();
if (it) shopRecipeListView->removeItem(it);
}

void ShoppingListDialog::showShoppingList(void)
{
shoppingListDisplay=new ShoppingListViewDialog(0,database);
shoppingListDisplay->show();
}