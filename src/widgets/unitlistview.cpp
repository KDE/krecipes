/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "unitlistview.h"

#include <kmessagebox.h>
#include <kconfig.h>
#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kpopupmenu.h>

#include "DBBackend/recipedb.h"
#include "dialogs/createunitdialog.h"
#include "dialogs/dependanciesdialog.h"
#include "datablocks/unit.h"

UnitListView::UnitListView( QWidget *parent, RecipeDB *db ) : KListView( parent ),
		database( db )
{
	connect( database, SIGNAL( unitCreated( const Unit & ) ), SLOT( createUnit( const Unit & ) ) );
	connect( database, SIGNAL( unitRemoved( int ) ), SLOT( removeUnit( int ) ) );

	setAllColumnsShowFocus( true );
	setDefaultRenameAction( QListView::Reject );
}

void UnitListView::reload()
{
	clear();

	UnitList unitList;
	database->loadUnits( &unitList );

	for ( UnitList::const_iterator it = unitList.begin(); it != unitList.end(); ++it ) {
		if ( !( *it ).name.isEmpty() || !( *it ).plural.isEmpty() )
			createUnit( *it );
	}
}



StdUnitListView::StdUnitListView( QWidget *parent, RecipeDB *db, bool editable ) : UnitListView( parent, db )
{
	KConfig * config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry( "ShowID", false );
	addColumn( i18n( "Id" ), show_id ? -1 : 0 );

	addColumn( i18n( "Unit" ) );
	addColumn( i18n( "Plural" ) );

	setSorting( 1 );

	if ( editable ) {
		setRenameable( 1, true );
		setRenameable( 2, true );

		KIconLoader *il = new KIconLoader;

		kpop = new KPopupMenu( this );
		kpop->insertItem( il->loadIcon( "filenew", KIcon::NoGroup, 16 ), i18n( "&Create" ), this, SLOT( createNew() ), CTRL + Key_C );
		kpop->insertItem( il->loadIcon( "editdelete", KIcon::NoGroup, 16 ), i18n( "&Delete" ), this, SLOT( remove
			                  () ), Key_Delete );
		kpop->insertItem( il->loadIcon( "edit", KIcon::NoGroup, 16 ), i18n( "&Rename" ), this, SLOT( rename() ), CTRL + Key_R );
		kpop->polish();

		delete il;

		connect( this, SIGNAL( contextMenu( KListView *, QListViewItem *, const QPoint & ) ), SLOT( showPopup( KListView *, QListViewItem *, const QPoint & ) ) );
		connect( this, SIGNAL( doubleClicked( QListViewItem*, const QPoint &, int ) ), this, SLOT( modUnit( QListViewItem*, const QPoint &, int ) ) );
		connect( this, SIGNAL( itemRenamed( QListViewItem*, const QString &, int ) ), this, SLOT( saveUnit( QListViewItem*, const QString &, int ) ) );
	}
}

void StdUnitListView::showPopup( KListView * /*l*/, QListViewItem *i, const QPoint &p )
{
	if ( i )
		kpop->exec( p );
}

void StdUnitListView::createNew()
{
	CreateUnitDialog * unitDialog = new CreateUnitDialog( this );

	if ( unitDialog->exec() == QDialog::Accepted ) {
		Unit result = unitDialog->newUnit();

		//check bounds first
		if ( checkBounds( result ) )
			database->createNewUnit( result.name, result.plural );
	}
	delete unitDialog;
}

void StdUnitListView::remove
	()
{
	// Find selected unit item
	QListViewItem * it = currentItem();

	if ( it ) {
		int unitID = it->text( 0 ).toInt();

		ElementList recipeDependancies, propertyDependancies;
		database->findUnitDependancies( unitID, &propertyDependancies, &recipeDependancies );

		if ( recipeDependancies.isEmpty() && propertyDependancies.isEmpty() )
			database->removeUnit( unitID );
		else { // need warning!
			DependanciesDialog *warnDialog = new DependanciesDialog( 0, &recipeDependancies, 0, &propertyDependancies );
			if ( warnDialog->exec() == QDialog::Accepted )
				database->removeUnit( unitID );
			delete warnDialog;
		}
	}
}

void StdUnitListView::rename()
{
	QListViewItem * item = currentItem();

	if ( item )
		UnitListView::rename( item, 1 );
}

void StdUnitListView::createUnit( const Unit &unit )
{
	( void ) new QListViewItem( this, QString::number( unit.id ), unit.name, unit.plural );
}

void StdUnitListView::removeUnit( int id )
{
	QListViewItem * item = findItem( QString::number( id ), 0 );

	Q_ASSERT( item );

	delete item;
}

void StdUnitListView::modUnit( QListViewItem* i, const QPoint & /*p*/, int c )
{
	if ( i )
		UnitListView::rename( i, c );
}

void StdUnitListView::saveUnit( QListViewItem* i, const QString &text, int c )
{
	if ( !checkBounds( Unit( text, text ) ) ) {
		reload(); //reset the changed text
		return ;
	}

	int existing_id = database->findExistingUnitByName( text );
	int unit_id = i->text( 0 ).toInt();
	if ( existing_id != -1 && existing_id != unit_id ) { //category already exists with this label... merge the two
		switch ( KMessageBox::warningContinueCancel( this, i18n( "This unit already exists.  Continuing will merge these two units into one.  Are you sure?" ) ) ) {
		case KMessageBox::Continue: {
				database->mergeUnits( existing_id, unit_id );
				break;
			}
		default:
			reload();
			break;
		}
	}
	else {
		database->modUnit( ( i->text( 0 ) ).toInt(), i->text( 1 ), i->text( 2 ) );
	}
}

bool StdUnitListView::checkBounds( const Unit &unit )
{
	if ( unit.name.length() > database->maxUnitNameLength() || unit.plural.length() > database->maxUnitNameLength() ) {
		KMessageBox::error( this, QString( i18n( "Unit name cannot be longer than %1 characters." ) ).arg( database->maxUnitNameLength() ) );
		return false;
	}

	return true;
}

#include "unitlistview.moc"
