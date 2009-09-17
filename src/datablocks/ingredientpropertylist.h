/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@users.sourceforge.net>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#ifndef INGREDIENTPROPERTYLIST_H
#define INGREDIENTPROPERTYLIST_H

#include <q3valuelist.h>

#include "datablocks/ingredientproperty.h"

class IngredientPropertyList : public Q3ValueList<IngredientProperty>
{
public:
	IngredientPropertyList();

	~IngredientPropertyList();

	void divide( double units_of_yield_type );
	IngredientPropertyList::const_iterator find( int id );
	int findByName( const QString & );
	void filter( int ingredientID, IngredientPropertyList *filteredList );
};

#endif
