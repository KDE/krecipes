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
sum(ingredientList,&(rec.ingList),db);
}

}

/*void sum (IngredientList *totalIngredientList,IngredientList *newIngredientList)
{
for (Ingredient *i=newIngredientList->getFirst();i;i=newIngredientList->getNext())
{
totalIngredientList->add(*i);
}
}*/

void sum(IngredientList *totalIngredientList, IngredientList *newIngredientList, RecipeDB *db)
{

for (Ingredient *i=newIngredientList->getFirst();i;i=newIngredientList->getNext())
{
// Find out if ingredient exists in list already
	int pos=totalIngredientList->find(i->ingredientID);
	if (pos>=0) // the ingredient is already listed
		{

		// Variables to store the current values
		int currentUnit; double currentAmount;

		// Variables to store the new total
		int newUnit; double newAmount;
		int converted;

		// Do the conversion
		// try to with this and next in the list until conversion rate is
		// found or end of list is reached
		int lastpos; // for 'backup'
		do{
		lastpos=pos;
		// Get current Values

		currentUnit=totalIngredientList->at(pos)->unitID;
		currentAmount=totalIngredientList->at(pos)->amount;
			totalIngredientList->getNext(); // Otherwise findNext() gets in closed loop...

		// Try to convert
		converted =autoConvertUnits(db,i->amount,i->unitID,currentAmount,currentUnit,newAmount,newUnit);
		} while ( (converted<0) && (((pos=totalIngredientList->findNext(i->ingredientID)))>0) );

		// If the conversion was succesful, Set the New Values
		if (converted>=0)
		{
		Ingredient *it; it=totalIngredientList->at(lastpos);
		it->amount=newAmount;
		it->unitID=newUnit;
		it->units=db->unitName(newUnit);
		}
		else // Otherwise append this ingredient at the end of the list
		{
		// Insert ingredient ID in the list
		totalIngredientList->add(*i);
		}
	}

	else // The ingredient is not in the list, just append
	{
	totalIngredientList->add(*i);
	}
}

}


int autoConvertUnits(RecipeDB* database,double amount1,int unit1,double amount2,int unit2, double &newAmount, int &newID)
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

