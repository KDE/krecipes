/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "selectrecipedialog.h"

SelectRecipeDialog::SelectRecipeDialog(QWidget *parent, RecipeDB* db)
 : QVBox(parent)
{
//Store pointer to Recipe Database
database=db;

//Initialize internal data
recipeList=new ElementList;

//Initialize widgets
il=new KIconLoader;
recipeListView=new KListView(this);
    recipeListView->addColumn("Id.");
    recipeListView->addColumn("Title");
    recipeListView->setGeometry( QRect( 10, 65, 190, 280 ) );

buttonBar=new QHBox(this);

openButton=new QPushButton(buttonBar);
  openButton->setText("Open Recipe");
  QPixmap pm=il->loadIcon("view_choose", KIcon::NoGroup,16); openButton->setPixmap(pm);
editButton=new QPushButton(buttonBar);
  editButton->setText("Edit");
  pm=il->loadIcon("lineedit", KIcon::NoGroup,16); editButton->setPixmap(pm);
removeButton=new QPushButton(buttonBar);
  removeButton->setText("Delete");
  removeButton->setMaximumWidth(40);
  pm=il->loadIcon("remove", KIcon::NoGroup,16); removeButton->setPixmap(pm);

// Load Recipe List
loadRecipeList();

// Signals & Slots

connect(openButton,SIGNAL(clicked()),this, SLOT(open()));
connect(editButton,SIGNAL(clicked()),this, SLOT(edit()));
connect(removeButton,SIGNAL(clicked()),this, SLOT(remove()));
}


SelectRecipeDialog::~SelectRecipeDialog()
{
}

void SelectRecipeDialog::loadRecipeList(void)
{
recipeListView->clear();
recipeList->clear();
database->loadRecipeList(recipeList);

for ( Element *recipe =recipeList->getFirst(); recipe; recipe =recipeList->getNext() )
	QListViewItem *it=new QListViewItem (recipeListView,QString::number(recipe->id),recipe->name);


}

void SelectRecipeDialog::open(void)
{
QListViewItem *it;
if (it=recipeListView->selectedItem()) emit recipeSelected(it->text(0).toInt(),0);
}
void SelectRecipeDialog::edit(void)
{
QListViewItem *it;
if (it=recipeListView->selectedItem()) emit recipeSelected(it->text(0).toInt(),1);
}
void SelectRecipeDialog::remove(void)
{
QListViewItem *it;
if (it=recipeListView->selectedItem()) emit recipeSelected(it->text(0).toInt(),2);

}

