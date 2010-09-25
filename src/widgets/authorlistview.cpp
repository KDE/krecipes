/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
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
#include <kmenu.h>
#include <QList>
#include <QPointer>

#include "backends/recipedb.h"
#include "dialogs/createelementdialog.h"
#include "dialogs/dependanciesdialog.h"

AuthorListView::AuthorListView( QWidget *parent, RecipeDB *db ) : DBListViewBase( parent, db, db->authorCount() )
{
	setAllColumnsShowFocus( true );
	setDefaultRenameAction( Q3ListView::Reject );
	connect( database, SIGNAL( authorCreated( const Element & ) ), SLOT( checkCreateAuthor( const Element & ) ) );
	connect( database, SIGNAL( authorRemoved( int ) ), SLOT( removeAuthor( int ) ) );
}

void AuthorListView::load( int limit, int offset )
{
	ElementList authorList;
	database->loadAuthors( &authorList, limit, offset );

	setTotalItems(authorList.count());

	for ( ElementList::const_iterator ing_it = authorList.constBegin(); ing_it != authorList.constEnd(); ++ing_it )
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
	addColumn( "Id" , show_id ? -1 : 0 );

	if ( editable ) {
		setRenameable( 0, true );
	}
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


AuthorCheckListItem::AuthorCheckListItem( AuthorCheckListView* qlv, const Element &author ) : Q3CheckListItem( qlv, QString(), Q3CheckListItem::CheckBox ),
	authorStored(author),
	m_listview(qlv)
{
}

AuthorCheckListItem::AuthorCheckListItem( AuthorCheckListView* qlv, Q3ListViewItem *after, const Element &author ) : Q3CheckListItem( qlv, after, QString(), Q3CheckListItem::CheckBox ),
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
		return QString();
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
	addColumn( "Id" , show_id ? -1 : 0 );
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
