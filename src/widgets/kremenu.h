/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro (ugarro@users.sourceforge.net)       *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef MENUGROUP_H
#define MENUGROUP_H

#include <qbuttongroup.h>
#include <qevent.h>
#include <qiconset.h>
#include <qmap.h>
#include <qpushbutton.h>
#include <qstring.h>


/**
* @author Unai Garro
* @author Bosselut Cyril
*/
class KreMenu : public QWidget
{
Q_OBJECT
public:
	KreMenu(QWidget *parent = 0, const char *name = 0);

	~KreMenu();

	//QSize sizeHint () const {QSize size;size=QButtonGroup::sizeHint();size.setWidth(150);return(size);}
	QSize sizeHint() const;
	void resizeEvent(QResizeEvent* e);
protected:

	virtual void paintEvent(QPaintEvent *e );
	virtual void childEvent ( QChildEvent *e );
private:
	//Variables
	int childPos;
	int widgetNumber;
	QMap <QWidget*,int> widgetList; // Stores the indexes for the widgets

signals:
  	void resized(int, int);
	void clicked(int);
private slots:
	void collectClicks(QWidget *w);

};

class KreMenuButton: public QWidget
{
Q_OBJECT
public:
	KreMenuButton(QWidget *parent = 0, const char *name = 0);

	~KreMenuButton();

	QSize sizeHint() const;

	void setTitle(const QString &s) {text=s;}
	QString title(void){return text;}
	void setIconSet(const QIconSet &is);

private:
	// Button parts
	 QPixmap* icon;
	 QString text;

protected:

	virtual void paintEvent(QPaintEvent *e );
	virtual void mousePressEvent (QMouseEvent * e);

signals:
	void resized(int, int);
	void clicked(void);
	void clicked(QWidget*); // sent together with clicked()

private slots:
	void rescale(int w, int h);
	void forwardClicks(void){emit clicked(this);}

};

#endif
