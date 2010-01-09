/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#ifndef INGREDIENTPROPERTY_H
#define INGREDIENTPROPERTY_H

#include <QString>

#include "unit.h"

/**
@author Unai Garro
*/
class IngredientProperty
{
public:
	IngredientProperty();
	IngredientProperty( const QString &name, const QString &units, int id = -1 );
	~IngredientProperty();
	int id; // The property's id
	int ingredientID; // (Optional) reference to the ingredient to which is attached
	QString name; // Name of the property
	QString units; // The units that the property uses
	Unit perUnit; // stores the unit ID, name, and type of the per units.
	double amount; // Stores the amount, in the case of being attached to an ingredient. If not attached to any, you can set it to -1 preferably. That's the case in which the property is treated as a characteristic any without value (amount).

	/** Compare two elements by their id */
	bool operator==( const IngredientProperty & ) const;
};

#endif
