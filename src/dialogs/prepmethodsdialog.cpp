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

#include "prepmethodsdialog.h"
#include "createelementdialog.h"
#include "dependanciesdialog.h"
#include "DBBackend/recipedb.h"
#include "widgets/prepmethodlistview.h"

#include <klocale.h>
#include <kmessagebox.h>

PrepMethodsDialog::PrepMethodsDialog(QWidget* parent, RecipeDB *db):QWidget(parent)
{

// Store pointer to database
database=db;

// Top & left spacers
layout = new QGridLayout(this);
QSpacerItem *spacerTop=new QSpacerItem(10,10,QSizePolicy::Minimum,QSizePolicy::Fixed);
layout->addItem(spacerTop,0,1);
QSpacerItem *spacerLeft=new QSpacerItem(10,10,QSizePolicy::Fixed,QSizePolicy::Minimum);
layout->addItem(spacerLeft,1,0);

//PrepMethod List
prepMethodListView=new StdPrepMethodListView(this,database,true);
prepMethodListView->reload();
layout->addWidget(prepMethodListView,1,1);

//Buttons
buttonBar=new QHBox(this);
layout->addWidget(buttonBar,2,1);

il=new KIconLoader;

newPrepMethodButton=new QPushButton(buttonBar);
newPrepMethodButton->setText(i18n("Create New Preparation Method"));
QPixmap pm=il->loadIcon("fileNew2", KIcon::NoGroup,16); newPrepMethodButton->setIconSet(pm);

removePrepMethodButton=new QPushButton(buttonBar);
removePrepMethodButton->setText(i18n("Remove"));
pm=il->loadIcon("editshred", KIcon::NoGroup,16); removePrepMethodButton->setIconSet(pm);
removePrepMethodButton->setMaximumWidth(100);

//Connect Signals & Slots

connect (newPrepMethodButton,SIGNAL(clicked()),this,SLOT(createNewPrepMethod()));
connect (removePrepMethodButton,SIGNAL(clicked()),this,SLOT(removePrepMethod()));
}

PrepMethodsDialog::~PrepMethodsDialog()
{
}

// (Re)loads the data from the database
void PrepMethodsDialog::reload(void)
{
prepMethodListView->reload();
}

void PrepMethodsDialog::createNewPrepMethod(void)
{
CreateElementDialog* elementDialog=new CreateElementDialog(this,i18n("New Preparation Method"));

if ( elementDialog->exec() == QDialog::Accepted ) {
   QString result = elementDialog->newElementName();
   database->createNewPrepMethod(result); // Create the new prepMethod in the database
   reload(); // Reload the list from the database
}
}

void PrepMethodsDialog::removePrepMethod(void)
{
// Find the selected prepMethod item
QListViewItem *it;
int prepMethodID=-1;

if ( (it=prepMethodListView->selectedItem()) ) prepMethodID=it->text(0).toInt();

if (prepMethodID>=0) // an prepMethod was selected previously
{
	ElementList dependingRecipes;
	database->findPrepMethodDependancies(prepMethodID,&dependingRecipes);
	if (dependingRecipes.isEmpty()) database->removePrepMethod(prepMethodID);
	else // Need Warning! 
	{
		DependanciesDialog *warnDialog = new DependanciesDialog(this,&dependingRecipes);
		if (warnDialog->exec()==QDialog::Accepted) database->removePrepMethod(prepMethodID);
		delete warnDialog;
	}
}
}

#include "prepmethodsdialog.moc"
