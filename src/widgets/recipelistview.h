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

#ifndef RECIPELISTVIEW_H
#define RECIPELISTVIEW_H

#include <qdragobject.h>

#include "categorylistview.h"
#include "recipe.h"

class QDragObject;
class QDropEvent;

class RecipeDB;

#define RECIPELISTITEM_RTTI 1000

#define RECIPEITEMMIMETYPE "data/x-kde.recipe.item"

class RecipeListItem : public QListViewItem
{
public:
	RecipeListItem( QListView* qlv, const Recipe &r ) : QListViewItem( qlv )
	{
		init( r );
	}

	RecipeListItem( QListView* qlv, QListViewItem *after, const Recipe &r ) : QListViewItem( qlv, after )
	{
		init( r );
	}

	RecipeListItem( CategoryListItem* it, const Recipe &r ) : QListViewItem( it )
	{
		init( r );
	}

	int rtti() const
	{
		return RECIPELISTITEM_RTTI;
	}

	~RecipeListItem( void )
	{
		delete recipeStored;
	}

	int recipeID() const
	{
		return recipeStored->recipeID;
	}
	QString title() const
	{
		return recipeStored->title;
	}

	void setRecipeID( int id )
	{
		recipeStored->recipeID = id;
	}
	void setTitle( const QString &title )
	{
		recipeStored->title = title;
	}

protected:
	Recipe *recipeStored;

public:
	virtual QString text( int column ) const
	{
		switch ( column ) {
		case 0:
			return ( recipeStored->title );
			break;
		case 1:
			return ( QString::number( recipeStored->recipeID ) );
			break;
		default:
			return ( QString::null );
		}
	}

private:
	void init( const Recipe &r )
	{
		recipeStored = new Recipe();

		//note: we only store the title and id
		recipeStored->recipeID = r.recipeID;
		recipeStored->title = r.title;
	}
};

class RecipeItemDrag : public QStoredDrag
{
public:
	RecipeItemDrag( RecipeListItem *recipeItem, QWidget *dragSource = 0, const char *name = 0 );
	static bool canDecode( QMimeSource* e );
	static bool decode( const QMimeSource* e, RecipeListItem& item );
};

class RecipeListView : public StdCategoryListView
{
	Q_OBJECT

public:
	RecipeListView( QWidget *parent, RecipeDB *db );

protected slots:
	virtual void createRecipe( const Recipe &, int parent_id );
	virtual void createRecipe( const Element &recipe, const ElementList &categories );
	virtual void modifyRecipe( const Element &recipe, const ElementList &categories );
	virtual void removeRecipe( int );
	virtual void removeRecipe( int, int );

protected:
	virtual void removeCategory( int id );
	virtual QDragObject *dragObject();
	virtual bool acceptDrag( QDropEvent *event ) const;

	void load(int limit, int offset);

private:
	void moveChildrenToRoot( QListViewItem * );

	bool flat_list;
};

#endif //RECIPELISTVIEW_H
