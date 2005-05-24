/***************************************************************************
*   Copyright (C) 2003 by Jason Kivlighn                                  *
*   mizunoami44@users.sourceforge.net                                     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef DRAGAREA_H
#define DRAGAREA_H

#include <qwidget.h>
#include <qpoint.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QEvent>

class QMouseEvent;
class QEvent;

class WidgetSelection;

/**
  * @author Jason Kivlighn
  */
class DragArea : public QWidget
{
	Q_OBJECT

public:
	DragArea( QWidget *parent = 0, const char *name = 0 );
	~DragArea();

	void setWidget( QWidget * );
	void setReadOnly( bool );

	QPoint gridSpacing()
	{
		return grid_spacing;
	}
	void setGridSpacing( int x, int y )
	{
		grid_spacing.setX( x );
		grid_spacing.setY( y );
	}

signals:
	void widgetClicked( QMouseEvent *, QWidget* );
	void widgetGeometryChanged();

protected:
	friend class MousePressEater;

	virtual void mousePressEvent( QMouseEvent * );
	virtual void mouseReleaseEvent( QMouseEvent * );
	virtual void mouseMoveEvent( QMouseEvent * );

	void moveWidget( QWidget *w, int dx, int dy );

	void update();

protected slots:
	void emitWidgetGeometryChanged()
	{
		emit widgetGeometryChanged();
	}

private:
	bool m_read_only;

	bool mouse_down;
	QPoint m_last_point;
	QPoint m_begin_point;
	QWidget *m_current_box;
	QRect widgetGeom;
	QPoint grid_spacing;
	WidgetSelection *selection;
};

#endif //DRAGAREA_H
