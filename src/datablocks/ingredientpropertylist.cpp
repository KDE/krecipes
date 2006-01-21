/***************************************************************************
*   Copyright (C) 2003 by Unai Garro                                      *
*   ugarro@users.sourceforge.net                                          *
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

void IngredientPropertyList::add
	( const IngredientProperty &property )
{
	append ( property );
}


IngredientProperty* IngredientPropertyList::getElement( int index )
{
	return ( at( index ) );
}

bool IngredientPropertyList::isEmpty( void )
{
	return ( isEmpty() );
}

int IngredientPropertyList::find( int id )
{
	IngredientProperty ip;
	ip.id = id;
	IngredientPropertyList::const_iterator find_it = QValueList<IngredientProperty>::find( ip );
	if ( find_it != end() )
		return (*find_it).id;
	else
		return -1;
}

int IngredientPropertyList::findByName( const QString &name )
{
	IngredientPropertyList::const_iterator prop_it;
	for ( prop_it = begin(); prop_it != end(); ++prop_it ) {
		if ( (*prop_it).name == name )
			return (*prop_it).id;
	}

	return -1;
}

IngredientProperty* IngredientPropertyList::at( int pos )
{
	return ( at( pos ) );
}

void IngredientPropertyList::divide( int units_of_yield_type )
{
	IngredientPropertyList::iterator prop_it;
	for ( prop_it = begin(); prop_it != end(); ++prop_it )
		(*prop_it).amount /= units_of_yield_type;
}

void IngredientPropertyList::filter( int ingredientID, IngredientPropertyList *filteredList )
{
	filteredList->clear();
	IngredientPropertyList::const_iterator prop_it;
	for ( prop_it = begin(); prop_it != end(); ++prop_it ) {
		if ( (*prop_it).ingredientID == ingredientID )
			filteredList->add( *prop_it );
	}
}

void IngredientPropertyList::remove
	( IngredientProperty* ip )
{
	remove( ip );
}
