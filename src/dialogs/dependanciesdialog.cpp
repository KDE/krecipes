/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "dependanciesdialog.h"
#include  <qlayout.h>


DependanciesDialog::DependanciesDialog(QWidget *parent,ElementList* recipeList, ElementList* ingredientList,ElementList* propertiesList):QDialog(parent,0,true)
{

// Design the dialog
QGridLayout *layout=new QGridLayout(this);
QSpacerItem *spacer_top=new QSpacerItem(10,10,QSizePolicy::Minimum, QSizePolicy::Fixed);
layout->addItem(spacer_top,0,1);
QSpacerItem *spacer_left=new QSpacerItem(10,10,QSizePolicy::Fixed, QSizePolicy::Minimum);
layout->addItem(spacer_left,1,0);

if (recipeList)
{
recipeListView=new KListView(this);
recipeListView->addColumn("Id.");
recipeListView->addColumn("Recipe Title");
layout->addWidget(recipeListView,1,1);
loadList(recipeListView,recipeList);
}

if (ingredientList)
{
ingredientListView=new KListView(this);
ingredientListView->addColumn("Id.");
ingredientListView->addColumn("Ingredient Name");
layout->addWidget(ingredientListView,3,1);
loadList(ingredientListView,ingredientList);
}

if (propertiesList)
{
propertiesListView=new KListView(this);
propertiesListView->addColumn("Id.");
propertiesListView->addColumn("Property Name");
layout->addWidget(propertiesListView,1,3);
loadList(propertiesListView,propertiesList);
}

}

DependanciesDialog::~DependanciesDialog()
{
}


void DependanciesDialog::loadList(KListView* listView,ElementList *list)
{
Element *el;
for (el=list->getFirst();el;el=list->getNext()){
	QListViewItem* it=new QListViewItem(listView,QString::number(el->id),el->name);
	listView->insertItem(it);
	}
}