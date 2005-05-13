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

#include "element.h"

CategoryTree::CategoryTree( CategoryTree *parent ) :
		m_parent( 0 ), m_child( 0 ), m_sibling( 0 ), m_last(0)
{
	if ( parent )
		parent->insertItem( this );
}

CategoryTree::~CategoryTree()
{
	if ( m_parent )
		m_parent->takeItem( this );

	CategoryTree * i = m_child;
	m_child = 0;
	while ( i ) {
		i->m_parent = 0;
		CategoryTree * n = i->m_sibling;
		delete i;
		i = n;
	}
}

CategoryTree *CategoryTree::add
	( const Element &cat )
{
	CategoryTree * new_child = new CategoryTree( this );
	new_child->category = cat;
	
	return new_child;
}

void CategoryTree::insertItem( CategoryTree *newChild )
{
	newChild->m_parent = this;
	if ( m_child && m_child->m_last )
		m_child->m_last->m_sibling = newChild;
	else
		m_child = newChild;

	m_child->m_last = newChild;
}

void CategoryTree::takeItem( CategoryTree *tree )
{
	//FIXME: Both these methods seem to be broken... don't use this function!

	CategoryTree *lastItem = m_child->m_last;
#if 0
	CategoryTree ** nextChild = &m_child;
	while( nextChild && *nextChild && tree != *nextChild )
		nextChild = &((*nextChild)->m_sibling);

	if ( nextChild && tree == *nextChild ) {
		*nextChild = (*nextChild)->m_sibling;
	}
	tree->m_parent = 0;
	tree->m_sibling = 0;
#else
	for ( CategoryTree *it = firstChild(); it; it = it->nextSibling() ) {
		if ( it->nextSibling() == tree ) {
			it->m_sibling = tree->nextSibling();
			break;
		}
	}
	tree->m_parent = 0;
	tree->m_sibling = 0;
#endif
	if ( tree != m_last )
		m_child->m_last = lastItem;
	else //FIXME: unstable behavior if this is the case
		kdDebug()<<"CategoryTree::takeItem: warning - unstable behavior expected"<<endl;
}

void CategoryTree::clear()
{
	CategoryTree *c = m_child;
	CategoryTree *n;
	while( c ) {
		n = c->m_sibling;
		delete c;
		c = n;
	}
}

bool CategoryTree::contains( int id ) const
{
	bool result = false;

	for ( CategoryTree * child_it = firstChild(); child_it; child_it = child_it->nextSibling() ) {
		if ( child_it->category.id == id )
			return true;

		result = child_it->contains( id );
	}

	return result;
}


