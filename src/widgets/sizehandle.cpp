/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**               2003 Jason Kivlighn <mizunoami44@users.sourceforge.net>
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "sizehandle.h"
#include "dragarea.h"

#include <qwidget.h>
#include <qapplication.h>
#include <qlabel.h>
#include <qpainter.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QPaintEvent>

SizeHandle::SizeHandle( DragArea *parent, Direction d, WidgetSelection *s )
		: QWidget( parent )
{
	active = TRUE;
	setBackgroundMode( active ? Qt::PaletteText : Qt::PaletteDark );
	setFixedSize( 6, 6 );
	widget = 0;
	dir = d ;
	setMouseTracking( FALSE );
	dragArea = parent;
	sel = s;
	updateCursor();
}

void SizeHandle::updateCursor()
{
	if ( !active ) {
		setCursor( Qt::arrowCursor );
		return ;
	}

	switch ( dir ) {
	case LeftTop:
		setCursor( Qt::sizeFDiagCursor );
		break;
	case Top:
		setCursor( Qt::sizeVerCursor );
		break;
	case RightTop:
		setCursor( Qt::sizeBDiagCursor );
		break;
	case Right:
		setCursor( Qt::sizeHorCursor );
		break;
	case RightBottom:
		setCursor( Qt::sizeFDiagCursor );
		break;
	case Bottom:
		setCursor( Qt::sizeVerCursor );
		break;
	case LeftBottom:
		setCursor( Qt::sizeBDiagCursor );
		break;
	case Left:
		setCursor( Qt::sizeHorCursor );
		break;
	}
}

void SizeHandle::setActive( bool a )
{
	active = a;
	setBackgroundMode( active ? Qt::PaletteText : Qt::PaletteDark );
	updateCursor();
}

void SizeHandle::setWidget( QWidget *w )
{
	widget = w;
}

void SizeHandle::paintEvent( QPaintEvent * )
{
	//   if ( ( (DragArea*)parentWidget() )->currentWidget() != widget )
	//	return;
	QPainter p( this );
	p.setPen( Qt::blue );
	p.drawRect( 0, 0, width(), height() );
}

void SizeHandle::mousePressEvent( QMouseEvent *e )
{
	if ( !widget || e->button() != Qt::LeftButton || !active )
		return ;
	oldPressPos = e->pos();
	geom = origGeom = QRect( widget->pos(), widget->size() );
}

