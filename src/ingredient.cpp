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

Ingredient::Ingredient() : amount(0)
{
}
Ingredient::Ingredient(const Ingredient &i)
    	{
	amount=i.amount;
	ingredientID=i.ingredientID;
	name=i.name;
	prepMethodID=i.prepMethodID;
	prepMethod=i.prepMethod;
	unitID=i.unitID;
	units=i.units;
	}
	
Ingredient::Ingredient( const QString &_name, double _amount, const QString &_units, int _unitID, int _ingredientID, const QString &_prepMethod, int _prepMethodID ) :
  ingredientID(_ingredientID),
  name(_name),
  amount(_amount),
  unitID(_unitID),
  units(_units),
  prepMethod(_prepMethod),
  prepMethodID(_prepMethodID)
{
}

Ingredient::~Ingredient()
{
}

bool Ingredient::operator==(const Ingredient &ing) const
{
	return (ing.ingredientID == this->ingredientID);
}

Ingredient & Ingredient::operator=(const Ingredient &i)
    	{
    	amount=i.amount;
	ingredientID=i.ingredientID;
	name=i.name;
	prepMethodID=i.prepMethodID;
	prepMethod=i.prepMethod;
	unitID=i.unitID;
	units=i.units;
	return *this;
	}

bool Ingredient::operator<(const Ingredient &ing) const
{
	return(QString::localeAwareCompare(name,ing.name) < 0);
}
