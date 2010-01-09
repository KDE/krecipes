/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2004 Unai Garro <ugarro@gmail.com>                        *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef CATEGORYLISTVIEW_H
#define CATEGORYLISTVIEW_H

#include <qmap.h>
#include <QPixmap>

#include "dblistviewbase.h"

#include "datablocks/elementlist.h"

class KMenu;

class RecipeDB;
class CategoryCheckListView;

#define CATEGORYCHECKLISTITEM_RTTI 1005
#define CATEGORYLISTITEM_RTTI 1001
#define PSEUDOLISTITEM_RTTI 1008

/** Category listitems inherit this class to provide a common interface for accessing this information.
 */
class CategoryItemInfo
{
public:
	CategoryItemInfo( const Element &category ) : ctyStored( category ), populated(false){}
	bool isPopulated() const { return populated; }
	void setPopulated( bool b ){ populated = b; }

	Element element() const
	{
		return ctyStored;
	}

	int categoryId() const
	{
		return ctyStored.id;
	}
	QString categoryName() const
	{
		return ctyStored.name;
	}

protected:
	Element ctyStored;

private:
	bool populated;
};

class CategoryCheckListItem : public Q3CheckListItem, public CategoryItemInfo
{
public:
	CategoryCheckListItem( CategoryCheckListView* klv, const Element &category, bool exclusive = true );
	CategoryCheckListItem( Q3ListViewItem* it, const Element &category, bool exclusive = true );
	CategoryCheckListItem( CategoryCheckListView* klv, Q3ListViewItem* it, const Element &category, bool exclusive = true );

	virtual QString text( int column ) const;
	virtual void setText( int column, const QString &text );

	int rtti() const
	{
		return CATEGORYCHECKLISTITEM_RTTI;
	}

protected:
	virtual void stateChange( bool );
	void setChildrenState( bool );
	void setParentsState( bool );

	bool locked;
	bool exclusive;

private:
	CategoryCheckListView *m_listview;
};


class CategoryListItem : public Q3ListViewItem, public CategoryItemInfo
{
public:
	CategoryListItem( Q3ListView* klv, const Element &category );
	CategoryListItem( Q3ListViewItem* it, const Element &category );
	CategoryListItem( Q3ListView* klv, Q3ListViewItem* it, const Element &category );

	virtual QString text( int column ) const;
	virtual void setText( int column, const QString &text );

	int rtti() const
	{
		return CATEGORYLISTITEM_RTTI;
	}
};



class CategoryListView : public DBListViewBase
{
	Q_OBJECT

public:
	CategoryListView( QWidget *parent, RecipeDB * );
	~CategoryListView(){}

	void populateAll( Q3ListViewItem *parent = 0 );

public slots:
	void open( Q3ListViewItem *item );

protected:
	QMap<int, Q3ListViewItem*> items_map;

	virtual void load( int limit, int offset );

	/** so that it allows dropping into
	  * subchildren that aren't expandable.  The code is taken from KDE's K3ListView with
	  * one line commented out.
	  */
	void findDrop( const QPoint &pos, Q3ListViewItem *&parent, Q3ListViewItem *&after )
	{
		QPoint p ( contentsToViewport( pos ) );

		// Get the position to put it in
		Q3ListViewItem *atpos = itemAt( p );

		Q3ListViewItem *above;
		if ( !atpos )  // put it at the end
			above = lastItem();
		else {
			// Get the closest item before us ('atpos' or the one above, if any)
			if ( p.y() - itemRect( atpos ).topLeft().y() < ( atpos->height() / 2 ) )
				above = atpos->itemAbove();
			else
				above = atpos;
		}

		if ( above ) {
			// if above has children, I might need to drop it as the first item there

			if ( above->firstChild() && above->isOpen() ) {
				parent = above;
				after = 0;
				return ;
			}

			// Now, we know we want to go after "above". But as a child or as a sibling ?
			// We have to ask the "above" item if it accepts children.
			// ### NOTE: Here is the one line commented out so that "above" always accepts children
			//if (above->isExpandable())
			{
				// The mouse is sufficiently on the right ? - doesn't matter if 'above' has visible children
				if ( p.x() >= depthToPixels( above->depth() + 1 ) ||
					( above->isOpen() && above->childCount() > 0 ) )
				{
					parent = above;
					after = 0L;
					return ;
				}
			}

			// Ok, there's one more level of complexity. We may want to become a new
			// sibling, but of an upper-level group, rather than the "above" item
			Q3ListViewItem * betterAbove = above->parent();
			Q3ListViewItem * last = above;
			while ( betterAbove ) {
				// We are allowed to become a sibling of "betterAbove" only if we are
				// after its last child
				if ( last->nextSibling() == 0 ) {
					if ( p.x() < depthToPixels ( betterAbove->depth() + 1 ) )
						above = betterAbove; // store this one, but don't stop yet, there may be a better one
					else
						break; // not enough on the left, so stop
					last = betterAbove;
					betterAbove = betterAbove->parent(); // up one level
				}
				else
					break; // we're among the child of betterAbove, not after the last one
			}
		}
		// set as sibling
		after = above;
		parent = after ? after->parent() : 0L ;
	}

	virtual void movableDropEvent( Q3ListViewItem * parent, Q3ListViewItem * afterme );

protected slots:
	virtual void removeCategory( int id ) = 0;
	virtual void createCategory( const Element &category, int parent_id ) = 0;
	virtual void modifyCategory( const Element &category );
	virtual void modifyCategory( int id, int parent_id );
	virtual void mergeCategories( int id1, int id2 );

	virtual void checkCreateCategory( const Element &, int parent_id );
	virtual void populate( Q3ListViewItem *item );


private:
	Q3ListViewItem *m_item_to_delete;
};


class StdCategoryListView : public CategoryListView
{
	Q_OBJECT

public:
	StdCategoryListView( QWidget *parent, RecipeDB *, bool editable = false );
	~StdCategoryListView(){}

protected:
	virtual void removeCategory( int id );
	virtual void createCategory( const Element &category, int parent_id );

	void setPixmap( const QPixmap &pixmap );

private:
	QPixmap m_folder_icon;
};


class CategoryCheckListView : public CategoryListView
{
	Q_OBJECT

public:
	CategoryCheckListView( QWidget *parent, RecipeDB *, bool exclusive=true, const ElementList &init_items_checked = ElementList() );

	virtual void stateChange( CategoryCheckListItem*, bool );

	ElementList selections() const{ return m_selections; }

protected:
	virtual void removeCategory( int id );
	virtual void createCategory( const Element &category, int parent_id );

	virtual void load( int limit, int offset );

	bool exclusive;

private:
	ElementList m_selections;
};


class PseudoListItem : public Q3ListViewItem
{
public:
	PseudoListItem( Q3ListView* lv ) : Q3ListViewItem(lv){}
	PseudoListItem( Q3ListViewItem* it ) : Q3ListViewItem(it){}

protected:
	int rtti() const { return PSEUDOLISTITEM_RTTI; }
};

#endif //CATEGORYLISTVIEW_H
