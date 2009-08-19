/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2009 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "propertyactionshandler.h"

#include <KLocale>
#include <K3ListView>
#include <KMenu>
#include <QPointer>
#include <KMessageBox>
class Q3ListViewItem;

#include "datablocks/elementlist.h"

#include "backends/recipedb.h"

#include "widgets/dblistviewbase.h"

#include "dialogs/createpropertydialog.h"
#include "dialogs/dependanciesdialog.h"


PropertyActionsHandler::PropertyActionsHandler( DBListViewBase *_parentListView, RecipeDB *db ):
	ActionsHandlerBase( _parentListView, db )
{
}

void PropertyActionsHandler::createNew()
{
	UnitList list;
	database->loadUnits( &list );
	QPointer<CreatePropertyDialog> propertyDialog = new CreatePropertyDialog( parentListView, &list );

	if ( propertyDialog->exec() == QDialog::Accepted ) {
		QString name = propertyDialog->newPropertyName();
		QString units = propertyDialog->newUnitsName();
		if ( !( ( name.isEmpty() ) || ( units.isEmpty() ) ) )  // Make sure none of the fields are empty
		{
			//check bounds first
			if ( checkBounds( name ) )
				database->addProperty( name, units );
		}
	}
	delete propertyDialog;
}

void PropertyActionsHandler::remove()
{
	Q3ListViewItem * item = parentListView->currentItem();

	if ( item ) {
		switch ( KMessageBox::warningContinueCancel( parentListView,
			i18n( "Are you sure you want to delete this property?" ) ) ) {
		case KMessageBox::Continue:
			database->removeProperty( item->text( 2 ).toInt() );
			break;
		default:
			break;
		}
	}
}

bool PropertyActionsHandler::checkBounds( const QString &name )
{
	if ( name.length() > (int)database->maxPropertyNameLength() ) {
		KMessageBox::error( parentListView,
			i18np( "Property name cannot be longer than 1 character.",
			"Property name cannot be longer than %1 characters." ,
			database->maxPropertyNameLength() ) );
		return false;
	}
	return true;
}

void PropertyActionsHandler::saveElement( Q3ListViewItem* i )
{
	if ( !checkBounds( i->text( 0 ) ) ) {
		parentListView->reload(); //reset the changed text
		return ;
	}
	int existing_id = database->findExistingPropertyByName( i->text( 0 ) );
	int prop_id = i->text( 2 ).toInt();
	if ( existing_id != -1 && existing_id != prop_id )  //already exists with this label... merge the two
	{
		switch ( KMessageBox::warningContinueCancel( parentListView,
		i18n( "This property already exists.  Continuing will merge these two properties into one.  Are you sure?" ) ) )
		{
		case KMessageBox::Continue: {
				database->mergeProperties( existing_id, prop_id );
				break;
			}
		default:
			parentListView->reload();
			break;
		}
	}
	else
		database->modProperty( prop_id, i->text( 0 ) );
}
