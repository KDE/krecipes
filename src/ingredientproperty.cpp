/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "ingredientproperty.h"

IngredientProperty::IngredientProperty()
{
id=-1;
amount=0.0;
}

IngredientProperty::IngredientProperty(IngredientProperty *ip)
{
amount=ip->amount;
id=ip->id;
name=ip->name;
perUnit=ip->perUnit;
units=ip->units;
}

IngredientProperty::~IngredientProperty()
{
}

