/***************************************************************************
*   Copyright (C) 2003 by Unai Garro (ugarro@users.sourceforge.net)       *
*                                                                         *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#include "paneldeco.h"

#include <qpainter.h>
#include <qpoint.h>
#include <qrect.h>
//Added by qt3to4:
#include <QEvent>
#include <QChildEvent>
#include <QPaintEvent>

#include <kiconloader.h>
#include <qpixmap.h>
#include <kvbox.h>



// Panel decoration

PanelDeco::PanelDeco( QWidget *parent, const char *name, const QString &title, const QString &iconName ) : KVBox( parent )
{

	// Top decoration
	tDeco = new TopDeco( this, "TopDecoration", title, iconName );

	hbox = new KHBox( this );

	//Left decoration
	lDeco = new LeftDeco( hbox, "LeftDecoration" );

	//The widget stack (panels)
	stack = new Q3WidgetStack( hbox );
	stack->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

}


PanelDeco::~PanelDeco()
{}

void PanelDeco::childEvent( QChildEvent *e )
{
	if ( e->type() == QEvent::ChildInserted ) {
		QObject * obj = e->child();
		if ( obj->inherits( "QWidget" ) ) {
			QWidget * w = ( QWidget* ) obj;
			if ( w != hbox && w != tDeco )
            w->setParent( stack, windowFlags() & ~Qt::WindowType_Mask);
            w->setGeometry( 0, 0 ,w->width(),w->height());
		}
	}
}


int PanelDeco::id( QWidget* w )
{
	return ( stack->id( w ) );
}

void PanelDeco::raise( QWidget *w )
{
	QWidget * old_w = visiblePanel();

	stack->raiseWidget( w );

	if ( old_w != w )
		emit panelRaised( w, old_w );
}

QWidget* PanelDeco::visiblePanel( void )
{
	return ( stack->visibleWidget() );
}

void PanelDeco::setHeader( const QString &title, const QString &icon )
{
	tDeco->setHeader( title, icon );
}

// Left part of the decoration

LeftDeco::LeftDeco( QWidget *parent, const char *name ) :
#if QT_VERSION >= 0x030200
		QWidget( parent, name, Qt::WNoAutoErase )
#else
		QWidget( parent, name )
#endif
{}

LeftDeco::~LeftDeco()
{}

// Top part of the decoration

TopDeco::TopDeco( QWidget *parent, const char *name, const QString &title, const QString &iconName ) :
#if QT_VERSION >= 0x030200
		QWidget( parent, name, Qt::WNoAutoErase )
#else
		QWidget( parent, name )
#endif
{
	setMinimumHeight( 30 );
	panelTitle = QString::null;
	if ( !iconName.isNull() ) {
		icon = KIconLoader::global()->loadIcon( iconName, KIconLoader::NoGroup, 22 );
	}

	if ( !title.isNull() ) {
		panelTitle = title;
	}
}

TopDeco::~TopDeco()
{
}


void TopDeco::paintEvent( QPaintEvent * )
{
	// Get gradient colors
	QColor c1 = colorGroup().button().light( 120 );
	QColor c2 = paletteBackgroundColor();

	// Draw the gradient
	QPixmap kpm;
	kpm.resize( size() );
	
	// to be done KDE4 port
	//KPixmapEffect::unbalancedGradient ( kpm, c1, c2, KPixmapEffect::VerticalGradient, 150, 150 );

	// Add a line on top
	QPainter painter( &kpm );
	painter.setPen( colorGroup().button().dark( 130 ) );
	painter.drawLine( 0, 0, width(), 0 );

	// Now Add the icon
	int xpos = 0, ypos = 0;
	if ( !icon.isNull() ) {
		xpos = 20;
		ypos = ( height() - icon.height() ) / 2 - 1;
		painter.drawPixmap( xpos, ypos, icon );
		xpos += icon.width(); // Move it so that later we can easily place the text
	}

	// Finally, draw the text besides the icon
	if ( !panelTitle.isNull() ) {
		xpos += 15;
		QRect r = rect();
		r.setLeft( xpos );
		painter.setPen( QColor( 0x00, 0x00, 0x00 ) );
		QFont ft = font();
		ft.setBold( true );
		painter.setFont( ft );
		painter.drawText( r, Qt::AlignVCenter, panelTitle );
	}
	painter.end();
	// Copy the pixmap to the widget
	bitBlt( this, 0, 0, &kpm );
}

void TopDeco::setHeader( const QString &title, const QString &iconName )
{
	if ( !title.isNull() )
		panelTitle = title;
	if ( !iconName.isNull() ) {
		icon = KIconLoader::global()->loadIcon( iconName, KIconLoader::NoGroup, 22 );
	}
	if ( !title.isNull() || !iconName.isNull() )
		update();
}

QSize TopDeco::sizeHint( void )
{
	return ( QSize( parentWidget() ->width(), 30 ) );
}

#include "paneldeco.moc"
