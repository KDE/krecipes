/**************************************************************************
*   Copyright © 2004-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "krelistview.h"

#include <KApplication>
#include <KConfigGroup>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kdebug.h>

//Added by qt3to4:
#include <QLabel>
#include <QFrame>
#include <kvbox.h>

#include "widgets/dblistviewbase.h"

//FIXME: this constructor should have an "searchAsYouType" parameter instead of reading the config
KreListView::KreListView( QWidget *parent, const QString &title, bool filter, int filterCol, QWidget *embeddedWidget ):
	KVBox( parent )
{

	filteredColumn = filterCol;
	QWidget *header = this;
	if ( filter || embeddedWidget ) {
		header = new KHBox( this );
		( ( KHBox* ) header ) ->setSpacing( 15 );
	}

	if ( !title.isEmpty() ) {
		listLabel = new QLabel( header );
		listLabel->setFrameShape( QFrame::NoFrame  );
		listLabel->setFrameShadow( QFrame::Plain );
		listLabel->setForegroundRole( QPalette::WindowText );
		QFont font;
		font.setWeight(QFont::Bold);
		listLabel->setFont( font );
		listLabel->setText( title );
	}

	if ( filter ) {
		filterBox = new KHBox( header );
		//filterBox->setFrameShape( QFrame::Box );
		//filterBox->setMargin( 2 );

		filterLabel = new QLabel( filterBox );
		filterLabel->setText( ' ' + i18nc( "@label:textbox Search recipes", "Search:" ) );
		filterEdit = new KLineEdit( filterBox );
		filterEdit->setClearButtonShown( true );
		connect( filterEdit, SIGNAL( clearButtonClicked() ), this, SLOT( clearSearch() ) );
	}

	list = new K3ListView( this );
	list->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::MinimumExpanding );
	setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Ignored );
	//setSpacing( 10 );

	// If the user provides a widget, embed it into the header
	if ( embeddedWidget ){
		embeddedWidget->setParent( header, windowFlags() & ~Qt::WindowType_Mask);
		embeddedWidget->setGeometry( 0, 0 ,embeddedWidget->width(),embeddedWidget->height());
	}
	//Connect Signals & Slots
	if ( filter ) {
		connect( filterEdit, SIGNAL( textChanged( const QString& ) ), SIGNAL( textChanged(const QString&) ) );
		connect( filterEdit, SIGNAL( returnPressed( const QString& ) ), SIGNAL( returnPressed(const QString&) ) );
		KConfigGroup config (KGlobal::config(), "Performance" );
		if ( config.readEntry("SearchAsYouType",true) )
			connect( this, SIGNAL( textChanged( const QString& ) ), SLOT( filter( const QString& ) ) );
		else
			connect( this, SIGNAL( returnPressed( const QString& ) ), SLOT( filter( const QString& ) ) );
	}
}

KreListView::~KreListView()
{}

void KreListView::filter( const QString& s )
{
	for ( Q3ListViewItem * it = list->firstChild();it;it = it->nextSibling() ) {
		if ( it->rtti() == NEXTLISTITEM_RTTI || it->rtti() == PREVLISTITEM_RTTI )
			continue;

		if ( s.isEmpty() )  // Don't filter if the filter text is empty
		{
			it->setVisible( true );
		}
		else
		{

			if ( it->text( filteredColumn ).contains( s, Qt::CaseInsensitive ) )
				it->setVisible( true );
			else
				it->setVisible( false );

		}


	}
}

void KreListView::refilter()
{
	if ( !filterEdit->text().isEmpty() ) {
		emit textChanged( filterEdit->text() );
	}
}

void KreListView::setCustomFilter( QObject *receiver, const char *slot )
{
	KConfigGroup config (KGlobal::config(), "Performance" );
	if ( config.readEntry("SearchAsYouType",true) ) {
		disconnect( this, SIGNAL( textChanged( const QString& ) ), this, SLOT( filter( const QString& ) ) );
		connect( this, SIGNAL( textChanged( const QString& ) ), receiver, slot );
	} else {
		disconnect( this, SIGNAL( returnPressed( const QString& ) ), this, SLOT( filter( const QString& ) ) );
		connect( this, SIGNAL( returnPressed( const QString& ) ), receiver, slot );
	}
}

void KreListView::setListView( DBListViewBase *list_view )
{
	delete list;

	connect( list_view, SIGNAL( nextGroupLoaded() ), SLOT( refilter() ) );
	connect( list_view, SIGNAL( prevGroupLoaded() ), SLOT( refilter() ) );
	list = list_view;
	list->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::MinimumExpanding );
}

void KreListView::clearSearch()
{
	filterEdit->clear();
	filter( QString() );
}

#include "krelistview.moc"
