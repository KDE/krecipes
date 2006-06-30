/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef PREPMETHODLISTVIEW_H
#define PREPMETHODLISTVIEW_H

#include "dblistviewbase.h"

#include "datablocks/element.h"

class RecipeDB;
class KPopupMenu;

class PrepMethodListView : public DBListViewBase
{
	Q_OBJECT

public:
	PrepMethodListView( QWidget *parent, RecipeDB *db );

public slots:
	virtual void load( int curr_limit, int curr_offset );

protected slots:
	virtual void createPrepMethod( const Element & ) = 0;
	virtual void removePrepMethod( int ) = 0;

	void checkCreatePrepMethod( const Element &el );

protected:
	virtual void init();
};


class StdPrepMethodListView : public PrepMethodListView
{
	Q_OBJECT

public:
	StdPrepMethodListView( QWidget *parent, RecipeDB *db, bool editable = false );

protected:
	virtual void createPrepMethod( const Element & );
	virtual void removePrepMethod( int );

private slots:
	void showPopup( KListView *, QListViewItem *, const QPoint & );

	void createNew();
	void remove
		();
	void rename();

	void modPrepMethod( QListViewItem* i );
	void savePrepMethod( QListViewItem* i );

private:
	bool checkBounds( const QString &name );

	KPopupMenu *kpop;
};

#endif //PREPMETHODLISTVIEW_H
