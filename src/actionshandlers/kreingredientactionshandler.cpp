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

#include "kreingredientactionshandler.h"

//#include <kdebug.h>

#include <KLocale>
#include <QPointer>
#include <KMessageBox>
#include <QModelIndex>

#include "backends/recipedb.h"

#include "widgets/kregenericlistwidget.h"

#include "dialogs/createelementdialog.h"
#include "dialogs/dependanciesdialog.h"


KreIngredientActionsHandler::KreIngredientActionsHandler( KreGenericListWidget * listWidget, RecipeDB * db ):
	KreGenericActionsHandler( listWidget, db )
{
}

void KreIngredientActionsHandler::createNew()
{
	QPointer<CreateElementDialog> elementDialog = new CreateElementDialog(
		m_listWidget, i18n( "New Ingredient" ) );

	if ( elementDialog->exec() == QDialog::Accepted ) {
		QString result = elementDialog->newElementName();

		// Check bounds first.
		if ( checkBounds( result ) ) {
			// Create the new ingredient in the database.
			m_database->createNewIngredient( result );
		}
	}
	delete elementDialog;
}

void KreIngredientActionsHandler::remove()
{
	int id = m_listWidget->selectedRowId();

	//If there is no element selected, do nothing
	if ( id == -1 ) {
		return;
	}

	//Delete the ingredient selected.
	ElementList dependingRecipes;
	m_database->findIngredientDependancies( id, &dependingRecipes );
	if ( dependingRecipes.isEmpty() ) {
		m_database->removeIngredient( id );
	} else { // Need Warning!
		ListInfo list;
		list.list = dependingRecipes;
		list.name = i18n( "Recipes" );

		QPointer<DependanciesDialog> warnDialog = new DependanciesDialog( m_listWidget, list );
		warnDialog->setCustomWarning( i18n("You are about to permanently delete recipes from your database.") );
		if ( warnDialog->exec() == QDialog::Accepted )
			m_database->removeIngredient( id );
		delete warnDialog;
	}
}


bool KreIngredientActionsHandler::checkBounds( const QString &name )
{
	if ( name.length() > m_database->maxIngredientNameLength() ) {
		KMessageBox::error( m_listWidget,
		i18np( "Ingredient name cannot be longer than 1 character.",
		"Ingredient name cannot be longer than %1 characters.",
		m_database->maxIngredientNameLength() ) );
		return false;
	} else {
		return true;
	}
}


void KreIngredientActionsHandler::saveElement( const QModelIndex & topLeft, 
		const QModelIndex & bottomRight )
{
	Q_UNUSED(bottomRight)
	QString ingredientName = topLeft.data().toString();
	if ( !checkBounds( ingredientName ) ) {
		m_listWidget->reload( ForceReload ); //reset the changed text
		return ;
	}

	int existing_id = m_database->findExistingIngredientByName( ingredientName );
	int id = m_listWidget->selectedRowId();
	if ( (existing_id != -1) && (existing_id != id) )  //ingredient already exists with this label... merge the two
	{
		switch ( KMessageBox::warningContinueCancel( m_listWidget,
		i18n( "This ingredient already exists. Continuing will merge these two ingredients into one. Are you sure?" ) ) )
		{
		case KMessageBox::Continue: {
				m_database->mergeIngredients( existing_id, id );
				break;
			}
		default:
			m_listWidget->reload( ForceReload );
			break;
		}
	}
	else {
		m_database->modIngredient( id, ingredientName );
	}
}
