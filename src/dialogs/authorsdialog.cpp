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

#include <klocale.h>
#include <kmessagebox.h>

AuthorsDialog::AuthorsDialog(QWidget* parent, RecipeDB *db):QWidget(parent)
{

// Store pointer to database
database=db;

// Top & left spacers
layout = new QGridLayout(this);
QSpacerItem *spacerTop=new QSpacerItem(10,10,QSizePolicy::Minimum,QSizePolicy::Fixed);
layout->addItem(spacerTop,0,1);
QSpacerItem *spacerLeft=new QSpacerItem(10,10,QSizePolicy::Fixed,QSizePolicy::Minimum);
layout->addItem(spacerLeft,1,0);

//Author List
authorListView=new StdAuthorListView(this,database,true);
authorListView->reload();
layout->addWidget(authorListView,1,1);

//Buttons
buttonBar=new QHBox(this);
layout->addWidget(buttonBar,2,1);

il=new KIconLoader;

newAuthorButton=new QPushButton(buttonBar);
newAuthorButton->setText(i18n("Create New Author"));
QPixmap pm=il->loadIcon("fileNew2", KIcon::NoGroup,16); newAuthorButton->setIconSet(pm);

removeAuthorButton=new QPushButton(buttonBar);
removeAuthorButton->setText(i18n("Remove"));
pm=il->loadIcon("editshred", KIcon::NoGroup,16); removeAuthorButton->setIconSet(pm);
removeAuthorButton->setMaximumWidth(100);

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
QListViewItem *it;
int authorID=-1;

if ( (it=authorListView->selectedItem()) ) authorID=it->text(0).toInt();

if (authorID>=0) // an author was selected previously
{
database->removeAuthor(authorID);
}
}

#include "authorsdialog.moc"
