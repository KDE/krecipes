/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
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
	CategoryTree();
	~CategoryTree();

	Element category;

	CategoryTreeChildren *children() const;
	CategoryTree *add(const Element &cat);
	void clear();

private:
	CategoryTreeChildren *m_children;
};

#endif
