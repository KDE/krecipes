/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef KREMENU_H
#define KREMENU_H

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

class Menu;
class KreMenu;
class KreMenuButton;
typedef QValueList <Menu>::Iterator MenuId;


class Menu
{
public:
	// Methods
	
	Menu(void);
	Menu(const Menu &m);
	~Menu(void);
	void addButton(KreMenuButton *button);
	Menu& Menu::operator=(const Menu &m);
	
	// Variables
	
	QMap <KreMenuButton*,int> positionList; // Stores the indexes for the widgets
	QMap <int,KreMenuButton*> widgetList; // Stores the widgets for each position (just the inverse mapping)
	KreMenuButton* activeButton; // Indicates which button is highlighted
	int childPos;
	int widgetNumber;
private:
	// Methods
	void copyMap(QMap <int,KreMenuButton*> &destMap, const QMap <int,KreMenuButton*> &origMap);
	void copyMap(QMap <KreMenuButton*,int> &destMap, const QMap <KreMenuButton*,int> &origMap);
};


class KreMenu : public QWidget
{
Q_OBJECT
public:
	KreMenu(QWidget *parent = 0,const char *name = 0);
	~KreMenu();
	
	MenuId createSubMenu(const QString &title, const QString &icon);
	MenuId mainMenu(void){return mainMenuId;}
	MenuId currentMenu(void){return currentMenuId;}
	QSize sizeHint() const;
	void resizeEvent(QResizeEvent* e);
	void highlightButton(KreMenuButton *button);
	
	
protected:

	virtual void paintEvent (QPaintEvent *e);
	virtual void childEvent (QChildEvent *e);
	virtual void mouseMoveEvent (QMouseEvent *e);
	virtual void mousePressEvent (QMouseEvent *e);
	virtual void mouseReleaseEvent (QMouseEvent *e);


private:
	//Variables
	QValueList <Menu> menus;
	MenuId mainMenuId;
	MenuId currentMenuId;
	Menu *m_currentMenu;
	
	
	int xOrig, yOrig; //For dragging the menu
	int xDest, yDest;
	bool dragging;

signals:
  	void resized(int, int);
	void clicked(int);
private slots:
	void collectClicks(KreMenuButton *w);
	void showMenu(MenuId id);

};

class KreMenuButton: public QWidget
{
Q_OBJECT
public:
	KreMenuButton(KreMenu *parent,MenuId id=0, const char *name = 0);

	~KreMenuButton();

	QSize sizeHint() const;

	QString title(void){return text;}
	void setActive(bool active=true){highlighted=active;}
	void setIconSet(const QIconSet &is);
	MenuId menuId;
	MenuId subMenuId;

signals:
	void resized(int, int);
	void clicked(void);
	void clicked(KreMenuButton*); // sent together with clicked()
	void clicked(MenuId); // sent together with clicked()

public slots:
	void setTitle(const QString &s) {text=s; update();}
	void rescale(void);

private:
	// Button parts
	 QPixmap* icon;
	 QString text;
	 bool highlighted;

private slots:

	void forwardClicks(void){emit clicked(this); if (subMenuId!=0) emit clicked(subMenuId);}

protected:

	virtual void paintEvent(QPaintEvent *e );
	virtual void mousePressEvent (QMouseEvent * e);

};



#endif
