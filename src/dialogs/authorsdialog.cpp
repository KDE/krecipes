/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   Copyright (C) 2008 by Montel Laurent <montel@kde.org                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "authorsdialog.h"
#include "createelementdialog.h"
#include "backends/recipedb.h"
#include "widgets/authorlistview.h"
#include "actionshandlers/authoractionshandler.h"

#include <kdialog.h>
#include <klocale.h>
#include <kmessagebox.h>
//Added by qt3to4:
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <KPushButton>

AuthorsDialog::AuthorsDialog( QWidget* parent, RecipeDB *db )
	 : QWidget( parent )
{

	// Store pointer to database
	database = db;
	QHBoxLayout* layout = new QHBoxLayout( this );
	layout->setMargin( KDialog::marginHint() );
	layout->setSpacing( KDialog::spacingHint() );

	//Author List
	authorListView = new StdAuthorListView( this, database, true );
	authorActionsHandler = new AuthorActionsHandler( authorListView, database );
	layout->addWidget( authorListView );

	//Buttons
	QVBoxLayout* vboxl = new QVBoxLayout();
	vboxl->setSpacing( KDialog::spacingHint() );

	newAuthorButton = new KPushButton( this );
	newAuthorButton->setText( i18n( "Create ..." ) );
	newAuthorButton->setIcon( KIcon( "list-add") );
	vboxl->addWidget( newAuthorButton );

	removeAuthorButton = new KPushButton( this );
	removeAuthorButton->setText( i18n( "Delete" ) );
	removeAuthorButton->setIcon( KIcon( "list-remove") );
	vboxl->addWidget( removeAuthorButton );
	vboxl->addStretch();

	layout->addLayout( vboxl );

	//Connect Signals & Slots

	connect ( newAuthorButton, SIGNAL( clicked() ), authorActionsHandler, SLOT( createNew() ) );
	connect ( removeAuthorButton, SIGNAL( clicked() ), authorActionsHandler, SLOT( remove() ) );
}

AuthorsDialog::~AuthorsDialog()
{}

// (Re)loads the data from the database
void AuthorsDialog::reload( ReloadFlags flag )
{
	authorListView->reload( flag );
}

#include "authorsdialog.moc"
