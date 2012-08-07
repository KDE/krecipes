/*****************************************************************************
*   Copyright © 2009-2012 José Manuel Santamaría Lema <panfaust@gmail.com>   *
*                                                                            *
*   This program is free software; you can redistribute it and/or modify     *
*   it under the terms of the GNU General Public License as published by     *
*   the Free Software Foundation; either version 2 of the License, or        *
*   (at your option) any later version.                                      *
******************************************************************************/

#include "kregenericactionshandler.h"

#include <KMenu>
#include <QModelIndex>
#include <KAction>
/*#include <KLocale>
#include <KIcon>
#include <QPointer>
#include <KMessageBox>

#include "datablocks/elementlist.h"

#include "backends/recipedb.h"

#include "widgets/authorlistview.h"

#include "dialogs/createelementdialog.h"
#include "dialogs/dependanciesdialog.h"*/
#include "widgets/kregenericlistwidget.h"


KreGenericActionsHandler::KreGenericActionsHandler( KreGenericListWidget * listWidget, RecipeDB * db ):
	QObject( listWidget ), m_listWidget( listWidget ), m_database( db )
{

	m_contextMenu = new KMenu( listWidget );
	connect( listWidget,
		SIGNAL( contextMenuRequested( const QModelIndex &, const QPoint & ) ),
		SLOT( showPopup( const QModelIndex &, const QPoint & ) )
	);
	connect( listWidget,
		SIGNAL( itemsChanged( const QModelIndex &, const QModelIndex & ) ),
		SLOT( saveElement( const QModelIndex &, const QModelIndex &) )
	);
}

void KreGenericActionsHandler::rename()
{
	int row = m_listWidget->currentRow();
	if ( row != -1 )
		m_listWidget->edit( row );
}

void KreGenericActionsHandler::addAction( KAction * action )
{
	m_contextMenu->addAction( action );
}

void KreGenericActionsHandler::showPopup( const QModelIndex & index, const QPoint & point )
{
	if (index.isValid() )
		m_contextMenu->exec( point );
}


