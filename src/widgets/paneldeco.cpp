/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#include "paneldeco.h"

#include <QPainter>
#include <qpoint.h>
#include <QRect>
//Added by qt3to4:
#include <QChildEvent>
#include <QPaintEvent>
#include <QVBoxLayout>

#include <kiconloader.h>
#include <kvbox.h>

// Panel decoration

PanelDeco::PanelDeco( QWidget *parent, const char *name, const QString &title, const QString &iconName ):
	QWidget( parent )
{
	QVBoxLayout * lay = new QVBoxLayout;

	tDeco = new TopDeco( this, "TopDecoration", title, iconName );
	lay->addWidget( tDeco );

	stack = new QStackedWidget;
	lay->addWidget( stack );

	stack->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
	setLayout( lay );
}


PanelDeco::~PanelDeco()
{
}


void PanelDeco::addStackWidget( QWidget *w )
{
	stack->addWidget( w );
}

int PanelDeco::id( QWidget* w )
{
	return ( stack->indexOf( w ) );
}

void PanelDeco::raise( QWidget *w )
{
	QWidget * old_w = visiblePanel();

	stack->setCurrentWidget( w );

	if ( old_w != w )
		emit panelRaised( w, old_w );
}

QWidget* PanelDeco::visiblePanel( void )
{
	return ( stack->currentWidget() );
}

void PanelDeco::setHeader( const QString &title, const QString &icon )
{
	tDeco->setHeader( title, icon );
}

// Top part of the decoration

TopDeco::TopDeco( QWidget *parent, const char *name, const QString &title, const QString &iconName ) :
	QWidget( parent, Qt::WNoAutoErase )
{
	setObjectName( name );
	setMinimumHeight( 30 );
	panelTitle.clear();
	if ( !iconName.isEmpty() ) {
		icon = KIconLoader::global()->loadIcon( iconName, KIconLoader::NoGroup, 22 );
	}

	if ( !title.isEmpty() ) {
		panelTitle = title;
	}
}

TopDeco::~TopDeco()
{
}


void TopDeco::paintEvent( QPaintEvent * )
{
	// Get gradient colors
	QColor c1 = palette().color( QPalette::Light );
	QColor c2 = palette().color(backgroundRole());

	// Add a line on top
	QPainter painter(this );
	painter.setPen( palette().color( QPalette::Dark ));
	painter.drawLine( 0, 0, width(), 0 );

	QLinearGradient linearGrad(QPointF(0, 0), QPointF(0, height()) );
	linearGrad.setColorAt(0, c1);
	linearGrad.setColorAt(1, c2);
	QBrush brush( linearGrad );
	painter.fillRect( QRect( 0, 0, width(), height() ),brush );

	// Now Add the iconName
	int xpos = 0, ypos = 0;
	if ( !icon.isNull() ) {
		xpos = 20;
		ypos = ( height() - icon.height() ) / 2 - 1;
		painter.drawPixmap( xpos, ypos, icon );
		xpos += icon.width(); // Move it so that later we can easily place the text
	}

	// Finally, draw the text besides the icon
	if ( !panelTitle.isEmpty() ) {
		xpos += 15;
		QRect r = rect();
		r.setLeft( xpos );
		painter.setPen( palette().color( QPalette::WindowText  ));
		QFont ft = font();
		ft.setBold( true );
		painter.setFont( ft );
		painter.drawText( r, Qt::AlignVCenter, panelTitle );
	}
	painter.end();
}

void TopDeco::setHeader( const QString &title, const QString &iconName )
{
	if ( !title.isEmpty() )
		panelTitle = title;
	if ( !iconName.isEmpty() ) {
		icon = KIconLoader::global()->loadIcon( iconName, KIconLoader::NoGroup, 22 );
	}
	if ( !title.isEmpty() || !iconName.isEmpty() )
		update();
}

QSize TopDeco::sizeHint( void ) const
{
	return ( QSize( parentWidget() ->width(), 30 ) );
}

#include "paneldeco.moc"

