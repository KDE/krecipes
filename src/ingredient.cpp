/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "ingredient.h"

Ingredient::Ingredient() : amount( 0 ), groupID( -1 )
{}
Ingredient::Ingredient( const Ingredient &i )
{
	amount = i.amount;
	ingredientID = i.ingredientID;
	name = i.name;
	prepMethodID = i.prepMethodID;
	prepMethod = i.prepMethod;
	unitID = i.unitID;
	units = i.units;
	groupID = i.groupID;
	group = i.group;
}

Ingredient::Ingredient( const QString &_name, double _amount, const Unit &_units, int _unitID, int _ingredientID, const QString &_prepMethod, int _prepMethodID ) :
		ingredientID( _ingredientID ),
		name( _name ),
		amount( _amount ),
		unitID( _unitID ),
		units( _units ),
		prepMethod( _prepMethod ),
		prepMethodID( _prepMethodID ),
		groupID( -1 )
{}

Ingredient::~Ingredient()
{}

//compare also using the group id because there may be the same ingredient in a list multiple times, but each in a different group
bool Ingredient::operator==( const Ingredient &ing ) const
{
	return ( ( ing.ingredientID == ingredientID ) && ( ing.groupID == groupID ) );
}

Ingredient & Ingredient::operator=( const Ingredient &i )
{
	amount = i.amount;
	ingredientID = i.ingredientID;
	name = i.name;
	prepMethodID = i.prepMethodID;
	prepMethod = i.prepMethod;
	unitID = i.unitID;
	units = i.units;
	groupID = i.groupID;
	group = i.group;
	return *this;
}

bool Ingredient::operator<( const Ingredient &ing ) const
{
	return ( QString::localeAwareCompare( name, ing.name ) < 0 );
}
