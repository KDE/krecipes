/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "shoppingcalculator.h"
void calculateShopping(ElementList *recipeList,IngredientList *ingredientList, RecipeDB *db)
{

for (Element *recipe=recipeList->getFirst();recipe; recipe=recipeList->getNext())
{
Recipe rec;
db->loadRecipe(&rec,recipe->id);
sum(ingredientList,&(rec.ingList));
}

}

void sum (IngredientList *totalIngredientList,IngredientList *newIngredientList)
{
for (Ingredient *i=newIngredientList->getFirst();i;i=newIngredientList->getNext())
{
totalIngredientList->add(*i);
}
}




