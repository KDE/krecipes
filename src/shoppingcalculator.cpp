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

#include <kdebug.h>

#include "elementlist.h"
#include "ingredientlist.h"
#include "DBBackend/recipedb.h"

void calculateShopping(const ElementList &recipeList,IngredientList *ingredientList, RecipeDB *db)
{

for ( ElementList::const_iterator recipe_it = recipeList.begin(); recipe_it != recipeList.end(); ++recipe_it )
{
Recipe rec;
db->loadRecipe(&rec,(*recipe_it).id);
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
for ( IngredientList::const_iterator ing_it = newIngredientList->begin(); ing_it != newIngredientList->end(); ++ing_it )
{
	IngredientList::iterator pos_it;

// Find out if ingredient exists in list already
	int pos=totalIngredientList->find((*ing_it).ingredientID);

	if (pos>=0) // the ingredient is already listed
	{
		pos_it = totalIngredientList->at(pos);

		// Variables to store the current values
		int currentUnit; double currentAmount;

		// Variables to store the new total
		int newUnit; double newAmount;
		int converted;

		// Do the conversion
		// try to with this and next in the list until conversion rate is
		// found or end of list is reached
		IngredientList::iterator lastpos_it; // for 'backup'
		do{
		lastpos_it=pos_it;
		// Get current Values

		currentUnit=(*pos_it).unitID;
		currentAmount=(*pos_it).amount;

		// Try to convert
		converted =autoConvertUnits(db,(*ing_it).amount,(*ing_it).unitID,currentAmount,currentUnit,newAmount,newUnit);
		} while ( (converted<0) && (((pos_it=totalIngredientList->find(pos_it,(*ing_it).ingredientID)))!=totalIngredientList->end()) );

		// If the conversion was succesful, Set the New Values
		if (converted>=0)
		{
			(*lastpos_it).amount=newAmount;
			(*lastpos_it).unitID=newUnit;
			(*lastpos_it).units=db->unitName(newUnit);
		}
		else // Otherwise append this ingredient at the end of the list
		{
			// Insert ingredient ID in the list
			totalIngredientList->append(*ing_it);
		}
	}
	else // The ingredient is not in the list, just append
	{
		totalIngredientList->append(*ing_it);
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

