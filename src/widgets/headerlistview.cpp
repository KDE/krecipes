/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2004 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2004 Cyril Bosselut <bosselut@b1project.com>              *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "headerlistview.h"

#include <kmessagebox.h>
#include <kconfig.h>
#include <klocale.h>
#include <kglobal.h>
#include <kmenu.h>
#include <QPointer>

#include "backends/recipedb.h"
#include "dialogs/createelementdialog.h"
#include "dialogs/dependanciesdialog.h"

HeaderListView::HeaderListView( QWidget *parent, RecipeDB *db ) : DBListViewBase( parent,db,db->unitCount() )
{
	setAllColumnsShowFocus( true );
	setDefaultRenameAction( Q3ListView::Reject );
	connect( database, SIGNAL( ingGroupCreated( const Element & ) ), SLOT( checkCreateHeader( const Element & ) ) );
	connect( database, SIGNAL( ingGroupRemoved( int ) ), SLOT( removeHeader( int ) ) );
}

void HeaderListView::load( int /*limit*/, int /*offset*/ )
{
	ElementList headerList;
	database->loadIngredientGroups( &headerList );

	setTotalItems(headerList.count());

	for ( ElementList::const_iterator it = headerList.constBegin(); it != headerList.constEnd(); ++it ) {
		createHeader( *it );
	}
}

void HeaderListView::checkCreateHeader( const Element &el )
{
	if ( handleElement(el.name) ) { //only create this header if the base class okays it
		createHeader(el);
	}
}


StdHeaderListView::StdHeaderListView( QWidget *parent, RecipeDB *db, bool editable ) : HeaderListView( parent, db )
{
	addColumn( i18nc( "@title:column", "Header" ) );

	KConfigGroup config = KGlobal::config()->group( "Advanced" );
	bool show_id = config.readEntry( "ShowID", false );

	addColumn( i18nc( "@title:column", "Id" ) , show_id ? -1 : 0 );

	if ( editable ) {
		setRenameable( 0, true );
	}
}

void StdHeaderListView::createHeader( const Element &header )
{
	createElement(new Q3ListViewItem( this, header.name, QString::number( header.id ) ));
}

void StdHeaderListView::removeHeader( int id )
{
	Q3ListViewItem * item = findItem( QString::number( id ), 1 );
	removeElement(item);
}

#include "headerlistview.moc"
