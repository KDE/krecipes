/***************************************************************************
 *   Copyright (C) 2004 by Jason Kivlighn                                  *
 *   mizunoami44@users.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
 
#include "categorytree.h" 
#include <kdebug.h>
CategoryTree::CategoryTree( CategoryTree *parent ) : m_children(new CategoryTreeChildren),
  m_parent(0), m_child(0), m_sibling(0)
{
	if ( parent )
		parent->insertItem(this);
}

CategoryTree::~CategoryTree()
{
	if ( m_parent )
		m_parent->takeItem( this );

	//for ( CategoryTreeChildren::iterator child_it = m_children->begin(); child_it != m_children->end(); ++child_it ) {
	//	delete *child_it;
	//}

	delete m_children;
}

const CategoryTreeChildren * CategoryTree::children() const
{
	return m_children;
}

CategoryTree *CategoryTree::add(const Element &cat)
{
	CategoryTree *new_child = new CategoryTree(this);
	new_child->category = cat;
	m_children->append( new_child );

	return new_child;
}

void CategoryTree::insertItem( CategoryTree *newChild )
{
	newChild->m_sibling = m_child;
	m_child = newChild;
	newChild->m_parent = this;
}

void CategoryTree::takeItem( CategoryTree *tree )
{
	for ( CategoryTreeChildren::iterator child_it = m_children->begin(); child_it != m_children->end(); ++child_it ) {
		if ( (*child_it) == tree ) {
			m_children->remove( child_it );
			break;
		}
	}

	/* FIXME
	CategoryTree ** nextChild = &m_child;
	while( nextChild && *nextChild && tree != *nextChild )
		nextChild = &((*nextChild)->m_sibling);
	
	if ( nextChild && tree == *nextChild )
		*nextChild = (*nextChild)->m_sibling;
	tree->m_parent = 0;
	tree->m_sibling = 0;
	*/
}

void CategoryTree::clear()
{
	//m_children->clear();
}

bool CategoryTree::contains( int id ) const
{
	bool result = false;

	for ( CategoryTreeChildren::const_iterator child_it = m_children->begin(); child_it != m_children->end(); ++child_it )
	{
		const CategoryTree *node = *child_it;
		
		if ( node->category.id == id ) 
			return true;

		result = node->contains(id);
	}

	return result;
}

