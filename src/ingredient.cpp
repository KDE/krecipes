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

Ingredient::Ingredient()
{
}

Ingredient::Ingredient( const QString &_name, double _amount, const QString &_units, int _unitID, int _ingredientID ) :
  ingredientID(_ingredientID),
  name(_name),
  amount(_amount),
  unitID(_unitID),
  units(_units)
{
}

Ingredient::Ingredient(const Ingredient &ing)
{
this->ingredientID=ing.ingredientID;
this->name=ing.name;
this->amount=ing.amount;
this->unitID=ing.unitID;
this->units=ing.units;

}

Ingredient::~Ingredient()
{
}

Ingredient& Ingredient::operator=(const Ingredient &ing)
{
ingredientID=ing.ingredientID;
amount=ing.amount;
name=ing.name;
unitID=ing.unitID;
units=ing.units;
return *this;
}

bool Ingredient::operator==(const Ingredient &ing)
{
	return (ing.ingredientID == this->ingredientID);
}
