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

#include "selectauthorsdialog.h"
#include <klocale.h>
#include <qhbox.h>
#include <qmessagebox.h>

#include "DBBackend/recipedb.h"

SelectAuthorsDialog::SelectAuthorsDialog(QWidget *parent, ElementList *currentAuthors,RecipeDB *db):QDialog(parent,0,true)
{


database=db;

//Design UI

layout = new QGridLayout( this, 1, 1, 0, 0);
	// Border Spacers
	//QSpacerItem* spacer_left = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );	//layout->addItem( spacer_left, 1,0 );
	//QSpacerItem* spacer_top = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	//layout->addItem(spacer_top,0,1);


// Combo to Pick authors

authorsCombo=new KComboBox(true,this);
layout->addWidget(authorsCombo,1,1);
authorsCombo->completionObject()->setCompletionMode( KGlobalSettings::CompletionPopupAuto );
authorsCombo->lineEdit()->disconnect(authorsCombo); //so hitting enter doesn't enter the item into the box
// Add/Remove buttons
QSpacerItem* spacerFromCombo=new QSpacerItem(10,10,QSizePolicy::Minimum, QSizePolicy::Fixed);
layout->addItem(spacerFromCombo,2,1);

il=new KIconLoader;
addAuthorButton=new QPushButton(this);
QPixmap pm=il->loadIcon("forward", KIcon::NoGroup,16); addAuthorButton->setIconSet(pm);
layout->addWidget(addAuthorButton,3,1);
removeAuthorButton=new QPushButton(this);
pm=il->loadIcon("back", KIcon::NoGroup,16); removeAuthorButton->setIconSet(pm);
layout->addWidget(removeAuthorButton,4,1);


// Author List
QSpacerItem* spacerFromAddRemoveButtons=new QSpacerItem(10,10,QSizePolicy::Fixed, QSizePolicy::Minimum);
layout->addItem(spacerFromAddRemoveButtons,1,2);

authorListView=new KListView(this);
authorListView->addColumn(i18n("Id"));
authorListView->addColumn(i18n("Author"));
authorListView->setAllColumnsShowFocus(true);
layout->addMultiCellWidget(authorListView,1,5,3,4);

//Ok/Cancel buttons
QSpacerItem* buttonSpacer=new QSpacerItem(10,10, QSizePolicy::Minimum, QSizePolicy::Fixed);
layout->addItem(buttonSpacer,6,1);

QHBox *okCancelButtonBox=new QHBox(this);
okCancelButtonBox->setSpacing(10);
layout->addMultiCellWidget(okCancelButtonBox,7,7,1,4);

okButton=new QPushButton(okCancelButtonBox);
okButton->setText(i18n("&OK"));
okButton->setFlat(true);


QSpacerItem* spacerBetweenButtons=new QSpacerItem(10,10, QSizePolicy::Fixed, QSizePolicy::Minimum);
layout->addItem(spacerBetweenButtons,5,2);

cancelButton=new QPushButton(okCancelButtonBox);
cancelButton->setText(i18n("&Cancel"));
cancelButton->setFlat(true);

//Initialize some internal variables
authorList=new ElementList;

// Load the list
loadAuthors(currentAuthors);

// Connect signals & Slots
connect (okButton,SIGNAL(clicked()),this,SLOT(accept()));
connect (cancelButton,SIGNAL(clicked()),this,SLOT(reject()));
connect (addAuthorButton,SIGNAL(clicked()),this,SLOT(addAuthor()));
connect (removeAuthorButton,SIGNAL(clicked()),this,SLOT(removeAuthor()));
}

SelectAuthorsDialog::~SelectAuthorsDialog()
{
delete authorList;
}

void SelectAuthorsDialog::getSelectedAuthors(ElementList *newAuthors)
{

for (QListViewItem *it=authorListView->firstChild();it; it=it->nextSibling())
	{
	Element author;
	author.id=it->text(0).toInt();
	author.name=it->text(1);
	newAuthors->add(author);
	}

}

void SelectAuthorsDialog::loadAuthors(ElementList *currentAuthors)
{

// Load the combo
reloadAuthorsCombo();

// Load the ListView with the authors of this recipe
authorListView->clear();
for (Element *el =currentAuthors->getFirst(); el; el=currentAuthors->getNext())
{
(void)new QListViewItem(authorListView,QString::number(el->id),el->name);
}

}

void SelectAuthorsDialog::addAuthor(void)
{
if ( authorsCombo->contains(authorsCombo->currentText()) )
	authorsCombo->setCurrentItem(authorsCombo->currentText());

createNewAuthorIfNecessary();

int currentItem=authorsCombo->currentItem();
Element *currentElement=authorList->getElement(currentItem);

if (currentElement) (void)new QListViewItem(authorListView,QString::number(currentElement->id),currentElement->name);

}

void SelectAuthorsDialog::removeAuthor(void)
{
// Find the selected item first
QListViewItem *it; it=authorListView->selectedItem();

if (it){  // Check if an author is selected first
delete it;
}

}

void SelectAuthorsDialog::createNewAuthorIfNecessary(void)
{

if (!authorsCombo->contains(authorsCombo->currentText()) &&
	authorsCombo->currentText().stripWhiteSpace() != "") // author is not in the list and is not empty
	{ // Create new author
	QString newAuthorName=authorsCombo->currentText();
	database->createNewAuthor(newAuthorName);
	//List again the authors
	reloadAuthorsCombo();

	// Select the newly created author
	authorsCombo->setCurrentItem(newAuthorName);
QMessageBox::information( this,
			  i18n("New author created"),
			  QString(i18n("A new author \"%1\" was successfully added to the list of authors")).arg(newAuthorName),
			  QMessageBox::Ok
			  );
	}
}


void SelectAuthorsDialog::reloadAuthorsCombo(void)
{

//Load the author list
authorList->clear();
database->loadAuthors(authorList);

// Load combo with all the authors
authorsCombo->clear();
authorsCombo->completionObject()->clear();

for (Element *el =authorList->getFirst(); el; el=authorList->getNext())
{
authorsCombo->insertItem(el->name);
authorsCombo->completionObject()->addItem(el->name);
}

}