void SizeHandle::mouseMoveEvent( QMouseEvent *e )
{
	if ( !widget || ( e->state() & Qt::LeftButton ) != Qt::LeftButton || !active )
		return ;
	QPoint rp = mapFromGlobal( e->globalPos() );
	QPoint d = oldPressPos - rp;
	oldPressPos = rp;
	QPoint checkPos = widget->parentWidget() ->mapFromGlobal( e->globalPos() );
	QRect pr = widget->parentWidget() ->rect();

	// ##### move code around a bit to reduce duplicated code here
	switch ( dir ) {
	case LeftTop: {
			if ( checkPos.x() > pr.width() - 2 * width() || checkPos.y() > pr.height() - 2 * height() )
				return ;
			int w = geom.width() + d.x();
			geom.setWidth( w );
			w = ( w / dragArea->gridSpacing().x() ) * dragArea->gridSpacing().x();
			int h = geom.height() + d.y();
			geom.setHeight( h );
			h = ( h / dragArea->gridSpacing().y() ) * dragArea->gridSpacing().y();
			int dx = widget->width() - w;
			int dy = widget->height() - h;
			trySetGeometry( widget, widget->x() + dx, widget->y() + dy, w, h );
		}
		break;
	case Top: {
			if ( checkPos.y() > pr.height() - 2 * height() )
				return ;
			int h = geom.height() + d.y();
			geom.setHeight( h );
			h = ( h / dragArea->gridSpacing().y() ) * dragArea->gridSpacing().y();
			int dy = widget->height() - h;
			trySetGeometry( widget, widget->x(), widget->y() + dy, widget->width(), h );
		}
		break;
	case RightTop: {
			if ( checkPos.x() < 2 * width() || checkPos.y() > pr.height() - 2 * height() )
				return ;
			int h = geom.height() + d.y();
			geom.setHeight( h );
			h = ( h / dragArea->gridSpacing().y() ) * dragArea->gridSpacing().y();
			int dy = widget->height() - h;
			int w = geom.width() - d.x();
			geom.setWidth( w );
			w = ( w / dragArea->gridSpacing().x() ) * dragArea->gridSpacing().x();
			trySetGeometry( widget, widget->x(), widget->y() + dy, w, h );
		}
		break;
	case Right: {
			if ( checkPos.x() < 2 * width() )
				return ;
			int w = geom.width() - d.x();
			geom.setWidth( w );
			w = ( w / dragArea->gridSpacing().x() ) * dragArea->gridSpacing().x();
			tryResize( widget, w, widget->height() );
		}
		break;
	case RightBottom: {
			if ( checkPos.x() < 2 * width() || checkPos.y() < 2 * height() )
				return ;
			int w = geom.width() - d.x();
			geom.setWidth( w );
			w = ( w / dragArea->gridSpacing().x() ) * dragArea->gridSpacing().x();
			int h = geom.height() - d.y();
			geom.setHeight( h );
			h = ( h / dragArea->gridSpacing().y() ) * dragArea->gridSpacing().y();
			tryResize( widget, w, h );
		}
		break;
	case Bottom: {
			if ( checkPos.y() < 2 * height() )
				return ;
			int h = geom.height() - d.y();
			geom.setHeight( h );
			h = ( h / dragArea->gridSpacing().y() ) * dragArea->gridSpacing().y();
			tryResize( widget, widget->width(), h );
		}
		break;
	case LeftBottom: {
			if ( checkPos.x() > pr.width() - 2 * width() || checkPos.y() < 2 * height() )
				return ;
			int w = geom.width() + d.x();
			geom.setWidth( w );
			w = ( w / dragArea->gridSpacing().x() ) * dragArea->gridSpacing().x();
			int dx = widget->width() - w;
			int h = geom.height() - d.y();
			geom.setHeight( h );
			h = ( h / dragArea->gridSpacing().y() ) * dragArea->gridSpacing().y();
			trySetGeometry( widget, widget->x() + dx, widget->y(), w, h );
		}
		break;
	case Left: {
			if ( checkPos.x() > pr.width() - 2 * width() )
				return ;
			int w = geom.width() + d.x();
			geom.setWidth( w );
			w = ( w / dragArea->gridSpacing().x() ) * dragArea->gridSpacing().x();
			int dx = widget->width() - w;
			trySetGeometry( widget, widget->x() + dx, widget->y(), w, widget->height() );
		}
		break;
	}

	QPoint p = pos();
	sel->updateGeometry();
	oldPressPos += ( p - pos() );
}

void SizeHandle::mouseReleaseEvent( QMouseEvent * /*e*/ )
{ /*
	    if ( e->button() != LeftButton || !active )
		return;
	 
	    dragArea->sizePreview()->hide();
	    if ( geom != widget->geometry() )
		dragArea->commandHistory()->addCommand( new ResizeCommand( tr( "Resize" ),
									     dragArea,
									     widget, origGeom,
									     widget->geometry() ) );
	    dragArea->emitUpdateProperties( widget );*/
}

void SizeHandle::trySetGeometry( QWidget *w, int x, int y, int width, int height )
{
	int minw =  /*QMAX( w->minimumSizeHint().width(),*/ w->minimumSize().width()  /*)*/;
	minw = QMAX( minw, 2 * dragArea->gridSpacing().x() );
	int minh =  /*QMAX( w->minimumSizeHint().height(),*/ w->minimumSize().height()  /*)*/;
	minh = QMAX( minh, 2 * dragArea->gridSpacing().y() );
	if ( QMAX( minw, width ) > w->maximumWidth() ||
	        QMAX( minh, height ) > w->maximumHeight() )
		return ;
	if ( width < minw && x != w->x() )
		x -= minw - width;
	if ( height < minh && y != w->y() )
		y -= minh - height;

	emit widgetGeometryChanged();
	w->setGeometry( x, y, QMAX( minw, width ), QMAX( minh, height ) );
}

