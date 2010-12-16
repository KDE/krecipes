/***************************************************************************
*   Copyright © 2004-2006 Jason Kivlighn <jkivlighn@gmail.com>            *
*   Copyright © 2004 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2004 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2009 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "unitactionshandler.h"

#include <KLocale>
#include <K3ListView>
#include <KMenu>
#include <KIcon>
#include <QPointer>
#include <QString>
#include <KMessageBox>
class Q3ListViewItem;

#include "datablocks/elementlist.h"

#include "backends/recipedb.h"

#include "widgets/unitlistview.h"

#include "dialogs/createunitdialog.h"
#include "dialogs/dependanciesdialog.h"


UnitActionsHandler::UnitActionsHandler( StdUnitListView *_parentListView, RecipeDB *db ):
	ActionsHandlerBase( _parentListView, db )
{
	connect( parentListView,
		SIGNAL( itemRenamed( Q3ListViewItem*, const QString &, int ) ),
		SLOT( saveUnit( Q3ListViewItem*, const QString &, int ) )
	);
}

void UnitActionsHandler::createNew()
{
	QPointer<CreateUnitDialog> unitDialog = new CreateUnitDialog( parentListView );

	if ( unitDialog->exec() == QDialog::Accepted ) {
		Unit result = unitDialog->newUnit();

		//check bounds first
		if ( checkBounds( result )
		  && database->findExistingUnitByName( result.name() ) == -1
		  && database->findExistingUnitByName( result.plural() ) == -1
		) {
			database->createNewUnit( result );
		}
	}
	delete unitDialog;
}

void UnitActionsHandler::rename()
{
	UnitListViewItem * item = (UnitListViewItem*)parentListView->currentItem();

	if ( item ) {
		QPointer<CreateUnitDialog> unitDialog = new CreateUnitDialog( 
			parentListView, item->text(0), item->text(2), item->text(1), item->text(3),
			item->type(), false );
		unitDialog->setCaption( i18n("Rename Unit") );

		if ( unitDialog->exec() == QDialog::Accepted ) {
			UnitListViewItem *unit_item = (UnitListViewItem*)item;
			Unit origUnit = unit_item->unit();
			Unit newUnit = unitDialog->newUnit();

			//for each changed entry, save the change individually

			Unit unit = origUnit;

			if ( newUnit.name() != origUnit.name() ) {
				unit.setName(newUnit.name());
				unit_item->setUnit( unit );
				saveUnit( unit_item, newUnit.name(), 0 );

				//saveUnit will call database->modUnit which deletes the list item we were using
				unit_item = (UnitListViewItem*) parentListView->findItem( QString::number(unit.id()), 5 );
			}

			if ( newUnit.plural() != origUnit.plural() ) {
				unit.setPlural(newUnit.plural());
				unit_item->setUnit( unit );
				saveUnit( unit_item, newUnit.plural(), 2 );
				unit_item = (UnitListViewItem*) parentListView->findItem( QString::number(unit.id()), 5 );
			}

			if ( !newUnit.nameAbbrev().trimmed().isEmpty() && newUnit.nameAbbrev() != origUnit.nameAbbrev() ) {
				unit.setNameAbbrev(newUnit.nameAbbrev());
				unit_item->setUnit( unit );
				saveUnit( unit_item, newUnit.nameAbbrev(), 1 );
				unit_item = (UnitListViewItem*) parentListView->findItem( QString::number(unit.id()), 5 );
			}
			if ( !newUnit.pluralAbbrev().trimmed().isEmpty() && newUnit.pluralAbbrev() != origUnit.pluralAbbrev() ) {
				unit.setPluralAbbrev(newUnit.pluralAbbrev());
				unit_item->setUnit( unit );
				saveUnit( unit_item, newUnit.pluralAbbrev(), 3 );
				unit_item = (UnitListViewItem*) parentListView->findItem( QString::number(unit.id()), 5 );
			}
			if ( newUnit.type() != unit.type() ) {
				unit.setType(newUnit.type());
				unit_item->setUnit( unit );
				saveUnit( unit_item, unit_item->text(4), 5 );
			}
		}
		delete unitDialog;
	}
}

void UnitActionsHandler::remove()
{
	// Find selected unit item
	UnitListViewItem* it = (UnitListViewItem*) parentListView->currentItem();

	if ( it ) {
		int unitID = it->unit().id();

		ElementList recipeDependancies, propertyDependancies, weightDependancies;
		database->findUnitDependancies( unitID, &propertyDependancies, &recipeDependancies, &weightDependancies );

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
			database->removeUnit( unitID );
		else { // need warning!
			QPointer<DependanciesDialog> warnDialog = new DependanciesDialog( parentListView, lists );
			if ( !recipeDependancies.isEmpty() )
				warnDialog->setCustomWarning( i18n("You are about to permanently delete recipes from your database.") );
			if ( warnDialog->exec() == QDialog::Accepted )
				database->removeUnit( unitID );
			delete warnDialog;
		}
	}
}

bool UnitActionsHandler::checkBounds( const Unit &unit )
{
	if ( unit.name().length() > int(database->maxUnitNameLength()) ||
	unit.plural().length() > int(database->maxUnitNameLength()) ) {
		KMessageBox::error( parentListView,
		i18np( "Unit name cannot be longer than 1 character.",
		"Unit name cannot be longer than %1 characters.",
		database->maxUnitNameLength() ) );
		return false;
	}
	else if ( unit.name().trimmed().isEmpty() || unit.plural().trimmed().isEmpty() )
		return false;

	return true;
}

void UnitActionsHandler::renameElement( Q3ListViewItem* i, const QPoint & /*p*/, int c )
{
	if ( i ) {
		if ( c != 4 )
			parentListView->rename( i, c );
		else {
			((StdUnitListView*)parentListView)->insertTypeComboBox(i);
		}
	}
}

void UnitActionsHandler::saveUnit( Q3ListViewItem* i, const QString &text, int c )
{
	//skip abbreviations
	if ( c == 0 || c == 2 ) {
		if ( !checkBounds( Unit( text, text ) ) ) {
			parentListView->reload(ForceReload); //reset the changed text
			return ;
		}
	}

	int existing_id = database->findExistingUnitByName( text );

	UnitListViewItem *unit_it = (UnitListViewItem*)i;
	int unit_id = unit_it->unit().id();
	if ( existing_id != -1 && existing_id != unit_id && !text.trimmed().isEmpty() ) { //unit already exists with this label... merge the two
		switch ( KMessageBox::warningContinueCancel( parentListView,
		i18n( "This unit already exists.  Continuing will merge these two units into one.  Are you sure?" ) ) ) {
		case KMessageBox::Continue: {
				database->modUnit( unit_it->unit() );
				database->mergeUnits( unit_id, existing_id );
				break;
			}
		default:
			parentListView->reload(ForceReload);
			break;
		}
	}
	else {
		database->modUnit( unit_it->unit() );
	}
}
