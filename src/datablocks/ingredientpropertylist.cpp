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
	( IngredientProperty &property )
{
	list.append ( new IngredientProperty( property ) );
}

IngredientProperty* IngredientPropertyList::getFirst( void )
{
	return ( list.first() );
}

IngredientProperty* IngredientPropertyList::getNext( void )
{
	return ( list.next() );
}

IngredientProperty* IngredientPropertyList::getElement( int index )
{
	return ( list.at( index ) );
}

void IngredientPropertyList::clear( void )
{
	list.clear();
}

bool IngredientPropertyList::isEmpty( void )
{
	return ( list.isEmpty() );
}

int IngredientPropertyList::find( IngredientProperty* it )
{
	return ( list.find( it ) );
}

int IngredientPropertyList::find( int id )
{
	IngredientProperty ip;
	ip.id = id;
	return ( list.find( &ip ) );
}

int IngredientPropertyList::findByName( const QString &name )
{
	IngredientProperty * prop;
	for ( prop = list.first(); prop; prop = list.next() ) {
		if ( prop->name == name )
			return prop->id;
	}

	return -1;
}

IngredientProperty* IngredientPropertyList::at( int pos )
{
	return ( list.at( pos ) );
}

void IngredientPropertyList::append( IngredientProperty *property )
{
	list.append ( property );
}

void IngredientPropertyList::divide( int persons )
{
	for ( IngredientProperty * ip = getFirst();ip;ip = getNext() )
		ip->amount /= persons;
}

void IngredientPropertyList::filter( int ingredientID, IngredientPropertyList *filteredList )
{
	filteredList->clear();
	for ( IngredientProperty * ip = getFirst();ip;ip = getNext() )
		if ( ip->ingredientID == ingredientID )
			filteredList->add
			( *ip );

}

int IngredientPropertyList::count( void )
{
	return ( list.count() );
}

void IngredientPropertyList::remove
	( IngredientProperty* ip )
{
	list.remove( ip );
}
