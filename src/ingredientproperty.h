/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef INGREDIENTPROPERTY_H
#define INGREDIENTPROPERTY_H

#include <qstring.h>
#include "element.h"

/**
@author Unai Garro
*/
class IngredientProperty{
public:
    IngredientProperty();
    IngredientProperty(IngredientProperty &ip);
    ~IngredientProperty();
    int id; // The property's id
    int ingredientID; // (Optional) reference to the ingredient to which is attached
    QString name; // Name of the property
    QString units; // The units that the property uses
    Element perUnit; // stores the unit ID and Name of the per units.
    double amount; // Stores the amount, in the case of being attached to an ingredient. If not attached to any, you can set it to -1 preferably. That's the case in which the property is treated as a characteristic any without value (amount).
    IngredientProperty& operator=(IngredientProperty &ip);


};

#endif
