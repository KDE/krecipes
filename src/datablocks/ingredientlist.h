/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#ifndef INGREDIENTLIST_H
#define INGREDIENTLIST_H

#include <q3valuelist.h>
#include <QRegExp>

#include "datablocks/ingredient.h"

class RecipeDB;

/**
@author Unai Garro
*/
class IngredientList: public Q3ValueList <Ingredient>
{
public:
	IngredientList();
	~IngredientList();
	bool contains( const Ingredient &ing, bool compareAmount = false, RecipeDB *database = 0 ) const;
	bool containsSubSet( IngredientList &il, IngredientList &missing, bool compareAmount = false, RecipeDB *database = 0 );
	bool containsSubSet( IngredientList &il ) const;
	bool containsAny( const IngredientList &, bool compareAmount = false, RecipeDB *database = 0 );

	IngredientList groupMembers( int id, IngredientList::const_iterator begin ) const;

	void move( int index1, int index2 );
	void move( int index1, int count, int index2 );
	int find( int id ) const;
	Ingredient findByName( const QString & ) const;
	Ingredient findByName( const QRegExp & ) const;
	IngredientList::const_iterator find( IngredientList::const_iterator, int id ) const;
	IngredientList::iterator find( IngredientList::iterator, int id );

	/** Warning, returns an invalid reference if no ingredient is found.  Must check prior
	  * to calling this function if the ingredient exists.
	  */
	IngredientData& findSubstitute( const Ingredient & );
	void removeSubstitute( const Ingredient & );

	IngredientList firstGroup();
	IngredientList nextGroup();

private:
	Q3ValueList<IngredientList::const_iterator> _groupMembers( int id, IngredientList::const_iterator begin ) const;
	Q3ValueList<IngredientList::const_iterator> usedGroups;
};

#endif
