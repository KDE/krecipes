/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include <iostream>
#include "propertycalculator.h"



int autoConvert(RecipeDB* database,double amount1,int unit1,double amount2,int unit2, double &newAmount, int &newID)
{


double ratio=database->unitRatio(unit1,unit2);
if (ratio>=0) // There is a ratio
	{
	if (ratio>=1) // Convert to unit 1, since unit1 is bigger
		{
		newID=unit1;
		newAmount=amount1+amount2/ratio;
		}
	else // Convert to unit2, since unit2 is bigger
		{
		newID=unit2;
		newAmount=amount1*ratio+amount2;
		}
	return(0);
	}
else
	{
	newAmount=-1;
	newID=-1;
	return(-1);
	}

}


void calculateProperties(Recipe& recipe,RecipeDB* database,IngredientPropertyList *recipePropertyList)
{
// Note that recipePropertyList is not attached to any ingredient. It's just the total of the recipe
IngredientPropertyList ingredientPropertyList; // property list for each ingredient

for (Ingredient *ing=recipe.ingList.getFirst();ing;ing=recipe.ingList.getNext())
	{
	database->loadProperties(&ingredientPropertyList,ing->ingredientID);
	addPropertyToList(database,recipePropertyList,ingredientPropertyList,*ing);
	}
}

void addPropertyToList(RecipeDB *database,IngredientPropertyList *recipePropertyList,IngredientPropertyList &ingPropertyList,Ingredient &ing)
{
for (IngredientProperty *prop=ingPropertyList.getFirst();prop;prop=ingPropertyList.getNext())
	{
	// Find if property was listed before
	int pos=recipePropertyList->find(prop);
	if (pos>=0) //Exists. Add to it
	{
	IngredientProperty *property=recipePropertyList->at(pos);
	double ratio; ratio=database->unitRatio(ing.unitID, prop->perUnit.id);

	if (ratio>0.0) // Could convert units to perUnit
		{
		property->amount+=(prop->amount)*(ing.amount)*ratio;
		std::cerr<<(prop->amount)<<";"<<(ing.amount)<<";"<<ratio<<"\n";
		}
	else { // Could not convert units
	     std::cerr<<"\nWarning: I could not calculate the full property list, due to impossible unit conversion\n";
	     }

	}
	else // Append new property
	{
	IngredientProperty property;
	property.id=prop->id;
	property.name=prop->name;
	property.perUnit.id=-1; // It's not per unit, it's total sum of the recipe
	property.perUnit.name=QString::null; // "
	property.units=prop->units;

	double ratio; ratio=database->unitRatio(ing.unitID, prop->perUnit.id);

	if (ratio>0.0) // Could convert units to perUnit
		{
		property.amount=(prop->amount)*(ing.amount)*ratio;
		std::cerr<<(prop->amount)<<";"<<(ing.amount)<<";"<<ratio<<"\n";
		recipePropertyList->add(property);
		}
	else { // Could not convert units
	     std::cerr<<"\nWarning: I could not calculate the full property list, due to impossible unit conversion\n";
	     }




	}

	}
}
