/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef UNITLISTVIEW_H
#define UNITLISTVIEW_H

#include "klistview.h"

#include "datablocks/unit.h"

class RecipeDB;
class KPopupMenu;

class ListViewHandler;

class UnitListView : public KListView
{
	Q_OBJECT

public:
	UnitListView( QWidget *parent, RecipeDB *db );

	void reload();

public slots:
	virtual void reload( int curr_limit, int curr_offset );

protected:
	RecipeDB *database;

protected slots:
	virtual void createUnit( const Unit & ) = 0;
	virtual void removeUnit( int ) = 0;

private:
	//make this private because the data should always be synced with the database
	void clear()
	{
		KListView::clear();
	}

	ListViewHandler *listViewHandler;
};

class StdUnitListView : public UnitListView
{
	Q_OBJECT

public:
	StdUnitListView( QWidget *parent, RecipeDB *db, bool editable = false );

protected:
	virtual void createUnit( const Unit & );
	virtual void removeUnit( int );

private slots:
	void showPopup( KListView *, QListViewItem *, const QPoint & );

	void createNew();
	void remove
		();
	void rename();

	void modUnit( QListViewItem* i, const QPoint &p, int c );
	void saveUnit( QListViewItem* i, const QString &text, int c );

private:
	bool checkBounds( const Unit &unit );

	KPopupMenu *kpop;
};

#endif //UNITLISTVIEW_H
