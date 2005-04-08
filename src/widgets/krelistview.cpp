/***************************************************************************
*   Copyright (C) 2004-2005 by                                            *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "krelistview.h"

#include <kglobalsettings.h>
#include <klocale.h>

#include "widgets/dblistviewbase.h"

KreListView::KreListView( QWidget *parent, const QString &title, bool filter, int filterCol, QWidget *embeddedWidget ) : QVBox( parent )
{

	filteredColumn = filterCol;
	QWidget *header = this;
	if ( filter || embeddedWidget ) {
		header = new QHBox( this );
		( ( QHBox* ) header ) ->setSpacing( 15 );
	}

	if ( !title.isNull() ) {
		listLabel = new QLabel( header );
		listLabel->setFrameShape( QFrame::GroupBoxPanel );
		listLabel->setFrameShadow( QFrame::Sunken );
		listLabel->setPaletteForegroundColor( KGlobalSettings::highlightedTextColor() );
		listLabel->setPaletteBackgroundColor( KGlobalSettings::highlightColor().light( 120 ) ); // 120, to match the kremenu settings
		listLabel->setText( title );

	}

	if ( filter ) {
		filterBox = new QHBox( header );
		filterBox->setFrameShape( QFrame::Box );
		filterBox->setMargin( 2 );
		filterLabel = new QLabel( filterBox );
		filterLabel->setText( i18n( "Search:" ) );
		filterEdit = new KLineEdit( filterBox );
	}


	list = new KListView( this );
	list->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
	setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
	setSpacing( 10 );


	// If the user provides a widget, embed it into the header
	if ( embeddedWidget )
		embeddedWidget->reparent( header, QPoint( 0, 0 ) );
	//Connect Signals & Slots
	if ( filter )
		connect( filterEdit, SIGNAL( textChanged( const QString& ) ), this, SLOT( filter( const QString& ) ) );
}

KreListView::~KreListView()
{}

void KreListView::filter( const QString& s )
{
	for ( QListViewItem * it = list->firstChild();it;it = it->nextSibling() ) {
		if ( it->rtti() == NEXTLISTITEM_RTTI || it->rtti() == PREVLISTITEM_RTTI )
			continue;

		if ( s.isEmpty() )  // Don't filter if the filter text is empty
		{
			it->setVisible( true );
		}
		else
		{

			if ( it->text( filteredColumn ).contains( s, false ) )
				it->setVisible( true );
			else
				it->setVisible( false );

		}


	}
}

void KreListView::setCustomFilter( QObject *receiver, const char *slot )
{
	disconnect( SIGNAL( textChanged( const QString& ) ), filterEdit, SLOT( filter( const QString& ) ) );
	connect( filterEdit, SIGNAL( textChanged( const QString& ) ), receiver, slot );
}

#include "krelistview.moc"
