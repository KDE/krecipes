/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   mizunoami44@users.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
 
#include "categorytree.h" 
 
CategoryTree::CategoryTree() : m_children(new CategoryTreeChildren)
{
}

CategoryTree::~CategoryTree()
{
	delete m_children;
}

CategoryTreeChildren * CategoryTree::children() const
{ 
	return m_children;
}

CategoryTree *CategoryTree::add(const Element &cat)
{
	CategoryTree *new_child = new CategoryTree;
	new_child->category = cat;
	m_children->append( new_child );

	return new_child;
}

void CategoryTree::clear()
{
	//m_children->clear();
}
