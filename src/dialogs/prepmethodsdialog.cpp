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
prepMethodListView=new KListView(this);
prepMethodListView->addColumn(i18n("Id"));
prepMethodListView->addColumn(i18n("Preparation Method"));
prepMethodListView->setRenameable(1, true);
prepMethodListView->setDefaultRenameAction(QListView::Reject);
prepMethodListView->setAllColumnsShowFocus(true);
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

//Load the data from the database
reload();

//Connect Signals & Slots

connect (newPrepMethodButton,SIGNAL(clicked()),this,SLOT(createNewPrepMethod()));
connect (removePrepMethodButton,SIGNAL(clicked()),this,SLOT(removePrepMethod()));
connect(prepMethodListView,SIGNAL(doubleClicked( QListViewItem*,const QPoint &, int )),this, SLOT(modPrepMethod( QListViewItem* )));
connect(prepMethodListView,SIGNAL(itemRenamed (QListViewItem*)),this, SLOT(savePrepMethod( QListViewItem* )));
}

PrepMethodsDialog::~PrepMethodsDialog()
{
}

// (Re)loads the data from the database
void PrepMethodsDialog::reload(void)
{

//Clear the listview first
prepMethodListView->clear();

ElementList prepMethodList;
database->loadPrepMethods(&prepMethodList);
for ( ElementList::const_iterator prepMethod_it = prepMethodList.begin(); prepMethod_it != prepMethodList.end(); ++prepMethod_it )
	{
		(void)new QListViewItem(prepMethodListView,QString::number((*prepMethod_it).id),(*prepMethod_it).name);
	}
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

reload();// Reload the list from the database
}

void PrepMethodsDialog::modPrepMethod(QListViewItem* i)
{
  newPrepMethodButton->setEnabled(false);
  removePrepMethodButton->setEnabled(false);
  prepMethodListView->rename(i, 1);
}

void PrepMethodsDialog::savePrepMethod(QListViewItem* i)
{
int existing_id = database->findExistingPrepByName( i->text(1) );
int prep_id = i->text(0).toInt();
if ( existing_id != -1 && existing_id != prep_id ) //category already exists with this label... merge the two
{  
  switch (KMessageBox::warningContinueCancel(this,i18n("This preparation method already exists.  Continuing will merge these two into one.  Are you sure?")))
  {
  case KMessageBox::Continue:
  {
  	database->mergePrepMethods(existing_id,prep_id);
  	delete i;
  	break;
  }
  default: reload(); break;
  }
}
else
{
  database->modPrepMethod((i->text(0)).toInt(), i->text(1));
}

newPrepMethodButton->setEnabled(true);
removePrepMethodButton->setEnabled(true);
}


#include "prepmethodsdialog.moc"
