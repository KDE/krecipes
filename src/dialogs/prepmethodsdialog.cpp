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

#include "prepmethodsdialog.h"
#include "createelementdialog.h"
#include "dependanciesdialog.h"
#include "backends/recipedb.h"
#include "widgets/prepmethodlistview.h"

#include <kdialog.h>
#include <klocale.h>
#include <kmessagebox.h>

PrepMethodsDialog::PrepMethodsDialog( QWidget* parent, RecipeDB *db ) : QWidget( parent )
{

	// Store pointer to database
	database = db;

	QHBoxLayout* layout = new QHBoxLayout( this, KDialog::marginHint(), KDialog::spacingHint() );

	//PrepMethod List
	prepMethodListView = new StdPrepMethodListView( this, database, true );
	prepMethodListView->reload();
	layout->addWidget( prepMethodListView );

	//Buttons
	QVBoxLayout* vboxl = new QVBoxLayout( KDialog::spacingHint() );

	newPrepMethodButton = new QPushButton( this );
	newPrepMethodButton->setText( i18n( "Create ..." ) );
	newPrepMethodButton->setFlat( true );
	vboxl->addWidget( newPrepMethodButton );

	removePrepMethodButton = new QPushButton( this );
	removePrepMethodButton->setText( i18n( "Delete" ) );
	removePrepMethodButton->setFlat( true );
	vboxl->addWidget( removePrepMethodButton );
	vboxl->addStretch();

	layout->addLayout( vboxl );

	//Connect Signals & Slots

	connect ( newPrepMethodButton, SIGNAL( clicked() ), prepMethodListView, SLOT( createNew() ) );
	connect ( removePrepMethodButton, SIGNAL( clicked() ), prepMethodListView, SLOT( remove
		          () ) );
}

PrepMethodsDialog::~PrepMethodsDialog()
{}

// (Re)loads the data from the database
void PrepMethodsDialog::reload( void )
{
	prepMethodListView->reload();
}

#include "prepmethodsdialog.moc"
