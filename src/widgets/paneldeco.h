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
#include <qiconset.h>
#include <qstring.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qwidget.h>
#include <qwidgetstack.h>


/**
* @author Unai Garro
*/

class PanelDeco;
class LeftDeco;
class TopDeco;

class PanelDeco : public QVBox
{
Q_OBJECT
public:
	// Methods
	PanelDeco(QWidget *parent = 0, const char *name = 0);
	~PanelDeco();
	int id(QWidget* w); // obtain the id of the given panel
	QWidget* visiblePanel(void); // obtain the current active panel no.
private:
	QHBox *hbox;
	LeftDeco *lDeco;
	TopDeco *tDeco;
	QWidgetStack *stack;

public slots:
	void raise(QWidget *w);
protected:
	virtual void childEvent(QChildEvent *e);


};

class LeftDeco: public QWidget
{
Q_OBJECT
public:
	LeftDeco(QWidget *parent = 0, const char *name = 0);

	~LeftDeco();
};

class TopDeco: public QWidget
{
Q_OBJECT
public:
	TopDeco(QWidget *parent = 0, const char *name = 0);

	~TopDeco();
};

#endif