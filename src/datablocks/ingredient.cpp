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

#include "datablocks/ingredient.h"

#include <qstringlist.h>

#include "mixednumber.h"

Ingredient::Ingredient() : amount( 0 ), amount_offset( 0 ), groupID( -1 )
{}

Ingredient::Ingredient( const QString &_name, double _amount, const Unit &_units, int _unitID, int _ingredientID ) :
		ingredientID( _ingredientID ),
		name( _name ),
		amount( _amount ),
		amount_offset( 0 ),
		unitID( _unitID ),
		units( _units ),
		groupID( -1 )
{}

Ingredient::~Ingredient()
{}

void Ingredient::setAmount( const QString &range, bool *ok )
{
	QStringList ranges = QStringList::split('-',range);

	QString amount_min = ranges[0];
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
