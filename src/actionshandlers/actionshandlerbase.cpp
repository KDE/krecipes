/***************************************************************************
*   Copyright © 2009 José Manuel Santamaría Lema (panfaust@gmail.com)     *
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

	//FIXME: This code is so redundant, we already added the actions in Krecipes class,
	// please code something like RecipeActionsHandler::addAction(...). Also, currently
	// if you configure other shortcuts, you can't see the correct shortcuts in contextual
	// menus.
	kpop = new KMenu( parentListView );
	kpop->addAction( KIcon( "document-new" ), i18n( "&Create" ), this, SLOT( createNew() ), Qt::CTRL + Qt::ALT + Qt::Key_N );
	kpop->addAction( KIcon( "edit-rename" ), i18n( "&Rename" ), this, SLOT( rename() ), Qt::CTRL + Qt::Key_R );
	kpop->addAction( KIcon( "edit-delete" ), i18n( "&Delete" ), this, SLOT( remove() ), Qt::Key_Delete );
	kpop->ensurePolished();

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
