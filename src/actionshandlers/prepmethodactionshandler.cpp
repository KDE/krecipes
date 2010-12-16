/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2009 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "prepmethodactionshandler.h"

#include <KLocale>
#include <K3ListView>
#include <KMenu>
#include <KIcon>
#include <QPointer>
#include <KMessageBox>
class Q3ListViewItem;

#include "datablocks/elementlist.h"

#include "backends/recipedb.h"

#include "widgets/dblistviewbase.h"

#include "dialogs/createelementdialog.h"
#include "dialogs/dependanciesdialog.h"


PrepMethodActionsHandler::PrepMethodActionsHandler( DBListViewBase *_parentListView, RecipeDB *db ):
	ActionsHandlerBase( _parentListView, db )
{
}

void PrepMethodActionsHandler::createNew()
{
	QPointer<CreateElementDialog> elementDialog = new CreateElementDialog( parentListView, i18n( "New Preparation Method" ) );

	if ( elementDialog->exec() == QDialog::Accepted ) {
		QString result = elementDialog->newElementName();

		//check bounds first
		if ( checkBounds( result ) )
			database->createNewPrepMethod( result ); // Create the new prepMethod in the database
	}

	delete elementDialog;
}

void PrepMethodActionsHandler::remove()
{
	Q3ListViewItem * item = parentListView->currentItem();

	if ( item ) {
		ElementList dependingRecipes;
		int prepMethodID = item->text( 1 ).toInt();
		database->findPrepMethodDependancies( prepMethodID, &dependingRecipes );
		if ( dependingRecipes.isEmpty() )
			database->removePrepMethod( prepMethodID );
		else // Need Warning!
		{
			ListInfo info;
			info.list = dependingRecipes;
			info.name = i18n("Recipes");
			QPointer<DependanciesDialog> warnDialog = new DependanciesDialog( parentListView, info );
			warnDialog->setCustomWarning( i18n("You are about to permanently delete recipes from your database.") );
			if ( warnDialog->exec() == QDialog::Accepted )
				database->removePrepMethod( prepMethodID );
			delete warnDialog;
		}
	}
}

bool PrepMethodActionsHandler::checkBounds( const QString &name )
{
	if ( name.length() > int(database->maxPrepMethodNameLength()) ) {
		KMessageBox::error( parentListView,
		i18np( "Preparation method cannot be longer than 1 character.",
		"Preparation method cannot be longer than %1 characters." ,
		database->maxPrepMethodNameLength() ) );
		return false;
	}

	return true;

}

void PrepMethodActionsHandler::saveElement( Q3ListViewItem* i )
{
	if ( !checkBounds( i->text( 0 ) ) ) {
		parentListView->reload(ForceReload); //reset the changed text
		return ;
	}

	int existing_id = database->findExistingIngredientGroupByName( i->text( 0 ) );
	int id = i->text( 1 ).toInt();
	if ( existing_id != -1 && existing_id != id )  //already exists with this label... merge the two
	{
		switch ( KMessageBox::warningContinueCancel( parentListView,
		i18n( "This header already exists.  Continuing will merge these two headers into one.  Are you sure?" ) ) )
		{
		case KMessageBox::Continue: {
				database->modIngredientGroup( id, i->text( 0 ) );
				database->mergeIngredientGroups( id, existing_id );
				break;
			}
		default:
			parentListView->reload(ForceReload);
			break;
		}
	}
	else {
		database->modIngredientGroup( id, i->text( 0 ) );
	}
}
