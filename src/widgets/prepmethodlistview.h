/***************************************************************************
 *   Copyright (C) 2004 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PREPMETHODLISTVIEW_H
#define PREPMETHODLISTVIEW_H 
 
#include "klistview.h"

#include "element.h"

class RecipeDB;
class KPopupMenu;

class PrepMethodListView : public KListView
{
Q_OBJECT

public:
	PrepMethodListView( QWidget *parent, RecipeDB *db );

	void reload();

protected:
	RecipeDB *database;

protected slots:
	virtual void createPrepMethod(const Element &)=0;
	virtual void removePrepMethod(int)=0;

private:
	//make this private because the data should always be synced with the database
	void clear(){ KListView::clear(); }
};

class StdPrepMethodListView : public PrepMethodListView
{
Q_OBJECT

public:
	StdPrepMethodListView( QWidget *parent, RecipeDB *db, bool editable=false );

protected:
	virtual void createPrepMethod(const Element &);
	virtual void removePrepMethod(int);

private slots:
	void showPopup(KListView *, QListViewItem *, const QPoint &);

	void createNew();
	void remove();
	void rename();

	void modPrepMethod(QListViewItem* i);
	void savePrepMethod(QListViewItem* i);

private:
	KPopupMenu *kpop;
};

#endif //PREPMETHODLISTVIEW_H
