/***************************************************************************
 *   Copyright (C) 2003 by Jason Kivlighn                                  *
 *   mizunoami44@users.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "pagesetupdialog.h"

#include <kapplication.h>
#include <kconfig.h>

#include <qpainter.h>
#include <qlabel.h>
#include <qlayout.h>

#include <cmath>

PageSetupDialog::PageSetupDialog( QWidget *parent ) : QDialog(parent,0,true)
{
	//KConfig *config=kapp->config();
	//config->setGroup("PageSetup");

	QWidget *dragSurface = new QWidget(this);
	dragSurface->resize(300,300);

	DraggableWidget *test = new DraggableWidget(dragSurface,"Test","text");
	DraggableWidget *test2 = new DraggableWidget(dragSurface,"Test2","text2");
}

DraggableWidget::DraggableWidget( QWidget *parent, const char *name, const QString &text ) : QWidget(parent,name),
 mouse_down(false),
 m_text(text),
 last_point(0,0)
{
	resize(50,50);

	setPaletteBackgroundColor ( QColor(0,0,0) );

	setMouseTracking(true);
}

void DraggableWidget::mousePressEvent( QMouseEvent *e )
{
	mouse_down = true;
}

void DraggableWidget::mouseReleaseEvent( QMouseEvent *e )
{
	mouse_down = false;
}


void DraggableWidget::mouseMoveEvent( QMouseEvent *e )
{
	QPoint this_point(mapToParent(e->pos())); //get the point in relation to parent, since this widget is on the move

	if ( mouse_down )
	{
		if ( e->x() <= 2 ) //resizing from left side
		{
			int deltax = this_point.x() - last_point.x();

			setGeometry( QRect( QPoint(this->x() + deltax, this->y()),
			  QSize(size().width() - deltax, size().height() ) ) );

		}
		else if ( pos().x() >= size().width() - 2 ) //resizing from right side
		{
			int deltax = this_point.x() - last_point.x();

			setGeometry( QRect( QPoint(this->x(), this->y()),
			  QSize(size().width() + deltax, size().height() ) ) );

		}
		else
		{
			QPoint delta_pos = this_point - last_point;

			move( pos() + delta_pos );
			//setGeometry( QRect(QPoint(x - last_point->x(), y - last_point->y()), QSize(50,50)) );
		}
	}

	last_point = this_point;
}

void DraggableWidget::paintEvent( QPaintEvent * )
{
	QPainter p(this);

	p.setPen( QPen( QColor(255,0,0) ) );
	p.drawText(10,10,m_text);
}

