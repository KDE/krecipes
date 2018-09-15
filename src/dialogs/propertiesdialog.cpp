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

#include "propertiesdialog.h"

//#include <kdebug.h>

#include <klocale.h>
#include <kdialog.h>
#include <kmessagebox.h>
#include <KPushButton>

#include "backends/recipedb.h"
#include "createpropertydialog.h"
#include "widgets/krepropertylistwidget.h"
#include "actionshandlers/krepropertyactionshandler.h"

//Added by qt3to4:
#include <QVBoxLayout>
#include <QHBoxLayout>

PropertiesDialog::PropertiesDialog( QWidget *parent, RecipeDB *db ) : QWidget( parent )
{

	// Store pointer to database
	database = db;

	// Design dialog

	QHBoxLayout* layout = new QHBoxLayout( this );

	propertyListWidget = new KrePropertyListWidget( this, database );
	propertyActionsHandler = new KrePropertyActionsHandler( propertyListWidget, database );

	layout->addWidget ( propertyListWidget );

	QVBoxLayout* vboxl = new QVBoxLayout();
	vboxl->setSpacing( KDialog::spacingHint() );
	addPropertyButton = new KPushButton( this );
	addPropertyButton->setText( i18nc( "@action:button", "Create..." ) );
	addPropertyButton->setIcon( KIcon( "list-add" ) );
	vboxl->addWidget( addPropertyButton );
	removePropertyButton = new KPushButton( this );
	removePropertyButton->setText( i18nc( "@action:button", "Delete" ) );
	removePropertyButton->setIcon( KIcon( "list-remove" ) );
	vboxl->addWidget( removePropertyButton );
	vboxl->addStretch();
	layout->addLayout( vboxl );

	// Connect signals & slots
	connect( addPropertyButton, SIGNAL(clicked()), propertyActionsHandler, SLOT(createNew()) );
	connect( removePropertyButton, SIGNAL(clicked()), propertyActionsHandler, SLOT(remove()) );

}


PropertiesDialog::~PropertiesDialog()
{}

void PropertiesDialog::reload( void )
{
	propertyListWidget->reload( ForceReload );
}

KreGenericActionsHandler* PropertiesDialog::getActionsHandler() const
{
	return propertyActionsHandler;
}

void PropertiesDialog::addAction( KAction * action )
{
	propertyActionsHandler->addAction( action );
}

#include "propertiesdialog.moc"
