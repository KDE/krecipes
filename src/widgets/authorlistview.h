/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef AUTHORLISTVIEW_H
#define AUTHORLISTVIEW_H

#include "klistview.h"

#include "element.h"

class RecipeDB;
class KPopupMenu;

class AuthorListView : public KListView
{
	Q_OBJECT

public:
	AuthorListView( QWidget *parent, RecipeDB *db );

	void reload();

protected:
	RecipeDB *database;

protected slots:
	virtual void createAuthor( const Element & ) = 0;
	virtual void removeAuthor( int ) = 0;

private:
	//make this private because the data should always be synced with the database
	void clear()
	{
		KListView::clear();
	}
};

class StdAuthorListView : public AuthorListView
{
	Q_OBJECT

public:
	StdAuthorListView( QWidget *parent, RecipeDB *db, bool editable = false );

protected:
	virtual void createAuthor( const Element & );
	virtual void removeAuthor( int );

private slots:
	void showPopup( KListView *, QListViewItem *, const QPoint & );

	void createNew();
	void remove
		();
	void rename();

	void modAuthor( QListViewItem* i );
	void saveAuthor( QListViewItem* i );

private:
	bool checkBounds( const QString &name );

	KPopupMenu *kpop;
};

#endif //AUTHORLISTVIEW_H
