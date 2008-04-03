/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef HEADERLISTVIEW_H
#define HEADERLISTVIEW_H

#include "dblistviewbase.h"

#include "datablocks/element.h"

class RecipeDB;
class KMenu;

class HeaderListView : public DBListViewBase
{
	Q_OBJECT

public:
	HeaderListView( QWidget *parent, RecipeDB *db );

public slots:
	virtual void load( int curr_limit, int curr_offset );

protected slots:
	virtual void createHeader( const Element & ) = 0;
	virtual void removeHeader( int ) = 0;

	void checkCreateHeader( const Element &el );

protected:
	virtual void init();
};

class StdHeaderListView : public HeaderListView
{
	Q_OBJECT

public:
	StdHeaderListView( QWidget *parent, RecipeDB *db, bool editable = false );

protected:
	virtual void createHeader( const Element & );
	virtual void removeHeader( int );

private slots:
	void showPopup( K3ListView *, Q3ListViewItem *, const QPoint & );

	void createNew();
	void remove();
	void rename( Q3ListViewItem* /*item*/,int /*c*/ );
        void slotRename();

	void modHeader( Q3ListViewItem* i, const QPoint &p, int c );
	void saveHeader( Q3ListViewItem* i, const QString &text, int c );

private:
	bool checkBounds( const QString &unit );

	KMenu *kpop;
};

#endif //HEADERLISTVIEW_H