void SizeHandle::tryResize( QWidget *w, int width, int height )
{
	int minw =  /*QMAX( w->minimumSizeHint().width(),*/ w->minimumSize().width()  /*)*/;
	minw = QMAX( minw, 16 );
	int minh =  /*QMAX( w->minimumSizeHint().height(),*/ w->minimumSize().height()  /*)*/;
	minh = QMAX( minh, 16 );

	emit widgetGeometryChanged();
	w->resize( QMAX( minw, width ), QMAX( minh, height ) );
}

// ------------------------------------------------------------------------

WidgetSelection::WidgetSelection( DragArea *parent )
{
	dragArea = parent;
	for ( int i = SizeHandle::LeftTop; i <= SizeHandle::Left; ++i ) {
		SizeHandle *size_handle = new SizeHandle( dragArea, ( SizeHandle::Direction ) i, this );
		QObject::connect( size_handle, SIGNAL( widgetGeometryChanged() ), dragArea, SLOT( emitWidgetGeometryChanged() ) );
		handles.insert( i, size_handle );
	}
	hide();
}

void WidgetSelection::setWidget( QWidget *w )
{
	if ( !w ) {
		hide();
		wid = 0;
		return ;
	}

	wid = w;
	//bool active = !wid->parentWidget() || WidgetFactory::layoutType( wid->parentWidget() ) == WidgetFactory::NoLayout;
	for ( int i = SizeHandle::LeftTop; i <= SizeHandle::Left; ++i ) {
		SizeHandle *h = handles[ i ];
		if ( h ) {
			h->setWidget( wid );
			h->setActive( true );
		}
	}
	updateGeometry();
	show();
}

bool WidgetSelection::isUsed() const
{
	return wid != 0;
}

void WidgetSelection::updateGeometry()
{
	if ( !wid )
		return ;

	QPoint p = wid->parentWidget() ->mapToGlobal( wid->pos() );
	p = dragArea->mapFromGlobal( p );
	QRect r( p, wid->size() );

	int w = 6;
	int h = 6;

	for ( int i = SizeHandle::LeftTop; i <= SizeHandle::Left; ++i ) {
		SizeHandle *hndl = handles[ i ];
		if ( !hndl )
			continue;
		switch ( i ) {
		case SizeHandle::LeftTop:
			hndl->move( r.x() - w / 2, r.y() - h / 2 );
			break;
		case SizeHandle::Top:
			hndl->move( r.x() + r.width() / 2 - w / 2, r.y() - h / 2 );
			break;
		case SizeHandle::RightTop:
			hndl->move( r.x() + r.width() - w / 2, r.y() - h / 2 );
			break;
		case SizeHandle::Right:
			hndl->move( r.x() + r.width() - w / 2, r.y() + r.height() / 2 - h / 2 );
			break;
		case SizeHandle::RightBottom:
			hndl->move( r.x() + r.width() - w / 2, r.y() + r.height() - h / 2 );
			break;
		case SizeHandle::Bottom:
			hndl->move( r.x() + r.width() / 2 - w / 2, r.y() + r.height() - h / 2 );
			break;
		case SizeHandle::LeftBottom:
			hndl->move( r.x() - w / 2, r.y() + r.height() - h / 2 );
			break;
		case SizeHandle::Left:
			hndl->move( r.x() - w / 2, r.y() + r.height() / 2 - h / 2 );
			break;
		default:
			break;
		}
	}
}

void WidgetSelection::hide()
{
	for ( int i = SizeHandle::LeftTop; i <= SizeHandle::Left; ++i ) {
		SizeHandle *h = handles[ i ];
		if ( h )
			h->hide();
	}
}

void WidgetSelection::show()
{
	for ( int i = SizeHandle::LeftTop; i <= SizeHandle::Left; ++i ) {
		SizeHandle *h = handles[ i ];
		if ( h ) {
			h->show();
			h->raise();
		}
	}
}

void WidgetSelection::update()
{
	for ( int i = SizeHandle::LeftTop; i <= SizeHandle::Left; ++i ) {
		SizeHandle *h = handles[ i ];
		if ( h )
			h->update();
	}
}

QWidget *WidgetSelection::widget() const
{
	return wid;
}

#include "sizehandle.moc"
