/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef AUTHORLISTVIEW_H
#define AUTHORLISTVIEW_H

#include "dblistviewbase.h"
#include "datablocks/element.h"
#include <QList>

class RecipeDB;
class KMenu;

class AuthorCheckListView;

class AuthorCheckListItem: public Q3CheckListItem
{
public:
	AuthorCheckListItem( AuthorCheckListView* qlv, const Element &author );
	AuthorCheckListItem( AuthorCheckListView* qlv, Q3ListViewItem *after, const Element &author );

	Element author() const;

	virtual QString text( int column ) const;

protected:
	virtual void stateChange( bool on );

private:
	Element authorStored;
	AuthorCheckListView *m_listview;
};


class AuthorListView : public DBListViewBase
{
	Q_OBJECT

public:
	AuthorListView( QWidget *parent, RecipeDB *db );

protected slots:
	void checkCreateAuthor( const Element &el );
	virtual void createAuthor( const Element & ) = 0;
	virtual void removeAuthor( int ) = 0;
	virtual void load( int curr_limit, int curr_offset );

protected:
	virtual void init();
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
	void showPopup( K3ListView *, Q3ListViewItem *, const QPoint & );

	void createNew();
	void remove
		();
	void rename();

	void modAuthor( Q3ListViewItem* i );
	void saveAuthor( Q3ListViewItem* i );

private:
	bool checkBounds( const QString &name );

	KMenu *kpop;
};


class AuthorCheckListView : public AuthorListView
{
public:
	AuthorCheckListView( QWidget *parent, RecipeDB *db );

	virtual void stateChange(AuthorCheckListItem *,bool);

	QList<Element> selections() const{ return m_selections; }

protected:
	virtual void createAuthor( const Element &ing );
	virtual void removeAuthor( int );

	virtual void load( int limit, int offset );

private:
	QList<Element> m_selections;
};

#endif //AUTHORLISTVIEW_H
