/***************************************************************************
*   Copyright © 2009 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "actionshandlerbase.h"

#include <KLocale>
#include <K3ListView>
#include <KMenu>
#include <KAction>
#include <KIcon>
#include <QPointer>
#include <KMessageBox>
class Q3ListViewItem;

#include "datablocks/elementlist.h"

#include "backends/recipedb.h"

#include "widgets/authorlistview.h"

#include "dialogs/createelementdialog.h"
#include "dialogs/dependanciesdialog.h"


ActionsHandlerBase::ActionsHandlerBase( DBListViewBase *_parentListView, RecipeDB *db ):
	QObject( _parentListView ), parentListView( _parentListView ), database( db )
{

	kpop = new KMenu( parentListView );
	connect( parentListView,
		SIGNAL( contextMenu( K3ListView *, Q3ListViewItem *, const QPoint & ) ),
		SLOT( showPopup( K3ListView *, Q3ListViewItem *, const QPoint & ) )
	);
	connect( parentListView,
		SIGNAL( doubleClicked( Q3ListViewItem*, const QPoint &, int ) ),
		SLOT( renameElement( Q3ListViewItem*, const QPoint &, int ) )
	);
	connect( parentListView,
		SIGNAL( itemRenamed( Q3ListViewItem* ) ),
		SLOT( saveElement( Q3ListViewItem* ) )
	);
}

void ActionsHandlerBase::rename()
{
	Q3ListViewItem * item = parentListView->currentItem();

	if ( item )
		parentListView->rename( item, 0 );
}

void ActionsHandlerBase::addAction( KAction * action )
{
	kpop->addAction( action );
}

void ActionsHandlerBase::showPopup( K3ListView * /*l*/, Q3ListViewItem *i, const QPoint &p )
{
	if ( i )
		kpop->exec( p );
}


void ActionsHandlerBase::renameElement( Q3ListViewItem* i, const QPoint &/*p*/, int c )
{
	if ( i )
		parentListView->rename( i, c );
}
