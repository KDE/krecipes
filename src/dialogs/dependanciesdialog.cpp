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

#include "dependanciesdialog.h"
#include "elementlist.h"
#include <qlayout.h>
#include <klocale.h>

DependanciesDialog::DependanciesDialog(QWidget *parent,const ElementList* recipeList, const ElementList* ingredientList,const ElementList* propertiesList):QDialog(parent,0,true)
{
int row=3, col=1;

// Design the dialog
QGridLayout *layout=new QGridLayout(this);
QSpacerItem *spacer_top=new QSpacerItem(10,10,QSizePolicy::Minimum, QSizePolicy::Fixed);
layout->addItem(spacer_top,0,1);
QSpacerItem *spacer_left=new QSpacerItem(10,10,QSizePolicy::Fixed, QSizePolicy::Minimum);
layout->addItem(spacer_left,1,0);

instructionsLabel=new QLabel(this);
instructionsLabel->setMinimumSize(QSize(100,30));
instructionsLabel->setMaximumSize(QSize(10000,10000));
instructionsLabel->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
instructionsLabel->setText(i18n("The following elements will have to be removed also, since currently they use the element you have chosen to be removed."));
layout->addWidget(instructionsLabel,1,1);
QSpacerItem *instructions_spacer=new QSpacerItem(10,10,QSizePolicy::Minimum, QSizePolicy::Fixed);
layout->addItem(instructions_spacer, 2,1);


if (recipeList)
{
if (!(recipeList->isEmpty()))
{
recipeBox=new QGroupBox(1,Qt::Vertical,i18n("Recipes"),this);
recipeListView=new KListView(recipeBox);
recipeListView->addColumn(i18n("Id"));
recipeListView->addColumn(i18n("Recipe Title"));
recipeListView->setAllColumnsShowFocus(true);
layout->addWidget(recipeBox,row,col);
QSpacerItem *list_spacer=new QSpacerItem(10,10,QSizePolicy::Fixed, QSizePolicy::Minimum);
layout->addItem(list_spacer, row+1,col);
loadList(recipeListView,recipeList);
row+=2;
if (row>6) {row=3; col+=2;} // Only two listview rows per column
}
}



if (ingredientList)
{
if (!(ingredientList->isEmpty()))
{
ingredientBox=new QGroupBox(1,Qt::Vertical,i18n("Ingredients"),this);
ingredientListView=new KListView(ingredientBox);
ingredientListView->addColumn(i18n("Id"));
ingredientListView->addColumn(i18n("Ingredient Name"));
layout->addWidget(ingredientBox,row,col);
QSpacerItem *list_spacer=new QSpacerItem(10,10,QSizePolicy::Fixed, QSizePolicy::Minimum);
layout->addItem(list_spacer, row+1,col);
loadList(ingredientListView,ingredientList);
row+=2;
if (row>6) {row=3; col+=2;} // Only two listview rows per column
}
}

if (propertiesList)
{
if (!(propertiesList->isEmpty()))
{
propertiesBox=new QGroupBox(1,Qt::Vertical,i18n("Properties"),this);
propertiesListView=new KListView(propertiesBox);
propertiesListView->addColumn(i18n("Id"));
propertiesListView->addColumn(i18n("Property"));
layout->addWidget(propertiesBox,row,col);
QSpacerItem *list_spacer=new QSpacerItem(10,10,QSizePolicy::Fixed, QSizePolicy::Minimum);
layout->addItem(list_spacer, row+1,col);
loadList(propertiesListView,propertiesList);
row+=2;
if (row>6) {row=3; col+=2;} // Only two listview rows per column
}
}


// Put the necessary vertical spacers
if (col>1) // There have been multiple (2) rows, so add vertical spacer
{
QSpacerItem *list_spacer=new QSpacerItem(10,10,QSizePolicy::Minimum, QSizePolicy::Fixed);
layout->addItem(list_spacer, 4,1);
}


// Ok/Cancel Buttons
buttonBox=new QGroupBox(2,Qt::Horizontal,this); buttonBox->setFlat(true);
okButton=new QPushButton(buttonBox); okButton->setText(i18n("&OK")); okButton->setFlat(true);
cancelButton=new QPushButton(buttonBox); cancelButton->setText(i18n("Cancel")); cancelButton->setFlat(true); cancelButton->setDefault(true);
QSpacerItem *list_spacer=new QSpacerItem(10,10,QSizePolicy::Fixed, QSizePolicy::Minimum);
if (col>1) // There are 2 rows
	{
	layout->addItem(list_spacer, 6,1);
	layout->addMultiCellWidget(buttonBox,7,7,1,col-1);
	}
else // There is a single row (and single column)
	{
	layout->addItem(list_spacer, 4,1);
	layout->addWidget(buttonBox,5,1);
	}

// Connect signals & slots
connect (okButton, SIGNAL(clicked()), this, SLOT(accept()));
connect (cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

}

DependanciesDialog::~DependanciesDialog()
{
}


void DependanciesDialog::loadList(KListView* listView,const ElementList *list)
{
for ( ElementList::const_iterator el_it = list->begin(); el_it != list->end(); ++el_it )
{
	QString id; int idnum=(*el_it).id;
	if (idnum<0) id="-"; else id=QString::number(idnum);
	QListViewItem* it=new QListViewItem(listView,id,(*el_it).name);
	listView->insertItem(it);
	}
}
