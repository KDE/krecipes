/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "propertycalculator.h"

#include <kdebug.h>

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

/*
** Version with database I/O. DB must be provided
*/

void calculateProperties(const Recipe& recipe,RecipeDB* database,IngredientPropertyList *recipePropertyList)
{
recipePropertyList->clear();
// Note that recipePropertyList is not attached to any ingredient. It's just the total of the recipe
IngredientPropertyList ingredientPropertyList; // property list for each ingredient

int ingredientNo=1;

QPtrListIterator<Ingredient> ing_it( recipe.ingList );
Ingredient *ing;
while ( (ing = ing_it.current()) != 0 )
	{
	++ing_it;

	database->loadProperties(&ingredientPropertyList,ing->ingredientID);
	ingredientPropertyList.divide(recipe.persons); // calculates properties per person
	addPropertyToList(database,recipePropertyList,ingredientPropertyList,*ing,ingredientNo);
	ingredientNo++;
	}
}


void addPropertyToList(RecipeDB *database,IngredientPropertyList *recipePropertyList,IngredientPropertyList &ingPropertyList,Ingredient &ing,int ingredientNo)
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
		if (property->amount>=0) property->amount+=(prop->amount)*(ing.amount)*ratio; //Normal case
		else property->amount-=(prop->amount)*(ing.amount)*ratio; //The recipe was marked as undefined previously. Keep it negative
		}
	else { // Could not convert units
	     kdDebug()<<"\nWarning: I could not calculate the full property list, due to impossible unit conversion\n";
	     }

	}
	else // Append new property
		{

		// We are about to add a new property. Were there previous ingredients that didn't have this defined?
		bool undefined=(ingredientNo>1);  // If 1, it's the first ingredient else, it's the second or more
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
			if (undefined) property.amount=-(abs(property.amount));
			recipePropertyList->add(property);
			}
		else { // Could not convert units
		kdDebug()<<"\nWarning: I could not calculate the full property list, due to impossible unit conversion\n";
		}



		}

	}

     // The previous procedure doesn't take into account if an appended ingredient has not defined a property.
     // Mark with negative sign all those properties in the recipe properties, since they're undefined
     checkUndefined(recipePropertyList,ingPropertyList);
}

/*
** Version with no database I/O. necessary DB data must be provided. Useful for caching data
*/


void calculateProperties(const Recipe& recipe,IngredientPropertyList& ipl,UnitRatioList& url, IngredientPropertyList *recipePropertyList)
{
recipePropertyList->clear();

IngredientPropertyList filteredPropertyList;

int ingredientNo=1;
QPtrListIterator<Ingredient> ing_it( recipe.ingList );
Ingredient *ing;
while ( (ing = ing_it.current()) != 0 )
{
	ipl.filter(ing->ingredientID,&filteredPropertyList); // Get the properties for the respective ingredient
	filteredPropertyList.divide(recipe.persons); // calculates properties per person
	addPropertyToList(recipePropertyList,filteredPropertyList,*ing,url,ingredientNo);
	ingredientNo++;
	}
}

void addPropertyToList(IngredientPropertyList *recipePropertyList,IngredientPropertyList &newProperties,Ingredient &ing,UnitRatioList &url,int ingredientNo)
{
for (IngredientProperty *prop=newProperties.getFirst();prop;prop=newProperties.getNext())
	{
	// Find if property was listed before
	int pos=recipePropertyList->find(prop);
	if (pos>=0) //The property exists in the list. Add to it
	{
	IngredientProperty *property=recipePropertyList->at(pos);
	double ratio; ratio=url.getRatio(ing.unitID, prop->perUnit.id);

	if (ratio>0.0) // Could convert units to perUnit
		{
		if (prop->amount>=0) property->amount+=(prop->amount)*(ing.amount)*ratio; //Normal case
		else property->amount-=(prop->amount)*(ing.amount)*ratio; //The recipe was marked as undefined previously. Keep it negative
		}
	else { // Could not convert units
	     kdDebug()<<"\nWarning: I could not calculate the full property list, due to impossible unit conversion";
	     }

	}
	else // The property doesn't exist in the list. Append new property
	{

	// We are about to add a new property. Were there previous ingredients that didn't have this defined?
	bool undefined=(ingredientNo>1);  // If 1, it's the first ingredient else, it's the second or more

	IngredientProperty property;
	property.id=prop->id;
	property.name=prop->name;
	property.perUnit.id=-1; // It's not per unit, it's total sum of the recipe
	property.perUnit.name=QString::null; // "
	property.units=prop->units;

	double ratio; ratio=url.getRatio(ing.unitID, prop->perUnit.id);

	if (ratio>0.0) // Could convert units to perUnit
		{
		property.amount=(prop->amount)*(ing.amount)*ratio;
		if (undefined) property.amount=-(abs(property.amount));
		recipePropertyList->add(property);
		}
	else { // Could not convert units
	     kdDebug()<<"\nWarning: I could not calculate the full property list, due to impossible unit conversion\n";
	     }
	}

	}

     // The previous procedure doesn't take into account if an appended ingredient has not defined a property.
     // Mark with negative sign all those properties in the recipe properties, since they're undefined
     checkUndefined(recipePropertyList,newProperties);

}


void checkUndefined(IngredientPropertyList *recipePropertyList,IngredientPropertyList &addedPropertyList)
{
for (IngredientProperty *prop=recipePropertyList->getFirst();prop;prop=recipePropertyList->getNext())
	{
	int pos=addedPropertyList.find(prop);
	if (pos<0) prop->amount=-(abs(prop->amount)); // undefined
	}
}


