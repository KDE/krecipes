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


class KreMenu;
class KreMenuButton;

class KreMenu : public QWidget
{
Q_OBJECT
public:
	KreMenu(QWidget *parent = 0, const char *name = 0);

	~KreMenu();

	QSize sizeHint() const;
	void resizeEvent(QResizeEvent* e);
protected:

	virtual void paintEvent (QPaintEvent *e);
	virtual void childEvent (QChildEvent *e);
	virtual void mouseMoveEvent (QMouseEvent *e);
	virtual void mousePressEvent (QMouseEvent *e);
	virtual void mouseReleaseEvent (QMouseEvent *e);


private:
	//Variables
	int childPos;
	int widgetNumber;
	QMap <KreMenuButton*,int> widgetList; // Stores the indexes for the widgets
	KreMenuButton* activeButton; // Indicates which button is highlighted

	int xOrig, yOrig; //For dragging the menu
	int xDest, yDest;
	bool dragging;

signals:
  	void resized(int, int);
	void clicked(int);
private slots:
	void collectClicks(KreMenuButton *w);

};

class KreMenuButton: public QWidget
{
Q_OBJECT
public:
	KreMenuButton(QWidget *parent = 0, const char *name = 0);

	~KreMenuButton();

	QSize sizeHint() const;

	QString title(void){return text;}
	void setActive(bool active=true){highlighted=active;}
	void setIconSet(const QIconSet &is);

signals:
	void resized(int, int);
	void clicked(void);
	void clicked(KreMenuButton*); // sent together with clicked()

public slots:
	void setTitle(const QString &s) {text=s; update();}

private:
	// Button parts
	 QPixmap* icon;
	 QString text;
	 bool highlighted;

private slots:
	void rescale(int w, int h);
	void forwardClicks(void){emit clicked(this);}

protected:

	virtual void paintEvent(QPaintEvent *e );
	virtual void mousePressEvent (QMouseEvent * e);

};

#endif
