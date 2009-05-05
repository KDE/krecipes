/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
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
#include <kmenu.h>
#include <QList>

#include "backends/recipedb.h"
#include "dialogs/createelementdialog.h"
#include "dialogs/dependanciesdialog.h"

AuthorListView::AuthorListView( QWidget *parent, RecipeDB *db ) : DBListViewBase( parent, db, db->authorCount() )
{
	setAllColumnsShowFocus( true );
	setDefaultRenameAction( Q3ListView::Reject );
}

void AuthorListView::init()
{
	connect( database, SIGNAL( authorCreated( const Element & ) ), SLOT( checkCreateAuthor( const Element & ) ) );
	connect( database, SIGNAL( authorRemoved( int ) ), SLOT( removeAuthor( int ) ) );
}

void AuthorListView::load( int limit, int offset )
{
	ElementList authorList;
	database->loadAuthors( &authorList, limit, offset );

	setTotalItems(authorList.count());

	for ( ElementList::const_iterator ing_it = authorList.begin(); ing_it != authorList.end(); ++ing_it )
		createAuthor( *ing_it );
}

void AuthorListView::checkCreateAuthor( const Element &el )
{
	if ( handleElement(el.name) ) { //only create this author if the base class okays it
		createAuthor(el);
	}
}


StdAuthorListView::StdAuthorListView( QWidget *parent, RecipeDB *db, bool editable ) : AuthorListView( parent, db )
{
	addColumn( i18n( "Author" ) );

	KConfigGroup config = KGlobal::config()->group( "Advanced" );
	bool show_id = config.readEntry( "ShowID", false );
	addColumn( i18n( "Id" ), show_id ? -1 : 0 );

	if ( editable ) {
		setRenameable( 0, true );

		KIconLoader *il = KIconLoader::global();

		kpop = new KMenu( this );
		kpop->addAction( il->loadIcon( "document-new", KIconLoader::NoGroup, 16 ), i18n( "&Create" ), this, SLOT( createNew() ), Qt::CTRL + Qt::Key_N );
		kpop->addAction( il->loadIcon( "edit-delete", KIconLoader::NoGroup, 16 ), i18n( "&Delete" ), this, SLOT( remove
			                  () ), Qt::Key_Delete );
		kpop->addAction( il->loadIcon( "edit-rename", KIconLoader::NoGroup, 16 ), i18n( "&Rename" ), this, SLOT( slotRename() ), Qt::CTRL + Qt::Key_R );
		kpop->ensurePolished();

		connect( this, SIGNAL( contextMenu( K3ListView *, Q3ListViewItem *, const QPoint & ) ), SLOT( showPopup( K3ListView *, Q3ListViewItem *, const QPoint & ) ) );
		connect( this, SIGNAL( doubleClicked( Q3ListViewItem* ) ), this, SLOT( modAuthor( Q3ListViewItem* ) ) );
		connect( this, SIGNAL( itemRenamed( Q3ListViewItem* ) ), this, SLOT( saveAuthor( Q3ListViewItem* ) ) );
	}
}

void StdAuthorListView::showPopup( K3ListView * /*l*/, Q3ListViewItem *i, const QPoint &p )
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
        delete elementDialog;
}

