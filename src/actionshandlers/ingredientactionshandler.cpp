/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2008 Montel Laurent <montel@kde.org>                      *
*   Copyright © 2009 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "ingredientactionshandler.h"

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


IngredientActionsHandler::IngredientActionsHandler( DBListViewBase *_parentListView, RecipeDB *db ):
	ActionsHandlerBase( _parentListView, db )
{
}

void IngredientActionsHandler::createNew()
{
	QPointer<CreateElementDialog> elementDialog = new CreateElementDialog( parentListView, i18n( "New Ingredient" ) );

	if ( elementDialog->exec() == QDialog::Accepted ) {
		QString result = elementDialog->newElementName();

		//check bounds first
		if ( checkBounds( result ) )
			database->createNewIngredient( result ); // Create the new ingredient in the database
	}
	delete elementDialog;
}

void IngredientActionsHandler::remove()
{
	Q3ListViewItem * item = parentListView->currentItem();

	if ( item ) {
		int id = item->text( 1 ).toInt();

		ElementList dependingRecipes;
		database->findIngredientDependancies( id, &dependingRecipes );

		if ( dependingRecipes.isEmpty() )
			database->removeIngredient( id );
		else { // Need Warning!
			ListInfo list;
			list.list = dependingRecipes;
			list.name = i18n( "Recipes" );

			QPointer<DependanciesDialog> warnDialog = new DependanciesDialog( parentListView, list );
			warnDialog->setCustomWarning( i18n("You are about to permanantly delete recipes from your database.") );
			if ( warnDialog->exec() == QDialog::Accepted )
				database->removeIngredient( id );
			delete warnDialog;
		}
	}
}

bool IngredientActionsHandler::checkBounds( const QString &name )
{
	if ( name.length() > int(database->maxIngredientNameLength()) ) {
		KMessageBox::error( parentListView,
		i18np( "Ingredient name cannot be longer than 1 character.",
		"Ingredient name cannot be longer than %1 characters.",
		database->maxIngredientNameLength() ) );
		return false;
	}

	return true;

}

void IngredientActionsHandler::saveElement( Q3ListViewItem* i )
{
	if ( !checkBounds( i->text( 0 ) ) ) {
		parentListView->reload(ForceReload); //reset the changed text
		return ;
	}

	int existing_id = database->findExistingIngredientByName( i->text( 0 ) );
	int ing_id = i->text( 1 ).toInt();
	if ( existing_id != -1 && existing_id != ing_id )  //ingredient already exists with this label... merge the two
	{
		switch ( KMessageBox::warningContinueCancel( parentListView,
		i18n( "This ingredient already exists. Continuing will merge these two ingredients into one. Are you sure?" ) ) )
		{
		case KMessageBox::Continue: {
				database->mergeIngredients( existing_id, ing_id );
				break;
			}
		default:
			parentListView->reload(ForceReload);
			break;
		}
	}
	else {
		database->modIngredient( ( i->text( 1 ) ).toInt(), i->text( 0 ) );
	}
}
