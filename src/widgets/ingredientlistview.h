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

#ifndef INGREDIENTLISTVIEW_H
#define INGREDIENTLISTVIEW_H

#include "klistview.h"

#include "element.h"

class RecipeDB;
class KPopupMenu;
class IngredientCheckListView;
class ListViewHandler;

/**
@author Unai Garro
*/
class IngredientCheckListItem: public QCheckListItem
{
public:
	IngredientCheckListItem( IngredientCheckListView* qlv, const Element &ing );
	~IngredientCheckListItem( void );

	int id( void ) const;
	QString name( void ) const;
	Element ingredient() const;

	virtual QString text( int column ) const;

protected:
	virtual void stateChange( bool on );

private:
	Element *ingStored;
	IngredientCheckListView *m_listview;
};



class IngredientListView : public KListView
{
	Q_OBJECT

public:
	IngredientListView( QWidget *parent, RecipeDB *db );

	virtual void reload();

public slots:
	virtual void reload( int curr_limit, int curr_offset );

protected slots:
	virtual void createIngredient( const Element & ) = 0;
	virtual void removeIngredient( int ) = 0;

protected:
	//make this protected because the data should always be synced with the database
	void clear()
	{
		KListView::clear();
	}

	RecipeDB *database;

private:
	ListViewHandler *listViewHandler;
};



class StdIngredientListView : public IngredientListView
{
	Q_OBJECT

public:
	StdIngredientListView( QWidget *parent, RecipeDB *db, bool editable = false );

protected:
	virtual void createIngredient( const Element & );
	virtual void removeIngredient( int );

private slots:
	void showPopup( KListView *, QListViewItem *, const QPoint & );

	void createNew();
	void remove
		();
	void rename();

	void modIngredient( QListViewItem* i );
	void saveIngredient( QListViewItem* i );

private:
	bool checkBounds( const QString &name );

	KPopupMenu *kpop;
};



class IngredientCheckListView : public IngredientListView
{
public:
	IngredientCheckListView( QWidget *parent, RecipeDB *db );
	virtual void reload();
	virtual void stateChange(IngredientCheckListItem *,bool);

	QValueList<Element> selections() const{ return m_selections; }

protected:
	virtual void createIngredient( const Element &ing );
	virtual void removeIngredient( int );

private:
	QValueList<Element> m_selections;
};

#endif //INGREDIENTLISTVIEW_H
