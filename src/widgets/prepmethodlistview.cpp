/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
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
#include <kmenu.h>
#include <QPointer>

#include "backends/recipedb.h"
#include "dialogs/createelementdialog.h"
#include "dialogs/dependanciesdialog.h"

PrepMethodListView::PrepMethodListView( QWidget *parent, RecipeDB *db ) : DBListViewBase( parent,db,db->prepMethodCount())
{
	setAllColumnsShowFocus( true );
	setDefaultRenameAction( Q3ListView::Reject );
	connect( database, SIGNAL( prepMethodCreated( const Element & ) ), SLOT( checkCreatePrepMethod( const Element & ) ) );
	connect( database, SIGNAL( prepMethodRemoved( int ) ), SLOT( removePrepMethod( int ) ) );
}

void PrepMethodListView::load( int limit, int offset )
{
	ElementList prepMethodList;
	database->loadPrepMethods( &prepMethodList, limit, offset );

	setTotalItems(prepMethodList.count());

	for ( ElementList::const_iterator ing_it = prepMethodList.constBegin(); ing_it != prepMethodList.constEnd(); ++ing_it )
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
	addColumn( i18nc( "@title:column", "Preparation Method" ) );

	KConfigGroup config = KGlobal::config()->group( "Advanced" );
	bool show_id = config.readEntry( "ShowID", false );
	addColumn( i18nc( "@title:column", "Id" ) , show_id ? -1 : 0 );

	if ( editable ) {
		setRenameable( 0, true );
	}
}

void StdPrepMethodListView::createPrepMethod( const Element &ing )
{
	createElement(new Q3ListViewItem( this, ing.name, QString::number( ing.id ) ));
}

void StdPrepMethodListView::removePrepMethod( int id )
{
	Q3ListViewItem * item = findItem( QString::number( id ), 1 );
	removeElement(item);
}

#include "prepmethodlistview.moc"
