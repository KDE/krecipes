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

#include "element.h"

class RecipeDB;

class UnitListView : public KListView
{
Q_OBJECT

public:
	UnitListView( QWidget *parent, RecipeDB *db );

	void reload();

protected:
	RecipeDB *database;

protected slots:
	virtual void createUnit(const Element &)=0;
	virtual void removeUnit(int)=0;

private:
	//make this private because the data should always be synced with the database
	void clear(){ KListView::clear(); }
};

class StdUnitListView : public UnitListView
{
Q_OBJECT

public:
	StdUnitListView( QWidget *parent, RecipeDB *db, bool editable=false );

protected:
	virtual void createUnit(const Element &);
	virtual void removeUnit(int);

private slots:
	void modUnit(QListViewItem* i);
	void saveUnit(QListViewItem* i);
};

#endif //UNITLISTVIEW_H
