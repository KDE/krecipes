/***************************************************************************
 *   Copyright (C) 2004 by Jason Kivlighn                                  *
 *   mizunoami44@users.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef CATEGORYTREE_H
#define CATEGORYTREE_H

#include <qvaluelist.h>

#include "element.h"

class CategoryTree;
typedef QValueList<CategoryTree*> CategoryTreeChildren;

class CategoryTree
{
public:
	CategoryTree( CategoryTree *parent=0 );

	~CategoryTree();

	Element category;

	const CategoryTreeChildren *children() const;
	CategoryTree *add(const Element &cat);
	void clear();

	bool contains( int id ) const;
	
	CategoryTree *parent() { return m_parent; }
	CategoryTree *firstChild() { return m_child; }
	CategoryTree *nextSibling() { return m_sibling; }
	
	void takeItem( CategoryTree * );
	void insertItem( CategoryTree * );

private:
	CategoryTree( const CategoryTree & );
	CategoryTree &operator=( const CategoryTree & );

	CategoryTreeChildren *m_children;

	CategoryTree *m_parent;
	CategoryTree *m_child;
	CategoryTree *m_sibling;
};

#endif
