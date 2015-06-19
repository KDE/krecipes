/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                  *
*   Copyright © 2008 Montel Laurent <montel@kde.org>                       *
*   Copyright © 2009-2015 José Manuel Santamaría Lema <panfaust@gmail.com> *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#include "kreheaderactionshandler.h"

//#include <kdebug.h>

#include <KLocale>
#include <QPointer>
#include <KMessageBox>
#include <QModelIndex>

#include "backends/recipedb.h"

#include "widgets/kregenericlistwidget.h"

#include "dialogs/createelementdialog.h"
#include "dialogs/dependanciesdialog.h"


KreHeaderActionsHandler::KreHeaderActionsHandler( KreGenericListWidget * listWidget, RecipeDB * db ):
	KreGenericActionsHandler( listWidget, db )
{
}

void KreHeaderActionsHandler::createNew()
{
	QPointer<CreateElementDialog> elementDialog = new CreateElementDialog(
		m_listWidget, i18n( "New Header" ) );

	if ( elementDialog->exec() == QDialog::Accepted ) {
		QString result = elementDialog->newElementName();

		// Check bounds first.
		if ( checkBounds( result ) ) {
			// Create the new header in the database.
			m_database->createNewIngGroup( result );
		}
	}
	delete elementDialog;
}

void KreHeaderActionsHandler::remove()
{
	int id = m_listWidget->selectedRowId();

	//If there is no element selected, do nothing
	if ( id == -1 ) {
		return;
	}

	//Delete the ingredient selected.
	ElementList dependingRecipes;
	m_database->findUseOfIngGroupInRecipes( &dependingRecipes, id );
	if ( dependingRecipes.isEmpty() ) {
		m_database->removeIngredientGroup( id );
	} else { // Need Warning!
		ListInfo list;
		list.list = dependingRecipes;
		list.name = i18n( "Recipes" );

		QPointer<DependanciesDialog> warnDialog = new DependanciesDialog( m_listWidget, list, false );
		warnDialog->setCustomWarning( i18n("You are about to permanently delete recipes from your database.") );
		if ( warnDialog->exec() == QDialog::Accepted )
			m_database->removeIngredientGroup( id );
		delete warnDialog;
	}
}


bool KreHeaderActionsHandler::checkBounds( const QString &name )
{
	if ( name.length() > m_database->maxIngGroupNameLength() ) {
		KMessageBox::error( m_listWidget,
		i18np( "Header name cannot be longer than 1 character.",
		"Header name cannot be longer than %1 characters.",
		m_database->maxIngGroupNameLength() ) );
		return false;
	} else {
		return true;
	}
}


void KreHeaderActionsHandler::saveElement( const QModelIndex & topLeft, 
		const QModelIndex & bottomRight )
{
	Q_UNUSED(bottomRight)
	QString headerName = topLeft.data().toString();
	if ( !checkBounds( headerName ) ) {
		m_listWidget->reload( ForceReload ); //reset the changed text
		return ;
	}

	int existing_id = m_database->findExistingIngredientGroupByName( headerName );
	int id = m_listWidget->selectedRowId();
	if ( (existing_id != -1) && (existing_id != id) )  //ingredient already exists with this label... merge the two
	{
		switch ( KMessageBox::warningContinueCancel( m_listWidget,
		i18n( "This header already exists. Continuing will merge these two headers into one. Are you sure?" ) ) )
		{
		case KMessageBox::Continue: {
				m_database->modIngredientGroup( id, headerName );
				m_database->mergeIngredientGroups( existing_id, id );
				break;
			}
		default:
			m_listWidget->reload( ForceReload );
			break;
		}
	}
	else {
		m_database->modIngredientGroup( id, headerName );
	}
}
