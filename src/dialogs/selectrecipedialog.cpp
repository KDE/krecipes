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

SelectRecipeDialog::SelectRecipeDialog(QWidget *parent, RecipeDB* db)
 : QWidget(parent)
{
//Store pointer to Recipe Database
database=db;

//Initialize internal data
recipeList=new ElementList;

//Design dialog

layout = new QGridLayout( this, 1, 1, 0, 0);

	// Border Spacers
	QSpacerItem* spacer_left = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );	layout->addMultiCell( spacer_left, 1,4,0,0 );
	QSpacerItem* spacer_top = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addMultiCell(spacer_top,0,0,1,4);

	searchBar=new QHBox(this);
	layout->addWidget(searchBar,1,1);

	searchLabel=new QLabel(searchBar); searchLabel->setText("Search:"); searchLabel->setFixedWidth(searchLabel->fontMetrics().width("Search:")+5);
	searchBox=new KLineEdit(searchBar);


	QSpacerItem* spacerFromSearchBar = new QSpacerItem(10,10,QSizePolicy::Minimum, QSizePolicy::Fixed);
    	layout->addItem(spacerFromSearchBar,2,1);

	il=new KIconLoader;
	recipeListView=new KListView(this);
    	recipeListView->addColumn("Id.");
    	recipeListView->addColumn("Title");
    	recipeListView->setGeometry( QRect( 10, 65, 190, 280 ) );
	layout->addWidget(recipeListView,3,1);

	buttonBar=new QHBox(this);
	layout->addWidget(buttonBar,4,1);

	openButton=new QPushButton(buttonBar);
	openButton->setText("Open Recipe");
	QPixmap pm=il->loadIcon("ok", KIcon::NoGroup,16); openButton->setIconSet(pm);
	editButton=new QPushButton(buttonBar);
	editButton->setText("Edit Recipe");
	pm=il->loadIcon("edit", KIcon::NoGroup,16); editButton->setIconSet(pm);
	removeButton=new QPushButton(buttonBar);
	removeButton->setText("Delete");
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

void SelectRecipeDialog::reload()
{
this->loadRecipeList();
}

void SelectRecipeDialog::filter(const QString& s)
{
for (QListViewItem *it=recipeListView->firstChild();it;it=it->nextSibling())
	{
	if (s==QString::null) it->setVisible(true); // Don't filter if text is empty
	else if (it->text(1).contains(s,false)) it->setVisible(true);
	else it->setVisible(false);
	}
}