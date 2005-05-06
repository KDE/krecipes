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

#include "categorieseditordialog.h"

#include <kdebug.h>
#include <kdialog.h>
#include <klocale.h>
#include <kmessagebox.h>

#include "widgets/categorylistview.h"
#include "createcategorydialog.h"
#include "backends/recipedb.h"

CategoriesEditorDialog::CategoriesEditorDialog( QWidget* parent, RecipeDB *db ) : QWidget( parent )
{

	// Store pointer to database
	database = db;

	QHBoxLayout* layout = new QHBoxLayout( this, KDialog::marginHint(), KDialog::spacingHint() );

	//Category List
	categoryListView = new StdCategoryListView( this, database, true );
	categoryListView->reload();
	QListViewItemIterator it( categoryListView ); //expand all items
	while ( it.current() ) {
		QListViewItem * item = it.current();
		item->setOpen( true );
		++it;
	}
	layout->addWidget( categoryListView );

	//Buttons
	QVBoxLayout* vboxl = new QVBoxLayout( KDialog::spacingHint() );

	newCategoryButton = new QPushButton( this );
	newCategoryButton->setText( i18n( "Create ..." ) );
	newCategoryButton->setFlat( true );
	vboxl->addWidget( newCategoryButton );

	removeCategoryButton = new QPushButton( this );
	removeCategoryButton->setText( i18n( "Delete" ) );
	removeCategoryButton->setFlat( true );
	vboxl->addWidget( removeCategoryButton );
	vboxl->addStretch();

	layout->addLayout( vboxl );

	//Connect Signals & Slots

	connect ( newCategoryButton, SIGNAL( clicked() ), categoryListView, SLOT( createNew() ) );
	connect ( removeCategoryButton, SIGNAL( clicked() ), categoryListView, SLOT( remove
		          () ) );
}

CategoriesEditorDialog::~CategoriesEditorDialog()
{}

void CategoriesEditorDialog::reload()
{
	categoryListView->reload();
}

#include "categorieseditordialog.moc"
