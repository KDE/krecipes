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

#ifndef CATEGORYLISTVIEW_H
#define CATEGORYLISTVIEW_H

#include <klistview.h>

#include "element.h"

class KPopupMenu;

class RecipeDB;
class CategoryTree;

class CategoryCheckListItem : public QCheckListItem {
public:
	CategoryCheckListItem(QListView* klv, const Element &category );
	CategoryCheckListItem(QListViewItem* it, const Element &category );

	~CategoryCheckListItem(void){}
	virtual QString text(int column) const;
	virtual void setText(int column, const QString &text);

	Element element() const{ return ctyStored; }

	int categoryId(void){return ctyStored.id;}
	QString categoryName(void){return ctyStored.name;}
	
protected:
	virtual void stateChange(bool);
	void setChildrenState( bool );
	void setParentsState( bool );

	bool locked;

private:
	Element ctyStored;
};



class CategoryListView : public KListView
{
Q_OBJECT

public:
	CategoryListView( QWidget *parent, RecipeDB * );
	
public slots:
	void reload(void);

protected:
	/** so that it allows dropping into 
          * subchildren that aren't expandable.  The code is taken from KDE's KListView with
          * one line commented out.
	  */
	void findDrop(const QPoint &pos, QListViewItem *&parent, QListViewItem *&after)
	{
		QPoint p (contentsToViewport(pos));
		
		// Get the position to put it in
		QListViewItem *atpos = itemAt(p);
		
		QListViewItem *above;
		if (!atpos) // put it at the end
			above = lastItem();
		else
		{
			// Get the closest item before us ('atpos' or the one above, if any)
			if (p.y() - itemRect(atpos).topLeft().y() < (atpos->height()/2))
			above = atpos->itemAbove();
			else
			above = atpos;
		}
		
		if (above)
		{
			// if above has children, I might need to drop it as the first item there
		
			if (above->firstChild() && above->isOpen())
			{
			parent = above;
			after = 0;
			return;
			}
		
		// Now, we know we want to go after "above". But as a child or as a sibling ?
		// We have to ask the "above" item if it accepts children.
		// ### NOTE: Here is the one line commented out so that "above" always accepts children
		//if (above->isExpandable())
		{
			// The mouse is sufficiently on the right ? - doesn't matter if 'above' has visible children
			if (p.x() >= depthToPixels( above->depth() + 1 ) ||
			(above->isOpen() && above->childCount() > 0) )
			{
			parent = above;
			after = 0L;
			return;
			}
		}
		
		// Ok, there's one more level of complexity. We may want to become a new
		// sibling, but of an upper-level group, rather than the "above" item
		QListViewItem * betterAbove = above->parent();
		QListViewItem * last = above;
		while ( betterAbove )
		{
			// We are allowed to become a sibling of "betterAbove" only if we are
			// after its last child
			if ( last->nextSibling() == 0 )
			{
			if (p.x() < depthToPixels ( betterAbove->depth() + 1 ))
				above = betterAbove; // store this one, but don't stop yet, there may be a better one
			else
				break; // not enough on the left, so stop
			last = betterAbove;
			betterAbove = betterAbove->parent(); // up one level
			} else
			break; // we're among the child of betterAbove, not after the last one
		}
		}
		// set as sibling
		after = above;
		parent = after ? after->parent() : 0L ;
	}

	RecipeDB *database;

protected slots:
	virtual void removeCategory(int id)=0;
	virtual void createCategory(const Element &category, int parent_id)=0;
	virtual void modifyCategory(const Element &category)=0;
	virtual void modifyCategory(int id, int parent_id)=0;

private:
	void loadListView(const CategoryTree *categoryTree, int parent_id=-1 );
};


class StdCategoryListView : public CategoryListView
{
Q_OBJECT

public:
	StdCategoryListView( QWidget *parent, RecipeDB *, bool editable=false );
	~StdCategoryListView();
	
protected:
	virtual void removeCategory(int id);
	virtual void createCategory(const Element &category, int parent_id);
	virtual void modifyCategory(const Element &category);
	virtual void modifyCategory(int id, int parent_id);

private slots:
	void preparePopup();
	void showPopup(KListView *, QListViewItem *, const QPoint &);

	void createNew();
	void remove();
	void rename();
	void cut();
	void paste();
	void pasteAsSub();

	void changeCategoryParent(QListViewItem *item,QListViewItem */*afterFirst*/,QListViewItem */*afterNow*/);
	
	void modCategory(QListViewItem* i);
	void saveCategory(QListViewItem* i);
	
private:
	KPopupMenu *kpop;
	QListViewItem *clipboard_item;
	QListViewItem *clipboard_parent;
};


class CategoryCheckListView : public CategoryListView
{
Q_OBJECT

public:
	CategoryCheckListView( QWidget *parent, RecipeDB * );

protected:
	virtual void removeCategory(int id);
	virtual void createCategory(const Element &category, int parent_id);
	virtual void modifyCategory(const Element &category);
	virtual void modifyCategory(int id, int parent_id);
};

#endif //CATEGORYLISTVIEW_H
