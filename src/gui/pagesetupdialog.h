/***************************************************************************
 *   Copyright (C) 2003 by Jason Kivlighn                                  *
 *   mizunoami44@users.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <qdialog.h>
#include <qpoint.h>

class QMouseEvent;

class PageSetupDialog : public QDialog
{
public:
	PageSetupDialog( QWidget *parent );

private:

};

class DraggableWidget : public QWidget
{
public:
	DraggableWidget( QWidget *parent, const char *name, const QString & text );

protected:
	virtual void mousePressEvent( QMouseEvent * );
	virtual void mouseReleaseEvent( QMouseEvent * );
	virtual void mouseMoveEvent( QMouseEvent * );
	virtual void paintEvent( QPaintEvent * );

private:
	bool mouse_down;
	QString m_text;
	QPoint last_point;
};
