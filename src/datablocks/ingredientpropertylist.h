/***************************************************************************
*   Copyright (C) 2003 by Unai Garro                                      *
*   ugarro@users.sourceforge.net                                          *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#ifndef INGREDIENTPROPERTYLIST_H
#define INGREDIENTPROPERTYLIST_H

#include <qvaluelist.h>

#include "datablocks/ingredientproperty.h"

class IngredientPropertyList : public QValueList<IngredientProperty>
{
public:
	IngredientPropertyList();

	~IngredientPropertyList();

	void add( const IngredientProperty &element );
	IngredientProperty* at( int pos );
	void divide( int units_of_yield_type );

	int find( int id );
	int findByName( const QString & );
	void filter( int ingredientID, IngredientPropertyList *filteredList );
	IngredientProperty* getElement( int index );
	bool isEmpty( void );
	void remove( IngredientProperty* ip );

};

#endif
