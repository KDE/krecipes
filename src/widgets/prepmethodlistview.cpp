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

PrepMethodListView::PrepMethodListView( QWidget *parent, RecipeDB *db ) : DBListViewBase( parent,db,db->prepMethodCount())
{
	connect( database, SIGNAL( prepMethodCreated( const Element & ) ), SLOT( checkCreatePrepMethod( const Element & ) ) );
	connect( database, SIGNAL( prepMethodRemoved( int ) ), SLOT( removePrepMethod( int ) ) );
	connect( database, SIGNAL( prepMethodCreated( const Element & ) ), SLOT( elementCreated() ) );
	connect( database, SIGNAL( prepMethodRemoved( int ) ), SLOT( elementRemoved() ) );

	setAllColumnsShowFocus( true );
	setDefaultRenameAction( QListView::Reject );
}

void PrepMethodListView::load( int limit, int offset )
{
	ElementList prepMethodList;
	database->loadPrepMethods( &prepMethodList, limit, offset );

	for ( ElementList::const_iterator ing_it = prepMethodList.begin(); ing_it != prepMethodList.end(); ++ing_it )
		createPrepMethod( *ing_it );
}

void PrepMethodListView::checkCreatePrepMethod( const Element &el )
{
	if ( handleElement(el.name) ) { //only create this prep method if the base class okays it
		createPrepMethod(el);
	}
}


StdPrepMethodListView::StdPrepMethodListView( QWidget *parent, RecipeDB *db, bool editable ) : PrepMethodListView( parent, db )
{
	addColumn( i18n( "Preparation Method" ) );

	KConfig * config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry( "ShowID", false );
	addColumn( i18n( "Id" ), show_id ? -1 : 0 );

	if ( editable ) {
		setRenameable( 0, true );

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
		int prepMethodID = item->text( 1 ).toInt();
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
		PrepMethodListView::rename( item, 0 );
}

void StdPrepMethodListView::createPrepMethod( const Element &ing )
{
	createElement(new QListViewItem( this, ing.name, QString::number( ing.id ) ));
}

void StdPrepMethodListView::removePrepMethod( int id )
{
	QListViewItem * item = findItem( QString::number( id ), 1 );
	delete item;
}

void StdPrepMethodListView::modPrepMethod( QListViewItem* i )
{
	if ( i )
		PrepMethodListView::rename( i, 0 );
}

void StdPrepMethodListView::savePrepMethod( QListViewItem* i )
{
	if ( !checkBounds( i->text( 0 ) ) ) {
		reload(); //reset the changed text
		return ;
	}

	int existing_id = database->findExistingPrepByName( i->text( 0 ) );
	int prep_id = i->text( 1 ).toInt();
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
		database->modPrepMethod( ( i->text( 1 ) ).toInt(), i->text( 0 ) );
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
