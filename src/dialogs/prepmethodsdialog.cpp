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

#include <kdialog.h>
#include <klocale.h>
#include <kmessagebox.h>

PrepMethodsDialog::PrepMethodsDialog(QWidget* parent, RecipeDB *db):QWidget(parent)
{

// Store pointer to database
database=db;

QHBoxLayout* layout = new QHBoxLayout(this, KDialog::marginHint(), KDialog::spacingHint());

//PrepMethod List
prepMethodListView=new StdPrepMethodListView(this,database,true);
prepMethodListView->reload();
layout->addWidget(prepMethodListView);

//Buttons
QVBoxLayout* vboxl=new QVBoxLayout(KDialog::spacingHint());

newPrepMethodButton=new QPushButton(this);
newPrepMethodButton->setText(i18n("Create ..."));
newPrepMethodButton->setFlat(true);
vboxl->addWidget(newPrepMethodButton);

removePrepMethodButton=new QPushButton(this);
removePrepMethodButton->setText(i18n("Delete"));
removePrepMethodButton->setFlat(true);
vboxl->addWidget(removePrepMethodButton);
vboxl->addStretch();

layout->addLayout(vboxl);

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
