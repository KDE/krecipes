/***************************************************************************
 * Copyright (C) 2000 Trolltech AS.  All rights reserved.                  *
 *               2003 by Jason Kivlighn <mizunoami44@users.sourceforge.net>*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "dragarea.h"

#include "sizehandle.h"

DragArea::DragArea( QWidget *parent, const char *name ) : QWidget(parent,name),
 m_last_point(0,0),
 m_current_box(0),
 selection(new WidgetSelection(this))
{
	setMouseTracking(true);
}

DragArea::~DragArea()
{
	delete selection;
}

void DragArea::setWidget( QWidget *w )
{
	selection->setWidget(w);
}

void DragArea::mousePressEvent( QMouseEvent *e )
{
	mouse_down = true;

	m_current_box = childAt(e->pos());

	e->ignore();
	emit widgetClicked( e, (m_current_box) ? m_current_box : this ); //we'll say 'this' was clicked if no widget was clicked
	if ( e->isAccepted() )
		mouse_down = false;
	e->accept();

	if ( m_current_box && m_current_box->isEnabled() )
	{
		m_current_box->raise();

		widgetGeom = QRect( m_current_box->pos(), m_current_box->size() ); //widget may be on the move, so store this
		m_last_point = m_current_box->mapFromGlobal( e->globalPos() );
		m_begin_point = m_last_point;
	}
	else
	{
		m_current_box = 0;
		mouse_down = false;
	}

	selection->setWidget(m_current_box); //widget should be raise()'ed before calling this to display correctly
}

void DragArea::mouseReleaseEvent( QMouseEvent * )
{
	mouse_down = false;
}

void DragArea::mouseMoveEvent( QMouseEvent *e )
{
	if ( mouse_down && m_current_box )
	{
		// calc correct position
		QPoint pos = m_current_box->mapFromGlobal( e->globalPos() );

		// calc move distance and store it
		QPoint d = m_last_point - pos;
		if ( d.x() == 0 )
			pos.setX( m_last_point.x() );
		if ( d.y() == 0 )
			pos.setY( m_last_point.y() );
		m_last_point = pos;

		// snap to grid
		int x = widgetGeom.x() - d.x();
		widgetGeom.setX( x );
		int y = widgetGeom.y() - d.y();
		widgetGeom.setY( y );
		QPoint p = m_current_box->pos();

		if ( x - p.x() != 0 || y - p.y() != 0 ) // if we actually have to move
			moveWidget( m_current_box, x - p.x(), y - p.y() );

		m_last_point += ( p - m_current_box->pos() );
	}
}

void DragArea::moveWidget( QWidget *w, int dx, int dy )
{
	w->move( w->x() + dx, w->y() + dy );
	selection->updateGeometry();
}
