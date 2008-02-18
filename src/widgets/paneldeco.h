/***************************************************************************
*   Copyright (C) 2003 by Unai Garro (ugarro@users.sourceforge.net)       *
*                                                                         *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#ifndef PANELDECO_H
#define PANELDECO_H


#include <qevent.h>
#include <qicon.h>
#include <qstring.h>

#include <qpixmap.h>

#include <qwidget.h>
#include <q3widgetstack.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QChildEvent>
#include <kvbox.h>


/**
* @author Unai Garro
*/

class PanelDeco;
class LeftDeco;
class TopDeco;

class PanelDeco : public KVBox
{
	Q_OBJECT
public:
	// Methods
	PanelDeco( QWidget *parent = 0, const char *name = 0, const QString &title = QString::null, const QString &iconName = QString::null );
	~PanelDeco();
	int id( QWidget* w ); // obtain the id of the given panel
	QWidget* visiblePanel( void ); // obtain the current active panel no.

signals:
	void panelRaised( QWidget *w, QWidget *old_w );

private:
	KHBox *hbox;
	LeftDeco *lDeco;
	TopDeco *tDeco;
	Q3WidgetStack *stack;

public slots:
	void raise( QWidget *w );
	void setHeader( const QString &title = QString::null, const QString &icon = QString::null );
protected:
	virtual void childEvent( QChildEvent *e );


};

class LeftDeco: public QWidget
{
	Q_OBJECT
public:
	LeftDeco( QWidget *parent = 0, const char *name = 0 );

	~LeftDeco();
};

class TopDeco: public QWidget
{
	Q_OBJECT
public:
	TopDeco( QWidget *parent = 0, const char *name = 0, const QString &title = QString::null, const QString &iconName = QString::null );
	~TopDeco();
	virtual QSize sizeHint( void );
public slots:
	void setHeader( const QString &title = QString::null, const QString &iconName = QString::null );
protected:
	virtual void paintEvent( QPaintEvent *e );
private:
	QPixmap icon;
	QString panelTitle;
};

#endif
