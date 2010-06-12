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

#include "datablocks/ingredient.h"

#include <QStringList>
#include <KGlobal>
#include <KLocale>

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
    units.setId(_unitID);
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
	amount_max = amount_max.trimmed();

	KLocale * locale = KGlobal::locale();
	double min, max=0;

	if ( MixedNumber::isFraction( amount_min ) ) {
		MixedNumber mixed_min = MixedNumber::fromString( amount_min, ok );
		if ( ok && *ok == false ) return;
		min = mixed_min.toDouble();
	} else {
		min = locale->readNumber( amount_min, ok );
		if ( ok && *ok == false ) return;
	}

	if ( !amount_max.isEmpty() ) {
		if ( MixedNumber::isFraction( amount_max ) ) {
			MixedNumber mixed_max = MixedNumber::fromString( amount_max, ok );
			if ( ok && *ok == false ) return;
			max = mixed_max.toDouble();
		} else {
			max = locale->readNumber( amount_max, ok );
			if ( ok && *ok == false ) return;
		}
	}

	amount = min;
	if ( max > 0 )
		amount_offset = max - min;
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
