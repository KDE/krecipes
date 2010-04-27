/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef KREMENU_H
#define KREMENU_H

#include <QIcon>
#include <qmap.h>
#include <QString>
//Added by qt3to4:
#include <QPixmap>
#include <QResizeEvent>
#include <QMouseEvent>
#include <Q3ValueList>
#include <QChildEvent>
#include <QKeyEvent>
#include <QPaintEvent>

#include "krecipesview.h" //for KrePanel enum


/**
* @author Unai Garro
* @author Bosselut Cyril
*/

class Menu;
class KreMenu;
class KreMenuButton;
typedef Q3ValueList <Menu>::Iterator MenuId;


class Menu
{
public:
	// Methods

	Menu( void );
	Menu( const Menu &m );
	~Menu( void );
	void addButton( KreMenuButton *button );
	Menu& operator=( const Menu &m );

	// Variables

	QMap <KreMenuButton*, int> positionList; // Stores the indexes for the widgets
	QMap <int, KreMenuButton*> widgetList; // Stores the widgets for each position (just the inverse mapping)
	KreMenuButton* activeButton; // Indicates which button is highlighted
	int childPos;
	int widgetNumber;
private:
	// Methods
	void copyMap( QMap <int, KreMenuButton*> &destMap, const QMap <int, KreMenuButton*> &origMap );
	void copyMap( QMap <KreMenuButton*, int> &destMap, const QMap <KreMenuButton*, int> &origMap );
};


class KreMenu : public QWidget
{
	Q_OBJECT
public:
	explicit KreMenu( QWidget *parent = 0, const char *name = 0 );
	~KreMenu();

	MenuId createSubMenu( const QString &title, const QString &icon );
	MenuId mainMenu( void )
	{
		return mainMenuId;
	}
	MenuId currentMenu( void )
	{
		return currentMenuId;
	}
	QSize sizeHint() const;
	QSize minimumSizeHint() const;
	void resizeEvent( QResizeEvent* e );
	void highlightButton( KreMenuButton *button );


protected:

	virtual void paintEvent ( QPaintEvent *e );
	virtual void childEvent ( QChildEvent *e );
	virtual void keyPressEvent( QKeyEvent *e );

private:
	//Variables
	Q3ValueList <Menu> menus;
	MenuId mainMenuId;
	MenuId currentMenuId;
	Menu *m_currentMenu;

signals:
	void resized( int, int );
	void clicked( KrePanel );

private slots:
	void collectClicks( KreMenuButton *w );
	void showMenu( MenuId id );

};

class KreMenuButton: public QWidget
{
	Q_OBJECT
public:
	explicit KreMenuButton( KreMenu *parent, KrePanel panel = KrePanel( -1 ), MenuId id = MenuId() );

	~KreMenuButton();

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	QString title( void )
	{
		return text;
	}
	void setActive( bool active = true )
	{
		highlighted = active;
	}
	void setIconSet( const QIcon &is );
	MenuId menuId;
	MenuId subMenuId;

	KrePanel getPanel() const
	{
		return panel;
	}

signals:
	void resized( int, int );
	void clicked( void );
	void clicked( KreMenuButton* ); // sent together with clicked()
	void clicked( MenuId ); // sent together with clicked()

public slots:
	void setTitle( const QString &s );
	void rescale( void );

private:
	// Button parts
	QPixmap icon;
	QString text;
	bool highlighted;

	KrePanel panel;

private slots:

	void forwardClicks( void )
	{
		emit clicked( this );
		if ( subMenuId != MenuId() )
			emit clicked( subMenuId );
	}

protected:

	virtual void paintEvent( QPaintEvent *e );
	virtual void mousePressEvent ( QMouseEvent * e );

};



#endif
