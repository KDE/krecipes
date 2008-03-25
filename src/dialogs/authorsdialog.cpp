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

#include "authorsdialog.h"
#include "createelementdialog.h"
#include "backends/recipedb.h"
#include "widgets/authorlistview.h"

#include <kdialog.h>
#include <klocale.h>
#include <kmessagebox.h>
//Added by qt3to4:
#include <QVBoxLayout>
#include <QHBoxLayout>

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
	layout->addWidget( authorListView );

	//Buttons
	QVBoxLayout* vboxl = new QVBoxLayout();
   vboxl->setSpacing( KDialog::spacingHint() );

	newAuthorButton = new QPushButton( this );
	newAuthorButton->setText( i18n( "Create ..." ) );
	newAuthorButton->setFlat( true );
	vboxl->addWidget( newAuthorButton );

	removeAuthorButton = new QPushButton( this );
	removeAuthorButton->setText( i18n( "Delete" ) );
	removeAuthorButton->setFlat( true );
	vboxl->addWidget( removeAuthorButton );
	vboxl->addStretch();

	layout->addLayout( vboxl );

	//Connect Signals & Slots

	connect ( newAuthorButton, SIGNAL( clicked() ), authorListView, SLOT( createNew() ) );
	connect ( removeAuthorButton, SIGNAL( clicked() ), authorListView, SLOT( remove() ) );
}

AuthorsDialog::~AuthorsDialog()
{}

// (Re)loads the data from the database
void AuthorsDialog::reload( ReloadFlags flag )
{
	authorListView->reload( flag );
}

#include "authorsdialog.moc"
