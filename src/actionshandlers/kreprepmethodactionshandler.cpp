/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                  *
*   Copyright © 2009-2015 José Manuel Santamaría Lema <panfaust@gmail.com> *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#include "kreprepmethodactionshandler.h"

#include <KLocale>
#include <KMessageBox>
#include <QPointer>
#include <QModelIndex>

#include "backends/recipedb.h"

#include "widgets/kregenericlistwidget.h"
#include "dialogs/createelementdialog.h"
#include "dialogs/dependanciesdialog.h"


KrePrepMethodActionsHandler::KrePrepMethodActionsHandler( KreGenericListWidget * listWidget, RecipeDB *db ):
	KreGenericActionsHandler( listWidget, db )
{
}

void KrePrepMethodActionsHandler::createNew()
{
	QPointer<CreateElementDialog> elementDialog = new CreateElementDialog(
		m_listWidget, i18n( "New Preparation Method" ) );

	if ( elementDialog->exec() == QDialog::Accepted ) {
		QString result = elementDialog->newElementName();

		//check bounds first
		if ( checkBounds( result ) )
			m_database->createNewPrepMethod( result ); // Create the new prepMethod in the database
	}

	delete elementDialog;

}

void KrePrepMethodActionsHandler::remove()
{
	int id = m_listWidget->selectedRowId();

	//If there is no element selected, do nothing
	if ( id == -1 ) {
		return;
	}

	//Delete the preparation method selected.
	ElementList dependingRecipes;
	m_database->findPrepMethodDependancies( id, &dependingRecipes );
	if ( dependingRecipes.isEmpty() ) {
		m_database->removePrepMethod( id );
	} else { // Need Warning!
		ListInfo info;
		info.list = dependingRecipes;
		info.name = i18n("Recipes");
		QPointer<DependanciesDialog> warnDialog = new DependanciesDialog( m_listWidget, info );
		warnDialog->setCustomWarning( i18n("You are about to permanently delete recipes from your database.") );
		if ( warnDialog->exec() == QDialog::Accepted )
			m_database->removePrepMethod( id );
		delete warnDialog;
	}

}


void KrePrepMethodActionsHandler::saveElement( const QModelIndex& topLeft, 
	const QModelIndex& topBottom )
{
	Q_UNUSED(topBottom)

	QString prepMethodName = topLeft.data().toString();
	if ( !checkBounds( prepMethodName ) ) {
		m_listWidget->reload( ForceReload ); //reset the changed text
		return ;
	}

	int existing_id = m_database->findExistingPrepByName( prepMethodName );
	int id = m_listWidget->selectedRowId();
	if ( (existing_id != -1) && (existing_id != id) )  //already exists with this label... merge the two
	{
		switch ( KMessageBox::warningContinueCancel( m_listWidget,
		i18n( "This preparation method already exists. Continuing will merge these two headers into one. Are you sure?" ) ) )
		{
		case KMessageBox::Continue: {
				m_database->modPrepMethod( id, prepMethodName );
				m_database->mergePrepMethods( id, existing_id );
				break;
			}
		default:
			m_listWidget->reload( ForceReload );
			break;
		}
	} else {
		m_database->modPrepMethod( id, prepMethodName );
	}

}


bool KrePrepMethodActionsHandler::checkBounds( const QString &name )
{
	if ( name.length() > int(m_database->maxPrepMethodNameLength()) ) {
		KMessageBox::error( m_listWidget,
		i18np( "Preparation method cannot be longer than 1 character.",
		"Preparation method cannot be longer than %1 characters." ,
		m_database->maxPrepMethodNameLength() ) );
		return false;
	}

	return true;

}

