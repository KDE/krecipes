/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "prepmethodlistview.h"

#include <kmessagebox.h>
#include <kconfig.h>
#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kpopupmenu.h>

#include "DBBackend/recipedb.h"
#include "dialogs/createelementdialog.h"
#include "dialogs/dependanciesdialog.h"

PrepMethodListView::PrepMethodListView( QWidget *parent, RecipeDB *db ) : KListView( parent ),
		database( db )
{
	connect( database, SIGNAL( prepMethodCreated( const Element & ) ), SLOT( createPrepMethod( const Element & ) ) );
	connect( database, SIGNAL( prepMethodRemoved( int ) ), SLOT( removePrepMethod( int ) ) );

	setAllColumnsShowFocus( true );
	setDefaultRenameAction( QListView::Reject );
}

void PrepMethodListView::reload()
{
	clear();

	ElementList prepMethodList;
	database->loadPrepMethods( &prepMethodList );

	for ( ElementList::const_iterator ing_it = prepMethodList.begin(); ing_it != prepMethodList.end(); ++ing_it )
		createPrepMethod( *ing_it );
}



StdPrepMethodListView::StdPrepMethodListView( QWidget *parent, RecipeDB *db, bool editable ) : PrepMethodListView( parent, db )
{
	KConfig * config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry( "ShowID", false );
	addColumn( i18n( "Id" ), show_id ? -1 : 0 );

	addColumn( i18n( "Preparation Method" ) );

	setSorting( 1 );

	if ( editable ) {
		setRenameable( 1, true );

		KIconLoader *il = new KIconLoader;

		kpop = new KPopupMenu( this );
		kpop->insertItem( il->loadIcon( "filenew", KIcon::NoGroup, 16 ), i18n( "&Create" ), this, SLOT( createNew() ), CTRL + Key_C );
		kpop->insertItem( il->loadIcon( "editdelete", KIcon::NoGroup, 16 ), i18n( "&Delete" ), this, SLOT( remove
			                  () ), Key_Delete );
		kpop->insertItem( il->loadIcon( "edit", KIcon::NoGroup, 16 ), i18n( "&Rename" ), this, SLOT( rename() ), CTRL + Key_R );
		kpop->polish();

		delete il;

		connect( this, SIGNAL( contextMenu( KListView *, QListViewItem *, const QPoint & ) ), SLOT( showPopup( KListView *, QListViewItem *, const QPoint & ) ) );
		connect( this, SIGNAL( doubleClicked( QListViewItem* ) ), this, SLOT( modPrepMethod( QListViewItem* ) ) );
		connect( this, SIGNAL( itemRenamed( QListViewItem* ) ), this, SLOT( savePrepMethod( QListViewItem* ) ) );
	}
}

void StdPrepMethodListView::showPopup( KListView * /*l*/, QListViewItem *i, const QPoint &p )
{
	if ( i )
		kpop->exec( p );
}

void StdPrepMethodListView::createNew()
{
	CreateElementDialog * elementDialog = new CreateElementDialog( this, i18n( "New Preparation Method" ) );

	if ( elementDialog->exec() == QDialog::Accepted ) {
		QString result = elementDialog->newElementName();

		//check bounds first
		if ( checkBounds( result ) )
			database->createNewPrepMethod( result ); // Create the new prepMethod in the database
	}
}

void StdPrepMethodListView::remove
	()
{
	QListViewItem * item = currentItem();

	if ( item ) {
		ElementList dependingRecipes;
		int prepMethodID = item->text( 0 ).toInt();
		database->findPrepMethodDependancies( prepMethodID, &dependingRecipes );
		if ( dependingRecipes.isEmpty() )
			database->removePrepMethod( prepMethodID );
		else // Need Warning!
		{
			DependanciesDialog *warnDialog = new DependanciesDialog( this, &dependingRecipes );
			if ( warnDialog->exec() == QDialog::Accepted )
				database->removePrepMethod( prepMethodID );
			delete warnDialog;
		}
	}
}

void StdPrepMethodListView::rename()
{
	QListViewItem * item = currentItem();

	if ( item )
		PrepMethodListView::rename( item, 1 );
}

void StdPrepMethodListView::createPrepMethod( const Element &ing )
{
	( void ) new QListViewItem( this, QString::number( ing.id ), ing.name );
}

void StdPrepMethodListView::removePrepMethod( int id )
{
	QListViewItem * item = findItem( QString::number( id ), 0 );

	Q_ASSERT( item );

	delete item;
}

void StdPrepMethodListView::modPrepMethod( QListViewItem* i )
{
	if ( i )
		PrepMethodListView::rename( i, 1 );
}

void StdPrepMethodListView::savePrepMethod( QListViewItem* i )
{
	if ( !checkBounds( i->text( 1 ) ) ) {
		reload(); //reset the changed text
		return ;
	}

	int existing_id = database->findExistingPrepByName( i->text( 1 ) );
	int prep_id = i->text( 0 ).toInt();
	if ( existing_id != -1 && existing_id != prep_id )  //category already exists with this label... merge the two
	{
		switch ( KMessageBox::warningContinueCancel( this, i18n( "This preparation method already exists.  Continuing will merge these two into one.  Are you sure?" ) ) )
		{
		case KMessageBox::Continue: {
				database->mergePrepMethods( existing_id, prep_id );
				break;
			}
		default:
			reload();
			break;
		}
	}
	else {
		database->modPrepMethod( ( i->text( 0 ) ).toInt(), i->text( 1 ) );
	}
}

bool StdPrepMethodListView::checkBounds( const QString &name )
{
	if ( name.length() > database->maxPrepMethodNameLength() ) {
		KMessageBox::error( this, QString( i18n( "Preparation method cannot be longer than %1 characters." ) ).arg( database->maxPrepMethodNameLength() ) );
		return false;
	}

	return true;
}

#include "prepmethodlistview.moc"
