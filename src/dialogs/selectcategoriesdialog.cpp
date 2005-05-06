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

#include "selectcategoriesdialog.h"
#include "createcategorydialog.h"

#include <klocale.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kmessagebox.h>

#include "datablocks/categorytree.h"
#include "backends/recipedb.h"
#include "widgets/categorylistview.h"

SelectCategoriesDialog::SelectCategoriesDialog( QWidget *parent, const QMap<Element, bool> &selected, RecipeDB *db ) : QDialog( parent, 0, true )
{

	// Store pointer
	database = db;

	//Design UI
	layout = new QGridLayout( this, 3, 2, KDialog::marginHint(), KDialog::spacingHint() );

	//Category List
	categoryListView = new CategoryCheckListView( this, db );
	categoryListView->reload();
	layout->addMultiCellWidget( categoryListView, 0, 0, 0, 1 );

	//New category button
	QPushButton *newCatButton = new QPushButton( this );
	newCatButton->setText( i18n( "&New Category..." ) );
	newCatButton->setFlat( true );
	layout->addMultiCellWidget( newCatButton, 1, 1, 0, 1 );

	//Ok/Cancel buttons
	okButton = new QPushButton( this );
	okButton->setText( i18n( "&OK" ) );
	okButton->setFlat( true );
	okButton->setDefault( true );
	layout->addWidget( okButton, 2, 0 );

	cancelButton = new QPushButton( this );
	cancelButton->setText( i18n( "&Cancel" ) );
	cancelButton->setFlat( true );
	layout->addWidget( cancelButton, 2, 1 );

	// Load the list
	loadCategories( selected );

	// Connect signals & Slots
	connect ( newCatButton, SIGNAL( clicked() ), SLOT( createNewCategory() ) );
	connect ( okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect ( cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );

}

SelectCategoriesDialog::~SelectCategoriesDialog()
{}

void SelectCategoriesDialog::getSelectedCategories( ElementList *newSelected )
{
	*newSelected = categoryListView->selections();
}

void SelectCategoriesDialog::loadCategories( const QMap<Element, bool> &selected )
{
	ElementList categories; database->loadCategories( &categories );

	for ( ElementList::const_iterator it = categories.begin(); it != categories.end(); ++it ) {
		CategoryCheckListItem *new_item = (CategoryCheckListItem*)categoryListView->findItem(QString::number((*it).id),1);
		if ( new_item && selected[*it] ) {
			new_item->setOn(true);
		}
	}
}

void SelectCategoriesDialog::createNewCategory( void )
{
	ElementList categories;
	database->loadCategories( &categories );
	CreateCategoryDialog* categoryDialog = new CreateCategoryDialog( this, categories );

	if ( categoryDialog->exec() == QDialog::Accepted ) {
		QString result = categoryDialog->newCategoryName();
		int subcategory = categoryDialog->subcategory();

		//check bounds first
		if ( result.length() > database->maxCategoryNameLength() ) {
			KMessageBox::error( this, QString( i18n( "Category name cannot be longer than %1 characters." ) ).arg( database->maxCategoryNameLength() ) );
			return ;
		}

		database->createNewCategory( result, subcategory ); // Create the new category in the database

		//a listview item will automatically be create, but we need to turn it on
		Element new_cat( result, database->lastInsertID() );
		((QCheckListItem*)categoryListView->findItem( QString::number(new_cat.id), 1 ))->setOn(true);
	}

	delete categoryDialog;
}


#include "selectcategoriesdialog.moc"
