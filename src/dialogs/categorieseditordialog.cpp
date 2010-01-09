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

#include "categorieseditordialog.h"

#include <kdebug.h>
#include <kdialog.h>
#include <klocale.h>
#include <kmessagebox.h>

#include "widgets/categorylistview.h"
#include "createcategorydialog.h"
#include "backends/recipedb.h"
#include "actionshandlers/categoryactionshandler.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <KPushButton>

CategoriesEditorDialog::CategoriesEditorDialog( QWidget* parent, RecipeDB *db ) : QWidget( parent )
{

	// Store pointer to database
	database = db;

	QHBoxLayout* layout = new QHBoxLayout( this );

	//Category List
	categoryListView = new StdCategoryListView( this, database, true );
	categoryActionsHandler = new CategoryActionsHandler( categoryListView, database );
	layout->addWidget( categoryListView );

	//Buttons
	QVBoxLayout* vboxl = new QVBoxLayout();
	vboxl->setSpacing( KDialog::spacingHint() );
	newCategoryButton = new KPushButton( this );
	newCategoryButton->setText( i18nc( "@action:button", "Create..." ) );
	newCategoryButton->setIcon( KIcon( "list-add" ) );
	vboxl->addWidget( newCategoryButton );

	removeCategoryButton = new KPushButton( this );
	removeCategoryButton->setText( i18nc( "@action:button", "Delete" ) );
	removeCategoryButton->setIcon( KIcon( "list-remove" ) );
	vboxl->addWidget( removeCategoryButton );
	vboxl->addStretch();

	layout->addLayout( vboxl );

	//Connect Signals & Slots

	connect ( newCategoryButton, SIGNAL( clicked() ), categoryActionsHandler, SLOT( createNew() ) );
	connect ( removeCategoryButton, SIGNAL( clicked() ), categoryActionsHandler, SLOT( remove() ) );
}

CategoriesEditorDialog::~CategoriesEditorDialog()
{}

void CategoriesEditorDialog::reload( ReloadFlags flag )
{
	categoryListView->reload( flag );
}

CategoryActionsHandler* CategoriesEditorDialog::getActionsHandler () const
{
	return categoryActionsHandler;
}

void CategoriesEditorDialog::addAction( KAction * action )
{
	categoryActionsHandler->addAction( action );
}

void CategoriesEditorDialog::setCategoryPasteAction( KAction * action )
{
	categoryActionsHandler->setCategoryPasteAction( action );
}

void CategoriesEditorDialog::setPasteAsSubcategoryAction( KAction * action )
{
	categoryActionsHandler->setPasteAsSubcategoryAction( action );
}

#include "categorieseditordialog.moc"
