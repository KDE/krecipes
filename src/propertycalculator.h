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

int autoConvert( RecipeDB *database, double amount1, int unit1, double amount2, int unit2, double &newAmount, int &newID );
void checkUndefined( IngredientPropertyList *recipePropertyList, IngredientPropertyList &addedPropertyList );

/*
** Version with database I/O. DB must be provided
*/

void calculateProperties( const Recipe& recipe, RecipeDB* database, IngredientPropertyList *recipePropertyList );
void addPropertyToList( RecipeDB *database, IngredientPropertyList *recipePropertyList, IngredientPropertyList &ingPropertyList, const Ingredient &ing, int ingredientNo );

/*
** Version with no database I/O. necessary DB data must be provided. Useful for caching data
*/
void calculateProperties( const Recipe& recipe, IngredientPropertyList& ipl, UnitRatioList& url, IngredientPropertyList *recipePropertyList );
void addPropertyToList( IngredientPropertyList *recipePropertyList, IngredientPropertyList &newProperties, const Ingredient &ing, UnitRatioList &url, int ingredientNo );

#endif //PROPERTYCALCULATOR_H
