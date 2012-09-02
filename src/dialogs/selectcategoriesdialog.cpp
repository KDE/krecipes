/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "selectcategoriesdialog.h"
#include "createcategorydialog.h"

#include <kvbox.h>

#include <klocale.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kmessagebox.h>
#include <KPushButton>
#include <QPointer>

#include "datablocks/categorytree.h"
#include "backends/recipedb.h"
#include "widgets/categorylistview.h"

SelectCategoriesDialog::SelectCategoriesDialog( QWidget *parent, const ElementList &items_on, RecipeDB *db )
		: KDialog( parent ),
		database(db)
{
	setCaption(i18nc("@title:window", "Categories" ));
	setButtons(KDialog::Ok | KDialog::Cancel);
	setDefaultButton(KDialog::Ok);
	setModal( true );
	KVBox *page = new KVBox( this );
	setMainWidget( page );
	//Design UI

	//Category List
	categoryListView = new CategoryCheckListView( page, db, true, items_on );
	categoryListView->reload();

	//New category button
	KPushButton *newCatButton = new KPushButton( page );
	newCatButton->setText( i18nc( "@action:button", "&New Category..." ) );
	newCatButton->setIcon( KIcon( "list-add" ) );

	// Load the list
	loadCategories( items_on );

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
	/*categoryListView->populateAll();

	ElementList::const_iterator it;
	for ( it = items_on.begin(); it != items_on.end(); ++it ) {
		CategoryCheckListItem *new_item = (CategoryCheckListItem*)categoryListView->findItem(QString::number((*it).id),1);
		if ( new_item ) {
			new_item->setOn(true);
		}
	}*/
}

void SelectCategoriesDialog::createNewCategory( void )
{
	ElementList categories;
	database->loadCategories( &categories );
	QPointer<CreateCategoryDialog> categoryDialog = new CreateCategoryDialog( this, categories );

	if ( categoryDialog->exec() == QDialog::Accepted ) {
		QString result = categoryDialog->newCategoryName();
		int subcategory = categoryDialog->subcategory();

		//check bounds first
		if ( result.length() > int(database->maxCategoryNameLength()) ) {
			KMessageBox::error( this, i18ncp( "@info", "Category name cannot be longer than 1 character.", "Category name cannot be longer than %1 characters.", database->maxCategoryNameLength() ) );
			return ;
		}
		// Create the new category in the database
		RecipeDB::IdType last_insert_id;
		last_insert_id = database->createNewCategory( result, subcategory );

		//a listview item will automatically be created, but we need to turn it on
		Element new_cat( result, last_insert_id );
		Q3CheckListItem *new_item = ((Q3CheckListItem*)categoryListView->findItem( QString::number(new_cat.id), 1 ));
		if ( new_item )
			new_item->setOn(true);
	}

	delete categoryDialog;
}


#include "selectcategoriesdialog.moc"
