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

#include "authorsdialog.h"
#include "createelementdialog.h"
#include "DBBackend/recipedb.h"
#include "widgets/authorlistview.h"

#include <kdialog.h>
#include <klocale.h>
#include <kmessagebox.h>

AuthorsDialog::AuthorsDialog(QWidget* parent, RecipeDB *db):QWidget(parent)
{

// Store pointer to database
database=db;

QHBoxLayout* layout = new QHBoxLayout(this, KDialog::marginHint(), KDialog::spacingHint());

//Author List
authorListView=new StdAuthorListView(this,database,true);
authorListView->reload();
layout->addWidget(authorListView);

//Buttons
QVBoxLayout* vboxl=new QVBoxLayout(KDialog::spacingHint());

newAuthorButton=new QPushButton(this);
newAuthorButton->setText(i18n("Create ..."));
newAuthorButton->setFlat(true);
vboxl->addWidget(newAuthorButton);

removeAuthorButton=new QPushButton(this);
removeAuthorButton->setText(i18n("Delete"));
removeAuthorButton->setFlat(true);
vboxl->addWidget(removeAuthorButton);
vboxl->addStretch();

layout->addLayout(vboxl);

//Connect Signals & Slots

connect (newAuthorButton,SIGNAL(clicked()),this,SLOT(createNewAuthor()));
connect (removeAuthorButton,SIGNAL(clicked()),this,SLOT(removeAuthor()));
}

AuthorsDialog::~AuthorsDialog()
{
}

// (Re)loads the data from the database
void AuthorsDialog::reload(void)
{
	authorListView->reload();
}

void AuthorsDialog::createNewAuthor(void)
{
CreateElementDialog* elementDialog=new CreateElementDialog(this,i18n("New Author"));

if ( elementDialog->exec() == QDialog::Accepted ) {
   QString result = elementDialog->newElementName();
   database->createNewAuthor(result); // Create the new author in the database
}
}

void AuthorsDialog::removeAuthor(void)
{
// Find the selected author item
QListViewItem *item = authorListView->selectedItem();

if ( item )
{
	switch (KMessageBox::warningContinueCancel(this,i18n("Are you sure you want to remove this author?")))
	{
	case KMessageBox::Continue: database->removeAuthor(item->text(0).toInt()); break;
	default: break;
	}
}
}

#include "authorsdialog.moc"
