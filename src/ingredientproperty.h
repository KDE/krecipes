/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
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

    ~IngredientProperty();
    int id;
    QString name;
    QString units;
    Element perUnit; // stores the unit ID and Name
    double amount; // Stores the amount, in the case of being attached to an ingredient, otherwise you can set it to -1 preferably.



};

#endif
