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

#include "shoppinglistdialog.h"

#include <kapplication.h>
#include <kcursor.h>
#include <klocale.h>

#include "DBBackend/recipedb.h"
#include "shoppinglistviewdialog.h"
#include "datablocks/recipelist.h"

ShoppingListDialog::ShoppingListDialog(QWidget *parent,RecipeDB *db):QWidget(parent)
{
   // Store pointer to database
    database=db;

    // Design dialog

    layout = new QGridLayout( this, 1, 1, 0, 0);
    QSpacerItem* spacer_left = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout->addItem( spacer_left,1,0);
    QSpacerItem* spacer_top = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout->addItem(spacer_top,0,1);

    layout->setRowStretch(1,1); layout->setRowStretch(2,1); layout->setRowStretch(3,1); layout->setRowStretch(4,1); //so the list views will expand to fill any extra space

    recipeListView=new KreListView (this,i18n("Full recipe list"),true,1);
    layout->addMultiCellWidget(recipeListView,1,4,1,1);
    recipeListView->listView()->addColumn(i18n("Id"));
    recipeListView->listView()->addColumn(i18n("Recipe Title"));
    recipeListView->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::MinimumExpanding);
    recipeListView->listView()->setAllColumnsShowFocus(true);

    QSpacerItem* spacer_toButtons = new QSpacerItem(30,10,QSizePolicy::Fixed, QSizePolicy::Minimum);
    layout->addItem(spacer_toButtons,1,2);

    KIconLoader il;
    addRecipeButton=new QPushButton(this);
    addRecipeButton->setIconSet(il.loadIconSet("forward", KIcon::Small));
    addRecipeButton->setFixedSize(QSize(32,32));
    addRecipeButton->setFlat(true);
    layout->addWidget(addRecipeButton,1,3);

    QSpacerItem* buttonSpacer = new QSpacerItem(10,10,QSizePolicy::Minimum, QSizePolicy::Fixed);
    layout->addItem(buttonSpacer,2,3);

    removeRecipeButton=new QPushButton(this);
    removeRecipeButton->setIconSet(il.loadIconSet("back", KIcon::Small));
    removeRecipeButton->setFixedSize(QSize(32,32));
    removeRecipeButton->setFlat(true);
    layout->addWidget(removeRecipeButton,3,3);


    QSpacerItem* spacerFromButtons = new QSpacerItem(30,10,QSizePolicy::Fixed, QSizePolicy::Minimum);
    layout->addItem(spacerFromButtons,1,4);

    shopRecipeListView=new KreListView (this,"Shopping list");
    layout->addMultiCellWidget(shopRecipeListView,1,4,5,5);
    shopRecipeListView->listView()->addColumn(i18n("Id"));
    shopRecipeListView->listView()->addColumn(i18n("Recipe Title"));
    shopRecipeListView->listView()->setSorting(-1);
    shopRecipeListView->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::MinimumExpanding);
    shopRecipeListView->listView()->setAllColumnsShowFocus(true);

    QSpacerItem* spacerToButtonBar = new QSpacerItem(10,10,QSizePolicy::Minimum, QSizePolicy::Fixed);
    layout->addItem(spacerToButtonBar,5,1);

    buttonBar=new QHBox(this,"buttonBar");
    layout->addMultiCellWidget(buttonBar,6,6,1,5);
    buttonBar->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);

    autoFillButton=new QPushButton(buttonBar,"dietWizardButton");
    autoFillButton->setText(i18n("Diet Wizard"));
    QPixmap pm=il.loadIcon("wizard", KIcon::NoGroup,16); autoFillButton->setIconSet(pm);
    autoFillButton->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);

    okButton=new QPushButton(buttonBar,"okButton");
    okButton->setText(i18n("&OK"));
    pm=il.loadIcon("ok", KIcon::NoGroup,16); okButton->setIconSet(pm);
    okButton->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);

    clearButton=new QPushButton(buttonBar,"clearButton");
    clearButton->setText(i18n("Clear"));
    clearButton->setMaximumWidth(100);
    pm=il.loadIcon("editclear", KIcon::NoGroup,16); clearButton->setIconSet(pm);
    clearButton->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);



    // Populate UI with data
    reloadRecipeList();

    // Connect signals & slots
    connect(addRecipeButton,SIGNAL(clicked()),this,SLOT(addRecipe()));
    connect(removeRecipeButton,SIGNAL(clicked()),this,SLOT(removeRecipe()));
    connect(okButton, SIGNAL(clicked()),this,SLOT(showShoppingList()));
    connect(clearButton,SIGNAL(clicked()),this,SLOT(clear()));
    connect(autoFillButton, SIGNAL(clicked()),this,SIGNAL(wizardClicked()));
    }

ShoppingListDialog::~ShoppingListDialog()
{
}

void ShoppingListDialog::createShopping(const RecipeList &rlist)
{
clear();
RecipeList::const_iterator it;
for (it=rlist.begin(); it != rlist.end(); it++)
{
	new QListViewItem(shopRecipeListView->listView(),shopRecipeListView->listView()->lastItem(),QString::number((*it).recipeID),(*it).title);
}
}

void ShoppingListDialog::reloadRecipeList(void)
{
ElementList recipeList;
recipeListView->listView()->clear();
database->loadRecipeList(&recipeList);

for ( ElementList::const_iterator recipe_it = recipeList.begin(); recipe_it != recipeList.end(); ++recipe_it )
	new QListViewItem (recipeListView->listView(),QString::number((*recipe_it).id),(*recipe_it).name);
}

void ShoppingListDialog::reload(void)
{
reloadRecipeList (); // Missing: check if there's non-existing recipes in the list now, and if so, delete.
}

void ShoppingListDialog::addRecipe(void)
{
QListViewItem *it;
it=recipeListView->listView()->selectedItem();
if (it) {
	int recipeID=it->text(0).toInt();
	QString recipeTitle=it->text(1);
	(void)new QListViewItem (shopRecipeListView->listView(),QString::number(recipeID),recipeTitle);
	}
}

void ShoppingListDialog::removeRecipe(void)
{
QListViewItem *it;
it=shopRecipeListView->listView()->selectedItem();
if (it) shopRecipeListView->listView()->removeItem(it);
}

void ShoppingListDialog::showShoppingList(void)
{
// Store the recipe list in ElementList object first
ElementList recipeList; QListViewItem *it;
for (it=this->shopRecipeListView->listView()->firstChild();it;it=it->nextSibling())
{
Element newEl; newEl.id=it->text(0).toInt(); newEl.name=it->text(1); // Storing the title is not necessary, but do it just in case it's used later on
recipeList.add(newEl); // Note that the element is *copied*, it's not added as pointer, so it doesn't matter it's deleted
}

KApplication::setOverrideCursor( KCursor::waitCursor() );
shoppingListDisplay=new ShoppingListViewDialog(0,database,recipeList);
shoppingListDisplay->show();
KApplication::restoreOverrideCursor();
}

void ShoppingListDialog::addRecipeToShoppingList(int recipeID)
{

QString title=database->recipeTitle(recipeID);
new QListViewItem(shopRecipeListView->listView(),QString::number(recipeID),title);
}

void ShoppingListDialog::clear()
{
shopRecipeListView->listView()->clear();
}
