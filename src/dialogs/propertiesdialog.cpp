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
#include <klocale.h>
#include <kdialog.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <KPushButton>

#include "backends/recipedb.h"
#include "createpropertydialog.h"
#include "widgets/propertylistview.h"
#include "actionshandlers/propertyactionshandler.h"

//Added by qt3to4:
#include <QVBoxLayout>
#include <QHBoxLayout>

PropertiesDialog::PropertiesDialog( QWidget *parent, RecipeDB *db ) : QWidget( parent )
{

	// Store pointer to database
	database = db;

	// Design dialog

	QHBoxLayout* layout = new QHBoxLayout( this );

	propertyListView = new CheckPropertyListView( this, database, true );
	propertyActionsHandler = new PropertyActionsHandler( propertyListView, database );
	propertyListView->reload();

	KConfigGroup config( KGlobal::config(), "Formatting");
	QStringList hiddenList = config.readEntry("HiddenProperties", QStringList());
	for ( Q3CheckListItem *item = (Q3CheckListItem*)propertyListView->firstChild(); item; item = (Q3CheckListItem*)item->nextSibling() ) {
		if ( !hiddenList.contains(item->text(0)) )
			item->setOn(true);
	}

	layout->addWidget ( propertyListView );

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
	connect( addPropertyButton, SIGNAL( clicked() ), propertyActionsHandler, SLOT( createNew() ) );
	connect( removePropertyButton, SIGNAL( clicked() ), propertyActionsHandler, SLOT( remove() ) );

}


PropertiesDialog::~PropertiesDialog()
{}

void PropertiesDialog::reload( void )
{
	propertyListView->reload();
}

ActionsHandlerBase* PropertiesDialog::getActionsHandler() const
{
	return propertyActionsHandler;
}

void PropertiesDialog::addAction( KAction * action )
{
	propertyActionsHandler->addAction( action );
}

#include "propertiesdialog.moc"
