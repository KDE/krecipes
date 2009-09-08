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
	layout->setMargin( KDialog::marginHint() );
	layout->setSpacing( KDialog::spacingHint() );

	//Category List
	categoryListView = new StdCategoryListView( this, database, true );
	categoryActionsHandler = new CategoryActionsHandler( categoryListView, database );
	layout->addWidget( categoryListView );

	//Buttons
	QVBoxLayout* vboxl = new QVBoxLayout();
	vboxl->setSpacing( KDialog::spacingHint() );
	newCategoryButton = new KPushButton( this );
	newCategoryButton->setText( i18n( "Create ..." ) );
	newCategoryButton->setIcon( KIcon( "list-add" ) );
	vboxl->addWidget( newCategoryButton );

	removeCategoryButton = new KPushButton( this );
	removeCategoryButton->setText( i18n( "Delete" ) );
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

ActionsHandlerBase* CategoriesEditorDialog::getActionsHandler () const
{
	return categoryActionsHandler;
}

#include "categorieseditordialog.moc"
