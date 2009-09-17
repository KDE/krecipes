/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#ifndef CATEGORYTREE_H
#define CATEGORYTREE_H

#include "element.h"

class CategoryTree
{
public:
	CategoryTree( CategoryTree *parent = 0 );

	~CategoryTree();

	Element category;

	CategoryTree *add
	( const Element &cat );
	void clear();

	bool contains( int id ) const;
	CategoryTree* find( int id ) const;

	CategoryTree *parent() const
	{
		return m_parent;
	}
	CategoryTree *firstChild() const
	{
		return m_child;
	}
	CategoryTree *nextSibling() const
	{
		return m_sibling;
	}

	void takeItem( CategoryTree * );
	void insertItem( CategoryTree * );

	int count() const { return m_count; }

private:
	CategoryTree( const CategoryTree & );
	CategoryTree &operator=( const CategoryTree & );

	CategoryTree *m_parent;
	CategoryTree *m_child;
	CategoryTree *m_sibling;
	CategoryTree *m_last;

	int m_count;
};

#endif
