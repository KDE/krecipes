/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2004 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2004 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2009 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "headeractionshandler.h"

#include <KLocale>
#include <K3ListView>
#include <KMenu>
#include <QPointer>
#include <KMessageBox>
class Q3ListViewItem;

#include "datablocks/elementlist.h"

#include "backends/recipedb.h"

#include "widgets/dblistviewbase.h"

#include "dialogs/createelementdialog.h"
#include "dialogs/dependanciesdialog.h"


HeaderActionsHandler::HeaderActionsHandler( DBListViewBase *_parentListView, RecipeDB *db ):
	ActionsHandlerBase( _parentListView, db )
{
}

void HeaderActionsHandler::createNew()
{
	QPointer<CreateElementDialog> elementDialog = new CreateElementDialog( parentListView, i18n( "New Header" ) );

	if ( elementDialog->exec() == QDialog::Accepted ) {
		QString result = elementDialog->newElementName();

		//check bounds first
		if ( checkBounds( result ) )
			database->createNewIngGroup( result ); // Create the new header in the database
	}
	delete elementDialog;
}

void HeaderActionsHandler::remove()
{
	Q3ListViewItem * item = parentListView->currentItem();

	if ( item ) {
		int id = item->text( 1 ).toInt();

		ElementList dependingRecipes;
		database->findUseOfIngGroupInRecipes( &dependingRecipes, id);

		if ( dependingRecipes.isEmpty() )
			database->removeIngredientGroup( id );
		else { // Need Warning!
			ListInfo list;
			list.list = dependingRecipes;
			list.name = i18n( "Recipes" );

			QPointer<DependanciesDialog> warnDialog = new DependanciesDialog( parentListView, list, false );
			if ( warnDialog->exec() == QDialog::Accepted )
				database->removeIngredientGroup( id );
			delete warnDialog;
		}
	}
}

bool HeaderActionsHandler::checkBounds( const QString &name )
{
	if ( name.length() > int(database->maxIngGroupNameLength()) ) {
		KMessageBox::error( parentListView,
		i18np( "Header cannot be longer than 1 character.", "Header cannot be longer than %1 characters.",
		database->maxIngGroupNameLength() ) );
		return false;
	}
	else if ( name.trimmed().isEmpty() )
		return false;

	return true;

}

void HeaderActionsHandler::saveElement( Q3ListViewItem* i )
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
