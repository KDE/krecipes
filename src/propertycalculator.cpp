/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include <iostream>
#include "propertycalculator.h"


void calculateIngredients(RecipeDB* database,Recipe *recipe,IDList &ingList, QPtrList <int> &unitList, QPtrList <double> &amountList)
{
Ingredient *ing;

	for (ing=recipe->ingList.getFirst();ing; ing=recipe->ingList.getNext())
	{
	// Find out if ingredient exists in list already
	int pos=ingList.find(&(ing->ingredientID));
	if (pos>=0) // the ingredient is already listed
		{

		// Variables to store the current values
		int *currentUnit; double * currentAmount;

		// Variables to store the new total
		int newUnit; double newAmount;
		int converted;

		// Do the conversion
		// try to with this and next in the list until conversion rate is
		// found or end of list is reached
		do{
		// Get current Values
		currentUnit=unitList.at(pos);
		currentAmount=amountList.at(pos);
		// Try to convert
		converted =autoConvert(database,ing->amount,ing->unitID,*currentAmount,*currentUnit,newAmount,newUnit);
		} while ( (converted<0) && ((pos=ingList.findNext(&(ing->ingredientID)))>0) );

		// If the conversion was succesful, Set the New Values
		if (converted>=0)
		{
		*currentAmount=newAmount;
		*currentUnit=newUnit;
		}
		else // Otherwise append this ingredient at the end of the list
		{
		// Insert ingredient ID in the list
		int *ingID=new int; *ingID=ing->ingredientID;
		ingList.append(ingID);

		// Set values for this ingredient
		int *unitID=new int; *unitID=ing->unitID;
		unitList.append(unitID);
		double *amount=new double; *amount=ing->amount;
		amountList.append(amount);

		}
		}
	else
		{

		// Insert ingredient ID in the list
		int *ingID=new int; *ingID=ing->ingredientID;
		ingList.append(ingID);

		// Set values for this ingredient
		int *unitID=new int; *unitID=ing->unitID;
		unitList.append(unitID);
		double *amount=new double; *amount=ing->amount;
		amountList.append(amount);

		}

	}

for (int *id=ingList.first();id;id=ingList.next()) // Warning! use first(),next() with QPtrLists... getFirst
	{
	int pos= ingList.at();
	std::cerr<<"ID: "<<*id<<" Amount: "<<*(amountList.at(pos))<<" "<<*(unitList.at(pos))<<"\n";
	}

}

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


void calculateProperties(Recipe* recipe,RecipeDB* database)
{
IngredientPropertyList recipePropertyList; // It's not attached to any ingredient. It's just the total
IngredientPropertyList ingredientPropertyList; // property list for each ingredient
std::cerr<<"\n\nCalculating recipe properties:\n";

for (Ingredient *ing=recipe->ingList.getFirst();ing;ing=recipe->ingList.getNext())
	{
	std::cerr<<"Ingredient amount:"<<ing->amount<<"\n";
	database->loadProperties(&ingredientPropertyList,ing->ingredientID);
	addPropertyToList(database,recipePropertyList,ingredientPropertyList,*ing);
	}

for (IngredientProperty *prop=recipePropertyList.getFirst();prop;prop=recipePropertyList.getNext())
	{

	std::cerr<<QString("PropertyID: %1 Amount: %2 Units: %3 \n").arg(prop->id).arg(prop->amount).arg(prop->units);
	}

}

void addPropertyToList(RecipeDB *database,IngredientPropertyList &recipePropertyList,IngredientPropertyList &ingPropertyList,Ingredient &ing)
{
for (IngredientProperty *prop=ingPropertyList.getFirst();prop;prop=ingPropertyList.getNext())
	{
	// Find if property was listed before
	int pos=recipePropertyList.find(prop);
	if (pos>=0) //Exists. Add to it
	{
	std::cerr<<"Adding to property: "<<prop->id<< "\n";
	IngredientProperty *property=recipePropertyList.at(pos);
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
	IngredientProperty*property; property=new IngredientProperty;
	property->id=prop->id;
	property->name=property->name;
	property->perUnit.id=-1; // It's not per unit, it's total sum of the recipe
	property->perUnit.name=QString::null; // "
	property->units=property->units;

	double ratio; ratio=database->unitRatio(ing.unitID, prop->perUnit.id);

	if (ratio>0.0) // Could convert units to perUnit
		{
		property->amount=(prop->amount)*(ing.amount)*ratio;
		std::cerr<<(prop->amount)<<";"<<(ing.amount)<<";"<<ratio<<"\n";
		recipePropertyList.append(property);
		}
	else { // Could not convert units
	     std::cerr<<"\nWarning: I could not calculate the full property list, due to impossible unit conversion\n";
	     }




	}

	}
}