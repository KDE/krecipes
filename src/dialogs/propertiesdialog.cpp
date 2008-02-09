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

#include "propertiesdialog.h"
#include <klocale.h>
#include <kdialog.h>
#include <kmessagebox.h>
#include <kconfig.h>

#include "backends/recipedb.h"
#include "createpropertydialog.h"
#include "widgets/propertylistview.h"
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3HBoxLayout>

PropertiesDialog::PropertiesDialog( QWidget *parent, RecipeDB *db ) : QWidget( parent )
{

	// Store pointer to database
	database = db;

	// Design dialog

	Q3HBoxLayout* layout = new Q3HBoxLayout( this, KDialog::marginHint(), KDialog::spacingHint() );

	propertyListView = new CheckPropertyListView( this, database, true );
	propertyListView->reload();

	KConfigGroup config( KGlobal::config(), "Formatting");
	QStringList hiddenList = config.readEntry("HiddenProperties", QStringList());
	for ( Q3CheckListItem *item = (Q3CheckListItem*)propertyListView->firstChild(); item; item = (Q3CheckListItem*)item->nextSibling() ) {
		if ( !hiddenList.contains(item->text(0)) )
			item->setOn(true);
	}

	layout->addWidget ( propertyListView );

	Q3VBoxLayout* vboxl = new Q3VBoxLayout( KDialog::spacingHint() );
	addPropertyButton = new QPushButton( this );
	addPropertyButton->setText( i18n( "Create ..." ) );
	addPropertyButton->setFlat( true );
	vboxl->addWidget( addPropertyButton );
	removePropertyButton = new QPushButton( this );
	removePropertyButton->setText( i18n( "Delete" ) );
	removePropertyButton->setFlat( true );
	vboxl->addWidget( removePropertyButton );
	vboxl->addStretch();
	layout->addLayout( vboxl );

	// Connect signals & slots
	connect( addPropertyButton, SIGNAL( clicked() ), propertyListView, SLOT( createNew() ) );
	connect( removePropertyButton, SIGNAL( clicked() ), propertyListView, SLOT( remove
		         () ) );

	//FIXME: We've got some sort of build issue... we get undefined references to CreatePropertyDialog without this dummy code here
	UnitList list;
	CreatePropertyDialog d( this, &list );
}


PropertiesDialog::~PropertiesDialog()
{}

void PropertiesDialog::reload( void )
{
	propertyListView->reload();
}

#include "propertiesdialog.moc"
