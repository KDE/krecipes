/***************************************************************************
*   Copyright © 2004 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2004, 2006 Jason Kivlighn <jkivlighn@gmail.com>           *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef RECIPELISTVIEW_H
#define RECIPELISTVIEW_H

#include <q3dragobject.h>
//Added by qt3to4:
#include <QDropEvent>

#include "categorylistview.h"
#include "datablocks/recipe.h"

class Q3DragObject;
class QDropEvent;

class KProgressDialog;

class RecipeDB;

#define RECIPELISTITEM_RTTI 1000

#define RECIPEITEMMIMETYPE "data/x-kde.recipe.item"

class RecipeListItem : public Q3ListViewItem
{
public:
	RecipeListItem( Q3ListView* qlv, const Recipe &r ) : Q3ListViewItem( qlv )
	{
		init( r );
	}

	RecipeListItem( Q3ListView* qlv, Q3ListViewItem *after, const Recipe &r ) : Q3ListViewItem( qlv, after )
	{
		init( r );
	}

	RecipeListItem( CategoryListItem* it, const Recipe &r ) : Q3ListViewItem( it )
	{
		init( r );
	}

	RecipeListItem( CategoryListItem* it, Q3ListViewItem *after, const Recipe &r ) : Q3ListViewItem( it, after )
	{
		init( r );
	}

	RecipeListItem( Q3ListViewItem* it, const Recipe &r ) : Q3ListViewItem( it )
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
			return ( QString() );
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

class RecipeItemDrag : public Q3StoredDrag
{
public:
	explicit RecipeItemDrag( RecipeListItem *recipeItem, QWidget *dragSource = 0, const char *name = 0 );
	static bool canDecode( QMimeSource* e );
	static bool decode( const QMimeSource* e, RecipeListItem& item );
};

class RecipeListView : public StdCategoryListView
{
	Q_OBJECT

public:
	explicit RecipeListView( QWidget *parent, RecipeDB *db );

public slots:
	void populateAll( Q3ListViewItem *parent = 0 );

protected slots:
	virtual void createRecipe( const Recipe &, int parent_id );
	virtual void createRecipe( const Element &recipe, const ElementList &categories );
	virtual void modifyRecipe( const Element &recipe, const ElementList &categories );
	virtual void removeRecipe( int );
	virtual void removeRecipe( int, int );

protected:
	virtual void createElement( Q3ListViewItem * );
	virtual void removeCategory( int id );
	virtual Q3DragObject *dragObject();
	virtual bool acceptDrag( QDropEvent *event ) const;
	virtual void populate( Q3ListViewItem *item );
	virtual QString tooltip(Q3ListViewItem *item, int column) const;

	friend class RecipeListToolTip;

	void load(int limit, int offset);

private:
	void moveChildrenToRoot( Q3ListViewItem * );

	bool flat_list;
	Q3ListViewItem *m_uncat_item;
	Q3ListViewItem *lastElementCurrLevel;

	KProgressDialog *m_progress_dlg;
};

#endif //RECIPELISTVIEW_H
