/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*                                                                         *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#ifndef INGREDIENTLIST_H
#define INGREDIENTLIST_H

#include <qvaluelist.h>

#include "ingredient.h"

/**
@author Unai Garro
*/
class IngredientList: public QValueList <Ingredient>
{
public:
	IngredientList();
	~IngredientList();
	bool contains( const Ingredient &ing ) const;
	bool containsSubSet( IngredientList &il, IngredientList &missing );
	bool containsAny( const IngredientList & );

	IngredientList groupMembers( int id, IngredientList::const_iterator begin ) const;

	void move( int index1, int index2 );
	void move( int index1, int count, int index2 );
	void empty( void );
	int find( int id ) const;
	IngredientList::const_iterator find( IngredientList::const_iterator, int id ) const;
	IngredientList::iterator find( IngredientList::iterator, int id );

	IngredientList firstGroup();
	IngredientList nextGroup();

private:
	QValueList<IngredientList::const_iterator> _groupMembers( int id, IngredientList::const_iterator begin ) const;
	QValueList<IngredientList::const_iterator> usedGroups;
};

#endif
