/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#ifndef PANELDECO_H
#define PANELDECO_H


#include <QString>

#include <QPixmap>

#include <qwidget.h>
#include <QStackedWidget>
//Added by qt3to4:
#include <QPaintEvent>
#include <QChildEvent>
#include <kvbox.h>


/**
* @author Unai Garro
*/

class PanelDeco;
class TopDeco;

class PanelDeco : public QWidget
{
	Q_OBJECT
public:
	// Methods
	explicit PanelDeco( QWidget *parent = 0, const QString &title = QString(), const QString &iconName = QString() );
	~PanelDeco();
	int id( QWidget* w ); // obtain the id of the given panel
	QWidget* visiblePanel( void ); // obtain the current active panel no.
	void addStackWidget( QWidget *w );

signals:
	void panelRaised( QWidget *w, QWidget *old_w );

private:
	TopDeco *tDeco;
	QStackedWidget *stack;

public slots:
	void raise( QWidget *w );
	void setHeader( const QString &title = QString(), const QString &icon = QString() );

};

class TopDeco: public QWidget
{
	Q_OBJECT
public:
	explicit TopDeco( QWidget *parent = 0, const char *name = 0, const QString &title = QString(), const QString &iconName = QString() );
	~TopDeco();

	virtual QSize sizeHint( void ) const;
public slots:
	void setHeader( const QString &title = QString(), const QString &iconName = QString() );
protected:
	virtual void paintEvent( QPaintEvent *e );
private:
	QPixmap icon;
	QString panelTitle;
};

#endif
