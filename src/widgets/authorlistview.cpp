/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "authorlistview.h"

#include <kmessagebox.h>
#include <kconfig.h>
#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kpopupmenu.h>

#include "DBBackend/recipedb.h"
#include "dialogs/createelementdialog.h"

AuthorListView::AuthorListView( QWidget *parent, RecipeDB *db ) : KListView( parent ),
		database( db )
{
	connect( database, SIGNAL( authorCreated( const Element & ) ), SLOT( createAuthor( const Element & ) ) );
	connect( database, SIGNAL( authorRemoved( int ) ), SLOT( removeAuthor( int ) ) );

	setAllColumnsShowFocus( true );
	setDefaultRenameAction( QListView::Reject );
}

void AuthorListView::reload()
{
	clear();

	ElementList authorList;KConfig * config = KGlobal::config();config->setGroup( "Advanced" );int limit = config->readNumEntry( "Limit", -1 );int offset = config->readNumEntry( "Offset", 0 );
	database->loadAuthors( &authorList, limit, offset );

	for ( ElementList::const_iterator ing_it = authorList.begin(); ing_it != authorList.end(); ++ing_it )
		createAuthor( *ing_it );
}



StdAuthorListView::StdAuthorListView( QWidget *parent, RecipeDB *db, bool editable ) : AuthorListView( parent, db )
{
	KConfig * config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry( "ShowID", false );
	addColumn( i18n( "Id" ), show_id ? -1 : 0 );

	addColumn( i18n( "Author" ) );

	setSorting( 1 );

	if ( editable ) {
		setRenameable( 1, true );

		KIconLoader *il = new KIconLoader;

		kpop = new KPopupMenu( this );
		kpop->insertItem( il->loadIcon( "filenew", KIcon::NoGroup, 16 ), i18n( "&Create" ), this, SLOT( createNew() ), CTRL + Key_N );
		kpop->insertItem( il->loadIcon( "editdelete", KIcon::NoGroup, 16 ), i18n( "&Delete" ), this, SLOT( remove
			                  () ), Key_Delete );
		kpop->insertItem( il->loadIcon( "edit", KIcon::NoGroup, 16 ), i18n( "&Rename" ), this, SLOT( rename() ), CTRL + Key_R );
		kpop->polish();

		delete il;

		connect( this, SIGNAL( contextMenu( KListView *, QListViewItem *, const QPoint & ) ), SLOT( showPopup( KListView *, QListViewItem *, const QPoint & ) ) );
		connect( this, SIGNAL( doubleClicked( QListViewItem* ) ), this, SLOT( modAuthor( QListViewItem* ) ) );
		connect( this, SIGNAL( itemRenamed( QListViewItem* ) ), this, SLOT( saveAuthor( QListViewItem* ) ) );
	}
}

void StdAuthorListView::showPopup( KListView * /*l*/, QListViewItem *i, const QPoint &p )
{
	if ( i )
		kpop->exec( p );
}

void StdAuthorListView::createNew()
{
	CreateElementDialog * elementDialog = new CreateElementDialog( this, i18n( "New Author" ) );

	if ( elementDialog->exec() == QDialog::Accepted ) {
		QString result = elementDialog->newElementName();

		//check bounds first
		if ( checkBounds( result ) )
			database->createNewAuthor( result ); // Create the new author in the database
	}
}

void StdAuthorListView::remove
	()
{
	QListViewItem * item = currentItem();

	if ( item ) {
		switch ( KMessageBox::warningContinueCancel( this, i18n( "Are you sure you want to delete this author?" ) ) ) {
		case KMessageBox::Continue:
			database->removeAuthor( item->text( 0 ).toInt() );
			break;
		default:
			break;
		}
	}
}

void StdAuthorListView::rename()
{
	QListViewItem * item = currentItem();

	if ( item )
		AuthorListView::rename( item, 1 );
}

void StdAuthorListView::createAuthor( const Element &ing )
{
	( void ) new QListViewItem( this, QString::number( ing.id ), ing.name );
}

void StdAuthorListView::removeAuthor( int id )
{
	QListViewItem * item = findItem( QString::number( id ), 0 );

	Q_ASSERT( item );

	delete item;
}

void StdAuthorListView::modAuthor( QListViewItem* i )
{
	if ( i )
		AuthorListView::rename( i, 1 );
}

void StdAuthorListView::saveAuthor( QListViewItem* i )
{
	if ( !checkBounds( i->text( 1 ) ) ) {
		reload(); //reset the changed text
		return ;
	}

	int existing_id = database->findExistingAuthorByName( i->text( 1 ) );
	int author_id = i->text( 0 ).toInt();
	if ( existing_id != -1 && existing_id != author_id )  //category already exists with this label... merge the two
	{
		switch ( KMessageBox::warningContinueCancel( this, i18n( "This author already exists.  Continuing will merge these two authors into one.  Are you sure?" ) ) )
		{
		case KMessageBox::Continue: {
				database->mergeAuthors( existing_id, author_id );
				break;
			}
		default:
			reload();
			break;
		}
	}
	else {
		database->modAuthor( ( i->text( 0 ) ).toInt(), i->text( 1 ) );
	}
}

bool StdAuthorListView::checkBounds( const QString &name )
{
	if ( name.length() > database->maxAuthorNameLength() ) {
		KMessageBox::error( this, QString( i18n( "Author name cannot be longer than %1 characters." ) ).arg( database->maxAuthorNameLength() ) );
		return false;
	}

	return true;
}

#include "authorlistview.moc"
