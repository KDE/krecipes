/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2009 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "authoractionshandler.h"

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


AuthorActionsHandler::AuthorActionsHandler( DBListViewBase *_parentListView, RecipeDB *db ):
	ActionsHandlerBase( _parentListView, db )
{
}

void AuthorActionsHandler::createNew()
{
	QPointer<CreateElementDialog> elementDialog = new CreateElementDialog( parentListView, i18n( "New Author" ) );

	if ( elementDialog->exec() == QDialog::Accepted ) {
		QString result = elementDialog->newElementName();

		//check bounds first
		if ( checkBounds( result ) )
			database->createNewAuthor( result ); // Create the new author in the database
	}
	delete elementDialog;
}

void AuthorActionsHandler::remove()
{
	Q3ListViewItem * item = parentListView->currentItem();

	if ( item ) {
		int id = item->text( 1 ).toInt();

		ElementList recipeDependancies;
		database->findUseOfAuthorInRecipes( &recipeDependancies, id );

		if ( recipeDependancies.isEmpty() ) {
			switch ( KMessageBox::warningContinueCancel(
				parentListView,
				i18n( "Are you sure you want to delete this author?" ) ) )
			{
				case KMessageBox::Continue:
					database->removeAuthor( id );
					break;
			}
			return;
		}
		else { // need warning!
			ListInfo info;
			info.list = recipeDependancies;
			info.name = i18n("Recipes");

			QPointer<DependanciesDialog> warnDialog =
				new DependanciesDialog( parentListView, info, false );
			if ( warnDialog->exec() == QDialog::Accepted )
				database->removeAuthor( id );

			delete warnDialog;
		}
	}

}

bool AuthorActionsHandler::checkBounds( const QString &name )
{
	if ( name.length() > int(database->maxAuthorNameLength()) ) {
		KMessageBox::error( parentListView,
		i18np( "Author name cannot be longer than 1 character.",
		"Author name cannot be longer than %1 characters." ,
		database->maxAuthorNameLength() ));
		return false;
	}

	return true;
}

void AuthorActionsHandler::saveElement( Q3ListViewItem* i )
{
	if ( !checkBounds( i->text( 0 ) ) ) {
		parentListView->reload(ForceReload); //reset the changed text
		return ;
	}

	int existing_id = database->findExistingAuthorByName( i->text( 0 ) );
	int author_id = i->text( 1 ).toInt();
	if ( existing_id != -1 && existing_id != author_id )  //author already exists with this label... merge the two
	{
		switch ( KMessageBox::warningContinueCancel( parentListView,
		i18n( "This author already exists. Continuing will merge these two authors into one. Are you sure?" ) ) )
		{
		case KMessageBox::Continue: {
				database->mergeAuthors( existing_id, author_id );
				break;
			}
		default:
			parentListView->reload(ForceReload);
			break;
		}
	}
	else {
		database->modAuthor( ( i->text( 1 ) ).toInt(), i->text( 0 ) );
	}
}
