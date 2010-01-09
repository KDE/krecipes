/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#ifndef SHOPPINGCALCULATOR_H
#define SHOPPINGCALCULATOR_H

class ElementList;
class IngredientList;
class RecipeDB;

/**
@author Unai Garro
*/
void calculateShopping( const ElementList &recipeList, IngredientList *ingredientList, RecipeDB *db );
void sum ( IngredientList *totalIngredientList, IngredientList *newIngredientList, RecipeDB *db );
int autoConvertUnits( RecipeDB* database, double amount1, int unit1, double amount2, int unit2, double &newAmount, int &newID );
#endif
