/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "selectrecipedialog.h"
#include <klocale.h>
#include <qintdict.h>
#include "DBBackend/recipedb.h"
#include "recipe.h"
#include "selectunitdialog.h"
#include "createelementdialog.h"

SelectRecipeDialog::SelectRecipeDialog(QWidget *parent, RecipeDB* db)
 : QWidget(parent)
{
//Store pointer to Recipe Database
database=db;

//Initialize internal data
recipeList=new ElementList;
categoryList=new ElementList;
//Design dialog

layout = new QGridLayout( this, 1, 1, 0, 0);

	// Border Spacers
	QSpacerItem* spacer_left = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );	layout->addMultiCell( spacer_left, 1,4,0,0 );
	QSpacerItem* spacer_top = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addMultiCell(spacer_top,0,0,1,4);

	searchBar=new QHBox(this);
	layout->addWidget(searchBar,1,1);

	searchLabel=new QLabel(searchBar); searchLabel->setText(i18n("Search:")); searchLabel->setFixedWidth(searchLabel->fontMetrics().width(i18n("Search:"))+5);
	searchBox=new KLineEdit(searchBar);


	QSpacerItem* spacerFromSearchBar = new QSpacerItem(10,10,QSizePolicy::Minimum, QSizePolicy::Fixed);
    	layout->addItem(spacerFromSearchBar,2,1);

	il=new KIconLoader;
	recipeListView=new KListView(this);
	recipeListView->addColumn(i18n("Category"));
    	recipeListView->addColumn(i18n("Id"));
    	recipeListView->addColumn(i18n("Title"));
    	recipeListView->setGeometry( QRect( 10, 65, 190, 280 ) );
	recipeListView->setRootIsDecorated(true); // Show "+" open icons
	layout->addWidget(recipeListView,3,1);

	buttonBar=new QHBox(this);
	layout->addWidget(buttonBar,4,1);

	openButton=new QPushButton(buttonBar);
	openButton->setText(i18n("Open Recipe"));
	QPixmap pm=il->loadIcon("ok", KIcon::NoGroup,16); openButton->setIconSet(pm);
	editButton=new QPushButton(buttonBar);
	editButton->setText(i18n("Edit Recipe"));
	pm=il->loadIcon("edit", KIcon::NoGroup,16); editButton->setIconSet(pm);
	removeButton=new QPushButton(buttonBar);
	removeButton->setText(i18n("Delete"));
	removeButton->setMaximumWidth(100);
	pm=il->loadIcon("editshred", KIcon::NoGroup,16); removeButton->setIconSet(pm);

// Load Recipe List
loadRecipeList();

// Signals & Slots

connect(openButton,SIGNAL(clicked()),this, SLOT(open()));
connect(editButton,SIGNAL(clicked()),this, SLOT(edit()));
connect(removeButton,SIGNAL(clicked()),this, SLOT(remove()));
connect(searchBox,SIGNAL(returnPressed(const QString&)),this,SLOT(filter(const QString&)));
connect(searchBox,SIGNAL(textChanged(const QString&)),this,SLOT(filter(const QString&)));
}


SelectRecipeDialog::~SelectRecipeDialog()
{
}

void SelectRecipeDialog::loadRecipeList(void)
{
recipeListView->clear();
recipeList->clear();
categoryList->clear();

// First show the categories

ElementList categoryList;
QIntDict <QListViewItem> categoryItems; // Contains the QListViewItem's for every category in the QListView
database->loadCategories(&categoryList);

for ( Element *category=categoryList.getFirst(); category; category=categoryList.getNext())
	{
	QListViewItem *it=new QListViewItem(recipeListView,category->name,"","");
	categoryItems.insert(category->id,it);
	}


// Now show the recipes

int *categoryID;
Element *recipe;
QPtrList <int> recipeCategoryList;


database->loadRecipeList(recipeList,0,&recipeCategoryList); // Read the whole list of recipes including category

for ( recipe=recipeList->getFirst(),categoryID=recipeCategoryList.first();(recipe && categoryID);recipe=recipeList->getNext(),categoryID=recipeCategoryList.next())
	{
	if (QListViewItem* categoryItem=categoryItems[*categoryID])
	{
	QListViewItem *it=new QListViewItem (categoryItem,"",QString::number(recipe->id),recipe->name,"");
	}
	else
	{
	QListViewItem *it=new QListViewItem (recipeListView,"...",QString::number(recipe->id),recipe->name);
	}
	}



filter(searchBox->text());

}

void SelectRecipeDialog::open(void)
{
QListViewItem *it;
if (it=recipeListView->selectedItem()) emit recipeSelected(it->text(1).toInt(),0);
}
void SelectRecipeDialog::edit(void)
{
QListViewItem *it;
if (it=recipeListView->selectedItem()) emit recipeSelected(it->text(1).toInt(),1);
}
void SelectRecipeDialog::remove(void)
{
QListViewItem *it;
if (it=recipeListView->selectedItem()) emit recipeSelected(it->text(1).toInt(),2);

}

void SelectRecipeDialog::reload()
{
this->loadRecipeList();
}

void SelectRecipeDialog::filter(const QString& s)
{
for (QListViewItem *it=recipeListView->firstChild();it;it=it->nextSibling())
	{
	if ((s==QString::null)||(it->firstChild())) it->setVisible(true); // Don't filter if the filter text is empty or the item is a category

	else if (it->text(2).contains(s,false)) it->setVisible(true);

	else it->setVisible(false);

	// Check if there are any children (is a category)
	for (QListViewItem *cit=it->firstChild();cit;cit=cit->nextSibling())
	{
	if (s==QString::null) cit->setVisible(true); // Don't filter if the filter text is empty

	else if (cit->text(2).contains(s,false)) cit->setVisible(true);

	else cit->setVisible(false);

	}


	}
}
