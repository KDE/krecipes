/***************************************************************************
*   Copyright © 2004-2006 Jason Kivlighn <jkivlighn@gmail.com>             *
*   Copyright © 2004 Unai Garro <ugarro@gmail.com>                         *
*   Copyright © 2004 Cyril Bosselut <bosselut@b1project.com>               *
*   Copyright © 2009-2015 José Manuel Santamaría Lema <panfaust@gmail.com> *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
***************************************************************************/

#include "kreunitactionshandler.h"
#include "kregenericactionshandler.h"
#include "widgets/kreunitlistwidget.h"

//#include <kdebug.h>
#include <KLocale>
#include <KMessageBox>
#include <QModelIndex>
#include <QPointer>

#include "datablocks/elementlist.h"

#include "backends/recipedb.h"


#include "dialogs/createunitdialog.h"
#include "dialogs/dependanciesdialog.h"


KreUnitActionsHandler::KreUnitActionsHandler( KreUnitListWidget * listWidget, RecipeDB *db ):
	KreGenericActionsHandler( listWidget, db )
{
}

void KreUnitActionsHandler::createNew()
{
	QPointer<CreateUnitDialog> unitDialog = new CreateUnitDialog( m_listWidget );

	if ( unitDialog->exec() == QDialog::Accepted ) {
		Unit result = unitDialog->newUnit();

		//check bounds first
		if ( checkBounds( result )
		  && m_database->findExistingUnitByName( result.name() ) == -1
		  && m_database->findExistingUnitByName( result.plural() ) == -1
		) {
			m_database->createNewUnit( result );
		}
	}
	delete unitDialog;
}

Unit KreUnitActionsHandler::getSelectedUnit()
{
	//Create the unit object and populate it with the data of the row in question.
	Unit unit;
	unit.setId( m_listWidget->selectedRowData(0).toInt() );
	unit.setName( m_listWidget->selectedRowData(1).toString() );
	unit.setNameAbbrev( m_listWidget->selectedRowData(2).toString() );
	unit.setPlural( m_listWidget->selectedRowData(3).toString() );
	unit.setPluralAbbrev( m_listWidget->selectedRowData(4).toString() );
	unit.setType( (Unit::Type)m_listWidget->selectedRowData(5,Qt::UserRole).toInt() );
	return unit;
}

void KreUnitActionsHandler::tryToSaveUnit(Unit unit)
{
	//Revert the change if any text exceeds the maximum allowed by the DBMS
	if ( !checkBounds(unit) ) {
		m_listWidget->reload( ForceReload );
		return;
	}

	//Check if the new unit name already exists in the database,
        //if so merge the two; otherwise just change the author in the
        //database.
	int existing_id = m_database->findExistingUnitByName( unit.name() );

	if ( (existing_id != -1) && (existing_id != unit.id()) ) { //unit already exists with this label... merge the two
		switch ( KMessageBox::warningContinueCancel( m_listWidget,
		i18n( "This unit already exists.  Continuing will merge these two units into one.  Are you sure?" ) ) ) {
		case KMessageBox::Continue: {
				m_database->modUnit( unit );
				m_database->mergeUnits( unit.id(), existing_id );
				break;
			}
		default:
			m_listWidget->reload(ForceReload);
			break;
		}
	}
	else {
		m_database->modUnit( unit );
	}
}

void KreUnitActionsHandler::rename()
{
	//Get the row of the selected unit
	int row = m_listWidget->currentRow();

	//If there is no unit selected, do nothing. This may happen when you press ctrl+R
	if (row == -1) {
		return;
	}

	//Create the dialog and fill it with the current values of the selected unit.
	Unit currentUnit = getSelectedUnit();
	QPointer<CreateUnitDialog> unitDialog = new CreateUnitDialog( m_listWidget,
		currentUnit.name(), currentUnit.nameAbbrev(), currentUnit.plural(), currentUnit.pluralAbbrev(),
		currentUnit.type(), false );
	unitDialog->setCaption( i18n("Rename Unit") );

	//Show the dialog and save the unit if it proceeds.
	if ( unitDialog->exec() == QDialog::Accepted ) {
		Unit newUnit = unitDialog->newUnit();
		newUnit.setId( currentUnit.id() );
		tryToSaveUnit( newUnit );	
	}

	delete unitDialog;
}


void KreUnitActionsHandler::remove()
{
	int unitID = m_listWidget->selectedRowId();
	if ( unitID == -1 )
		return;

	ElementList recipeDependancies, propertyDependancies, weightDependancies;
	m_database->findUnitDependancies( unitID, &propertyDependancies, &recipeDependancies, &weightDependancies );

	QList<ListInfo> lists;
	if ( !recipeDependancies.isEmpty() ) {
		ListInfo info;
		info.list = recipeDependancies;
		info.name = i18n("Recipes");
			lists << info;
	}
	if ( !propertyDependancies.isEmpty() ) {
		ListInfo info;
		info.list = propertyDependancies;
		info.name = i18n("Properties");
		lists << info;
	}
	if ( !weightDependancies.isEmpty() ) {
		ListInfo info;
		info.list = weightDependancies;
		info.name = i18n("Ingredient Weights");
		lists << info;
	}

	if ( lists.isEmpty() )
		m_database->removeUnit( unitID );
	else { // if they are recipes using the unit they are going to be removed too
		QPointer<DependanciesDialog> warnDialog = new DependanciesDialog( m_listWidget, lists );
		if ( !recipeDependancies.isEmpty() )
			warnDialog->setCustomWarning( i18n("You are about to permanently delete recipes from your database.") );
		if ( warnDialog->exec() == QDialog::Accepted )
			m_database->removeUnit( unitID );
		delete warnDialog;
	}
}


bool KreUnitActionsHandler::checkBounds( const Unit &unit )
{
	if ( unit.name().length() > m_database->maxUnitNameLength() ||
	unit.nameAbbrev().length() > m_database->maxUnitNameLength() ||
	unit.plural().length() > m_database->maxUnitNameLength() ||
	unit.pluralAbbrev().length() > m_database->maxUnitNameLength() ) {
		KMessageBox::error( m_listWidget,
		i18np( "Unit name cannot be longer than 1 character.",
		"Unit name cannot be longer than %1 characters.",
		m_database->maxUnitNameLength() ) );
		return false;
	}
	else if ( unit.name().trimmed().isEmpty() )
		return false;

	return true;
}


void KreUnitActionsHandler::saveElement( const QModelIndex & topLeft, 
		const QModelIndex & bottomRight)
{
	//Not used parameters
	Q_UNUSED(topLeft)
	Q_UNUSED(bottomRight)

	Unit unit = getSelectedUnit();
	tryToSaveUnit( unit );
}
