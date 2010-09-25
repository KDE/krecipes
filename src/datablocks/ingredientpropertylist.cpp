/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "datablocks/ingredientpropertylist.h"

IngredientPropertyList::IngredientPropertyList()
{}


IngredientPropertyList::~IngredientPropertyList()
{}

IngredientPropertyList::iterator IngredientPropertyList::find( int id )
{
	IngredientProperty ip;
	ip.id = id;
	return QList<IngredientProperty>::find( ip );
}

int IngredientPropertyList::findByName( const QString &name )
{
	IngredientPropertyList::const_iterator prop_it;
	for ( prop_it = constBegin(); prop_it != constEnd(); ++prop_it ) {
		if ( (*prop_it).name == name )
			return (*prop_it).id;
	}

	return -1;
}

void IngredientPropertyList::divide( double units_of_yield_type )
{
	IngredientPropertyList::iterator prop_it;
	for ( prop_it = begin(); prop_it != end(); ++prop_it )
		(*prop_it).amount /= units_of_yield_type;
}

void IngredientPropertyList::filter( int ingredientID, IngredientPropertyList *filteredList )
{
	filteredList->clear();
	IngredientPropertyList::const_iterator prop_it;
	for ( prop_it = constBegin(); prop_it != constEnd(); ++prop_it ) {
		if ( (*prop_it).ingredientID == ingredientID )
			filteredList->append( *prop_it );
	}
}
