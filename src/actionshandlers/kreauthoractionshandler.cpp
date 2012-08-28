/*****************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                    *
*   Copyright © 2009-2012 José Manuel Santamaría Lema <panfaust@gmail.com>   *
*                                                                            *
*   This program is free software; you can redistribute it and/or modify     *
*   it under the terms of the GNU General Public License as published by     *
*   the Free Software Foundation; either version 2 of the License, or        *
*   (at your option) any later version.                                      *
******************************************************************************/

#include "kreauthoractionshandler.h"

#include <KLocale>
#include <KMessageBox>
#include <QPointer>
#include <QModelIndex>

#include "datablocks/elementlist.h"
#include "backends/recipedb.h"
#include "widgets/kregenericlistwidget.h"
#include "dialogs/createelementdialog.h"
#include "dialogs/dependanciesdialog.h"


KreAuthorActionsHandler::KreAuthorActionsHandler( KreGenericListWidget * listWidget, RecipeDB * db ):
	KreGenericActionsHandler( listWidget, db )
{
}

void KreAuthorActionsHandler::createNew()
{
	QPointer<CreateElementDialog> elementDialog = new CreateElementDialog( m_listWidget, i18n( "New Author" ) );

	if ( elementDialog->exec() == QDialog::Accepted ) {
		QString result = elementDialog->newElementName();

		// Create the new author in the database only 
		// if it fits in the current length bounds, 
		// otherwise display an error message.
		if ( checkBounds( result ) )
			m_database->createNewAuthor( result );
	}
	delete elementDialog;
}

void KreAuthorActionsHandler::remove()
{
	int id = m_listWidget->selectedRowId();
	if ( id == -1 )
		return;

	ElementList recipeDependancies;
	m_database->findUseOfAuthorInRecipes( &recipeDependancies, id );

	//Check the current usage of the author in existing recipes, if it's used warn
	//that the recipes using that author will be modified, otherwise just say that
	//the selected author will be removed.
	if ( recipeDependancies.isEmpty() ) {
		switch ( KMessageBox::warningContinueCancel(
			m_listWidget,
			i18n( "Are you sure you want to delete this author?" ) ) ) {
			case KMessageBox::Continue:
				m_database->removeAuthor( id );
				break;
		}
		return;
	} else {
		ListInfo info;
		info.list = recipeDependancies;
		info.name = i18n("Recipes");

		QPointer<DependanciesDialog> warnDialog =
			new DependanciesDialog( m_listWidget, info, false );
		if ( warnDialog->exec() == QDialog::Accepted )
			m_database->removeAuthor( id );

		delete warnDialog;
	}
}

bool KreAuthorActionsHandler::checkBounds( const QString &name )
{
	if ( name.length() > m_database->maxAuthorNameLength() ) {
		KMessageBox::error( m_listWidget,
		i18np( "Author name cannot be longer than 1 character.",
		"Author name cannot be longer than %1 characters." ,
		m_database->maxAuthorNameLength() ));
		return false;
	}

	return true;
}

void KreAuthorActionsHandler::saveElement( const QModelIndex & topLeft,
	const QModelIndex & bottomRight )
{
	//Not used parameters.
	Q_UNUSED( bottomRight )

	//Revert the changed text if the new name is longer than the maximum length. 
	QString newAuthorName = topLeft.data().toString();
	if ( !checkBounds(newAuthorName) ) {
		m_listWidget->reload(ForceReload);
		return;
	}

	//Check if the new author name already exists in the database,
	//if so merge the two; otherwise just change the author in the
	//database.
	int existing_id = m_database->findExistingAuthorByName( newAuthorName );
	int author_id = m_listWidget->selectedRowId();
	if ( existing_id != -1 && existing_id != author_id )  //author already exists with this label... merge the two
	{
		switch ( KMessageBox::warningContinueCancel( m_listWidget,
		i18n( "This author already exists. Continuing will merge these two authors into one. Are you sure?" ) ) ) {
		case KMessageBox::Continue: {
				m_database->mergeAuthors( existing_id, author_id );
				break;
			}
		default:
			m_listWidget->reload(ForceReload);
			break;
		}
	} else {
		m_database->modAuthor( author_id, newAuthorName );
	}
}
