/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "selectcategoriesdialog.h"
#include "createcategorydialog.h"

#include <qvbox.h>

#include <klocale.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kmessagebox.h>

#include "datablocks/categorytree.h"
#include "backends/recipedb.h"
#include "widgets/categorylistview.h"

SelectCategoriesDialog::SelectCategoriesDialog( QWidget *parent, const ElementList &items_on, RecipeDB *db )
		: KDialogBase( parent, "SelectCategoriesDialog", true, i18n("Categories"),
		    KDialogBase::Ok | KDialogBase::Cancel, KDialogBase::Ok ),
		database(db)
{
	QVBox *page = makeVBoxMainWidget();

	//Design UI

	//Category List
	categoryListView = new CategoryCheckListView( page, db, true, items_on );
	categoryListView->reload();

	//New category button
	QPushButton *newCatButton = new QPushButton( page );
	newCatButton->setText( i18n( "&New Category..." ) );
	newCatButton->setFlat( true );

	// Load the list
	loadCategories( items_on );

	setSizeGripEnabled( true );

	// Connect signals & Slots
	connect ( newCatButton, SIGNAL( clicked() ), SLOT( createNewCategory() ) );
}

SelectCategoriesDialog::~SelectCategoriesDialog()
{}

void SelectCategoriesDialog::getSelectedCategories( ElementList *newSelected )
{
	*newSelected = categoryListView->selections();
}

void SelectCategoriesDialog::loadCategories( const ElementList &items_on )
{
	ElementList::const_iterator it;
        for ( it = items_on.begin(); it != items_on.end(); ++it ) {
		CategoryCheckListItem *new_item = (CategoryCheckListItem*)categoryListView->findItem(QString::number((*it).id),1);
		if ( new_item ) {
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

		//a listview item will automatically be created, but we need to turn it on
		Element new_cat( result, database->lastInsertID() );
		QCheckListItem *new_item = ((QCheckListItem*)categoryListView->findItem( QString::number(new_cat.id), 1 ));
		if ( new_item )
			new_item->setOn(true);
	}

	delete categoryDialog;
}


#include "selectcategoriesdialog.moc"
