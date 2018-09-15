/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "ingredientgroupsdialog.h"

#include <KPushButton>

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <klocale.h>
#include <kdialog.h>
#include <kvbox.h>

#include "actionshandlers/kreheaderactionshandler.h"
#include "widgets/kreheaderlistwidget.h"


IngredientGroupsDialog::IngredientGroupsDialog( RecipeDB *db, QWidget *parent, const char *name ) : QWidget(parent), database(db)
{
	setObjectName( name );
	QHBoxLayout* layout = new QHBoxLayout;

	headerListWidget= new KreHeaderListWidget( this, database );
	headerActionsHandler = new KreHeaderActionsHandler( headerListWidget, database );
	layout->addWidget(headerListWidget);

	QVBoxLayout *buttonLayout = new QVBoxLayout;
	KPushButton *addHeaderButton = new KPushButton( this );
	addHeaderButton->setText( i18nc( "@action:button", "Create..." ) );
	addHeaderButton->setIcon( KIcon( "list-add" ) );
	buttonLayout->addWidget(addHeaderButton);

	KPushButton *removeHeaderButton = new KPushButton( this );
	removeHeaderButton->setText( i18nc( "@action:button", "Delete" ) );
	removeHeaderButton->setIcon( KIcon ( "list-remove" ) );
	buttonLayout->addWidget(removeHeaderButton);

	buttonLayout->addStretch();

	layout->addLayout(buttonLayout);
	setLayout( layout );

	connect( addHeaderButton, SIGNAL(clicked()), headerActionsHandler, SLOT(createNew()) );
	connect( removeHeaderButton, SIGNAL(clicked()), headerActionsHandler, SLOT(remove()) );
}

void IngredientGroupsDialog::reload( ReloadFlags flag )
{
	headerListWidget->reload( flag );
}

KreGenericActionsHandler* IngredientGroupsDialog::getActionsHandler() const
{
	return headerActionsHandler;
}

void IngredientGroupsDialog::addAction( KAction * action )
{
	headerActionsHandler->addAction( action );
}
