/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "datablocks/ingredient.h"

#include <QStringList>

#include "mixednumber.h"

IngredientData::IngredientData() : amount( 0 ), amount_offset( 0 ), groupID( -1 )
{}

IngredientData::IngredientData( const QString &_name, double _amount, const Unit &_units, int _unitID, int _ingredientID ) :
		ingredientID( _ingredientID ),
		name( _name ),
		amount( _amount ),
		amount_offset( 0 ),
		units( _units ),
		groupID( -1 )
{
units.id=_unitID;
}

//compare also using the group id because there may be the same ingredient in a list multiple times, but each in a different group
bool IngredientData::operator==( const IngredientData &ing ) const
{
	return ( ( ing.ingredientID == ingredientID ) && ( ing.groupID == groupID ) );
}

Ingredient::Ingredient() : IngredientData()
{}

Ingredient::Ingredient( const QString &_name, double _amount, const Unit &_units, int _unitID, int _ingredientID ) :
	IngredientData(_name,_amount,_units,_unitID,_ingredientID)
{}

Ingredient::Ingredient( const IngredientData &d ) : IngredientData(d)
{}

void Ingredient::setAmount( const QString &range, bool *ok )
{
	if ( range.isEmpty() ) {
		if ( ok ) *ok = true;
 		return;
	}

	QStringList ranges = range.QString::split('-');

	QString amount_min = ranges[0];
	if ( amount_min.isEmpty() ) {
		if ( ok ) *ok = false;
		return;
	}

	QString amount_max;
	switch ( ranges.count() ) {
		case 0:
		case 1: break;
		case 2: amount_max = ranges[1];
			break;
		default:
			if ( ok ) *ok = false;
			return;
	}

	MixedNumber nm_min = MixedNumber::fromString( amount_min, ok );
	if ( ok && *ok == false ) return;

	MixedNumber nm_max = MixedNumber::fromString( amount_max, ok );
	if ( ok && *ok == false ) return;

	amount = nm_min.toDouble();
	if ( nm_max > 0 )
		amount_offset = nm_max.toDouble() - amount;
}

//compare also using the group id because there may be the same ingredient in a list multiple times, but each in a different group
bool Ingredient::operator==( const Ingredient &ing ) const
{
	return ( ( ing.ingredientID == ingredientID ) && ( ing.groupID == groupID ) );
}

bool Ingredient::operator<( const Ingredient &ing ) const
{
	return ( QString::localeAwareCompare( name, ing.name ) < 0 );
}