void StdAuthorListView::remove
	()
{
	Q3ListViewItem * item = currentItem();

	if ( item ) {
		int id = item->text( 1 ).toInt();

		ElementList recipeDependancies;
		database->findUseOfAuthorInRecipes( &recipeDependancies, id );

		if ( recipeDependancies.isEmpty() ) {
			switch ( KMessageBox::warningContinueCancel( this, i18n( "Are you sure you want to delete this author?" ) ) ) {
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

			DependanciesDialog warnDialog( this, info, false );
			if ( warnDialog.exec() == QDialog::Accepted )
				database->removeAuthor( id );
		}
	}
}

void StdAuthorListView::slotRename()
{
    rename( 0, 0 );
}

void StdAuthorListView::rename( Q3ListViewItem* /*item*/,int /*c*/ )
{
	Q3ListViewItem * item = currentItem();

	if ( item )
		AuthorListView::rename( item, 0 );
}

void StdAuthorListView::createAuthor( const Element &author )
{
	createElement(new Q3ListViewItem( this, author.name, QString::number( author.id ) ));
}

void StdAuthorListView::removeAuthor( int id )
{
	Q3ListViewItem * item = findItem( QString::number( id ), 1 );
	removeElement(item);
}

void StdAuthorListView::modAuthor( Q3ListViewItem* i )
{
	if ( i )
		AuthorListView::rename( i, 0 );
}

void StdAuthorListView::saveAuthor( Q3ListViewItem* i )
{
	if ( !checkBounds( i->text( 0 ) ) ) {
		reload(ForceReload); //reset the changed text
		return ;
	}

	int existing_id = database->findExistingAuthorByName( i->text( 0 ) );
	int author_id = i->text( 1 ).toInt();
	if ( existing_id != -1 && existing_id != author_id )  //category already exists with this label... merge the two
	{
		switch ( KMessageBox::warningContinueCancel( this, i18n( "This author already exists.  Continuing will merge these two authors into one.  Are you sure?" ) ) )
		{
		case KMessageBox::Continue: {
				database->mergeAuthors( existing_id, author_id );
				break;
			}
		default:
			reload(ForceReload);
			break;
		}
	}
	else {
		database->modAuthor( ( i->text( 1 ) ).toInt(), i->text( 0 ) );
	}
}

bool StdAuthorListView::checkBounds( const QString &name )
{
	if ( name.length() > int(database->maxAuthorNameLength()) ) {
		KMessageBox::error( this, i18np( "Author name cannot be longer than 1 character.", "Author name cannot be longer than %1 characters." , database->maxAuthorNameLength() ));
		return false;
	}

	return true;
}


AuthorCheckListItem::AuthorCheckListItem( AuthorCheckListView* qlv, const Element &author ) : Q3CheckListItem( qlv, QString::null, Q3CheckListItem::CheckBox ),
	authorStored(author),
	m_listview(qlv)
{
}

AuthorCheckListItem::AuthorCheckListItem( AuthorCheckListView* qlv, Q3ListViewItem *after, const Element &author ) : Q3CheckListItem( qlv, after, QString::null, Q3CheckListItem::CheckBox ),
	authorStored(author),
	m_listview(qlv)
{
}

Element AuthorCheckListItem::author() const
{
	return authorStored;
}

QString AuthorCheckListItem::text( int column ) const
{
	switch ( column ) {
	case 0:
		return ( authorStored.name );
	case 1:
		return ( QString::number( authorStored.id ) );
	default:
		return QString::null;
	}
}

void AuthorCheckListItem::stateChange( bool on )
{
	m_listview->stateChange(this,on);
}


AuthorCheckListView::AuthorCheckListView( QWidget *parent, RecipeDB *db ) : AuthorListView( parent, db )
{
	addColumn( i18n( "Author" ) );

	KConfigGroup config = KGlobal::config()->group( "Advanced" );
	bool show_id = config.readEntry( "ShowID", false );
	addColumn( i18n( "Id" ), show_id ? -1 : 0 );
}

void AuthorCheckListView::createAuthor( const Element &author )
{
	createElement(new AuthorCheckListItem( this, author ));
}

void AuthorCheckListView::removeAuthor( int id )
{
	Q3ListViewItem * item = findItem( QString::number( id ), 1 );
	removeElement(item);
}

void AuthorCheckListView::load( int limit, int offset )
{
	AuthorListView::load(limit,offset);

	for ( QList<Element>::const_iterator author_it = m_selections.constBegin(); author_it != m_selections.constEnd(); ++author_it ) {
		Q3CheckListItem * item = ( Q3CheckListItem* ) findItem( QString::number( (*author_it).id ), 1 );
		if ( item ) {
			item->setOn(true);
		}
	}
}

void AuthorCheckListView::stateChange(AuthorCheckListItem *it,bool on)
{
	if ( !reloading() ) {
		if ( on )
			m_selections.append(it->author());
		else
			m_selections.removeAll(it->author());
	}
}

#include "authorlistview.moc"
