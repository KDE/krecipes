/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                                       *
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

Ingredient::Ingredient(Ingredient &ing)
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


