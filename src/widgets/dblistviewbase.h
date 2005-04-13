/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef LISTVIEWHANDLER_H
#define LISTVIEWHANDLER_H

#include <qobject.h>

#include <klistview.h>

#define PREVLISTITEM_RTTI 1002
#define NEXTLISTITEM_RTTI 1003

class RecipeDB;

class DBListViewBase : public KListView
{
Q_OBJECT

public:
	DBListViewBase( QWidget *, RecipeDB *, int total );

	void reload();

signals:
	void nextGroupLoaded();
	void prevGroupLoaded();

protected:
	virtual void load(int limit, int offset) = 0;
	virtual void keyPressEvent( QKeyEvent *e );
	bool handleElement( const QString & );
	void createElement( QListViewItem * );
	void removeElement( QListViewItem * );

	bool reloading(){ return bulk_load; }
	void setSorting(int c){KListView::setSorting(c);} //don't do sorting, the database comes sorted from the database anyways

	RecipeDB *database;
	int curr_limit;
	int curr_offset;

protected slots:
	void rename( QListViewItem *, int c );
	void slotDoubleClicked( QListViewItem * );

private:
	void activatePrev();
	void activateNext();

	//make this private because the data should always be synced with the database
	void clear(){KListView::clear();}

	int total;
	
	bool bulk_load;

	QListViewItem *lastElement;
};

#endif //LISTVIEWHANDLER_H
