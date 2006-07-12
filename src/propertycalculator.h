/***************************************************************************
*   Copyright (C) 2003 by Unai Garro                                      *
*   ugarro@users.sourceforge.net                                          *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef PROPERTYCALCULATOR_H
#define PROPERTYCALCULATOR_H

#include <qptrlist.h>
#include <backends/recipedb.h>
#include "datablocks/elementlist.h"
#include "datablocks/ingredientpropertylist.h"
#include "datablocks/recipe.h"

bool autoConvert( RecipeDB* database, const Ingredient &from, const Unit &to, Ingredient &result );
void checkUndefined( IngredientPropertyList *recipePropertyList, IngredientPropertyList &addedPropertyList );
void calculateProperties( Recipe& recipe, RecipeDB* database );
void addPropertyToList( RecipeDB *database, IngredientPropertyList *recipePropertyList, IngredientPropertyList &ingPropertyList, const Ingredient &ing, int ingredientNo );

#endif //PROPERTYCALCULATOR_H
