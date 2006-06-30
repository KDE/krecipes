/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef LISTVIEWHANDLER_H
#define LISTVIEWHANDLER_H

#include <qobject.h>
#include <qmap.h>

#include <klistview.h>

#define PREVLISTITEM_RTTI 1002
#define NEXTLISTITEM_RTTI 1003

class KProgressDialog;

class RecipeDB;

class DBListViewBase : public KListView
{
Q_OBJECT

public:
	DBListViewBase( QWidget *, RecipeDB *, int total );
	~DBListViewBase();

	void reload( bool force=false );

signals:
	void nextGroupLoaded();
	void prevGroupLoaded();

protected:
	/**
	 * Called when the list view is ready to be used, i.e., it has been loaded with data.
	 * Until the list view has been loaded, we can ignore all database signals regarding changes
	 * of data.  Therefore, subclasses should connect to these signals during this call.
	 */
	virtual void init(){}
	virtual void load(int limit, int offset) = 0;
	virtual void keyPressEvent( QKeyEvent *e );
	bool handleElement( const QString & );
	virtual void createElement( QListViewItem * );
	void removeElement( QListViewItem *, bool delete_item = true );

	bool reloading(){ return bulk_load; }
	void setSorting(int c){KListView::setSorting(c);} //don't do sorting, the database comes sorted from the database anyways
	void setTotalItems(int);

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

	QMap<QListViewItem*,QListViewItem*> lastElementMap;
	QListViewItem *delete_me_later;

	KProgressDialog *m_progress;
	int m_totalSteps;
};

#endif //LISTVIEWHANDLER_H
