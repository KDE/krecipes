/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "recipeviewdialog.h"
#include "image.h"

RecipeViewDialog::RecipeViewDialog(QWidget *parent, RecipeDB *db, int recipeID):QVBox(parent)
{

// Initialize UI Elements
recipeView=new KHTMLPart(this);

// Store/Initialize local variables
database=db; // Store the database pointer.
loadedRecipe=new Recipe();

//----------Load  the recipe --------
loadRecipe(recipeID);

//this->calculateProperties();


 }

RecipeViewDialog::~RecipeViewDialog()
{
}

void RecipeViewDialog::loadRecipe(int recipeID)
{

// Load specified Recipe ID

database->loadRecipe(loadedRecipe,recipeID);
showRecipe();
calculateProperties();

}

void RecipeViewDialog::showRecipe(void)
{
QString recipeHTML;



// Create HTML Code
if (loadedRecipe->recipeID<0)
{
// Show default (empty) recipe
recipeHTML="<html><head><title>Title of the Recipe</title></head><body>";
recipeHTML+="<div STYLE=\"position: absolute; top: 30px; left:1%; width: 22%\"> <li>Ingredient 1</li>";
recipeHTML+="<li>Ingredient 2</li> <li>Ingredient 3</li> </div>";
recipeHTML+="<div STYLE=\"position: absolute; top: 30px; left:25%; width: 74%\">";
recipeHTML+="<center><h1>Title of the Recipe</h1></center>";
recipeHTML+="<p>Recipe Instructions </p></div></body></html>";
}
else
{
// Format the loaded recipe as HTML code

// title (not shown)
recipeHTML= QString("<html><head><title>%1</title></head><body>").arg( loadedRecipe->title);
// Ingredient Block
recipeHTML+="<div STYLE=\"position: absolute; top: 250px; left:1%; width: 220px; background-color: #D4A143\">";
    //Ingredients
    Ingredient * ing;
    for ( ing = loadedRecipe->ingList.getFirst(); ing; ing = loadedRecipe->ingList.getNext() )
       {
       recipeHTML+=QString("<li>%2 %3  %1</li>")
			    .arg(ing->name)
			    .arg(ing->amount)
			    .arg(ing->units);
       }
recipeHTML+="</div>";


// Instructions Block
recipeHTML+="<div STYLE=\"margin-left: 240px;margin-right: 1%;margin-top: 80px\">";
recipeHTML+=QString("<center><h1>%1</h1></center>").arg(loadedRecipe->title);
recipeHTML+=QString("<p>%1</p></div>").arg(loadedRecipe->instructions);

// Photo Block

recipeHTML+="<div STYLE=\"position: absolute; top: 50px; left:1%; width: 220px; height: 165px; border: solid #000000 1px \">";
recipeHTML+=QString("<img src=\"/tmp/krecipes_photo.png\" width=220px height=165px> </div>");

// Header

recipeHTML+="<div STYLE=\"position: absolute; top: 5px; left:1%; width: 98%; height:30px; background-color: #EDD89E\">";
recipeHTML+=QString("<p align=right >Recipe: #%1</p></div>").arg(loadedRecipe->recipeID);

// Close HTML
recipeHTML+="</body></html>";

// Store Photo
if (!loadedRecipe->photo.isNull()) loadedRecipe->photo.save("/tmp/krecipes_photo.png","PNG");
else {QPixmap dp(defaultPhoto); dp.save("/tmp/krecipes_photo.png","PNG");}
}

recipeView->begin(KURL("file:/tmp/" )); // Initialize to /tmp, where the photo was stored
recipeView->write(recipeHTML);
recipeView->end();

}



void RecipeViewDialog::calculateIngredients(IDList &ingList, QPtrList <int> &unitList, QPtrList <double> &amountList)
{
Ingredient *ing;

	for (ing=loadedRecipe->ingList.getFirst();ing; ing=loadedRecipe->ingList.getNext())
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
		converted =autoConvert(ing->amount,ing->unitID,*currentAmount,*currentUnit,newAmount,newUnit);
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

int RecipeViewDialog::autoConvert(double amount1,int unit1,double amount2,int unit2, double &newAmount, int &newID)
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

void RecipeViewDialog::calculatePropertiesOld(void)
{

std::cerr<<"\n\nCalculating recipe properties:\n";
// For ingredients reorganization
IDList ingList;
QPtrList <int> ingUnitList;
QPtrList <double> ingAmountList;

// Properties data
IngredientPropertyList ingPropertyList; // To store properties of each ingredient
IDList propertyList; // To store the final result
QPtrList <double> propertyAmountList;
QPtrList <QString> propertyUnitList;

// Recalculate (reorganize) the ingredient list
calculateIngredients(ingList,ingUnitList,ingAmountList);

for (int *ing=ingList.first();ing;ing=ingList.next()) // Warning! use first(),next() with QPtrLists... getFirst behaves differently
	{
	int pos=ingList.at();
	database->loadProperties(&ingPropertyList,*ing);
	addPropertyToList(propertyList,propertyAmountList,propertyUnitList,ingPropertyList,*(ingUnitList.at(pos)),*(ingAmountList.at(pos)));
	}

for (int *propid=propertyList.first();propid;propid=propertyList.next()) // Warning! use first(),next() with QPtrLists... getFirst
	{
	int pos= propertyList.at();
	std::cerr<<"Property ID: "<<*propid<<" Amount: "<<*(propertyAmountList.at(pos))<<*(propertyUnitList.at(pos))<<"\n";
	}

}

void RecipeViewDialog::addPropertyToList(IDList &idList,QPtrList <double> & amountList,QPtrList <QString> &unitList,IngredientPropertyList &list,int ingUnitID, double ingAmount )
{
IngredientProperty *prop;
	for (prop=list.getFirst();prop; prop=list.getNext())
	{
		int pos=idList.find(&(prop->id)); // Find out if property already exists in the list

		if (pos>=0) // the property is already listed
		{
		double ratio=database->unitRatio(ingUnitID,prop->perUnit.id);
		double *currentAmount=amountList.at(pos);

		if (ratio>0.0)
			{
			*currentAmount+=ingAmount*ratio*prop->amount;
			std::cerr<<"\n"<<*currentAmount<<"\n";
			}
		else    {
			std::cerr<<"Warning: could not calculate ingredient ratio. The final property list won't be accurate. \n";
		        }

		}

		else // add the property to the list
		{
		// Insert property ID in the list
		int *propID=new int; *propID=prop->id;
		idList.append(propID);

		// Set values for this property

		// Set the units
		QString *unit=new QString; *unit=prop->units;
		unitList.append(unit);

		// Calculate and set amount
		double *amount=new double; *amount=prop->amount;
		double ratio=database->unitRatio(ingUnitID,prop->perUnit.id);
		if (ratio>0)
			{
			*amount=ingAmount*ratio*prop->amount;
			amountList.append(amount);
			}
		else
			{
			std::cerr<<"I couldn't convert units properly. The final result will be inaccurate\n";
			*amount=0;
			amountList.append(amount);
			}


		}

	}

}


void RecipeViewDialog::calculateProperties(void)
{
IngredientPropertyList recipePropertyList; // It's not attached to any ingredient. It's just the total
IngredientPropertyList ingredientPropertyList; // property list for each ingredient
std::cerr<<"\n\nCalculating recipe properties:\n";

for (Ingredient *ing=loadedRecipe->ingList.getFirst();ing;ing=loadedRecipe->ingList.getNext())
	{
	std::cerr<<"Ingredient amount:"<<ing->amount<<"\n";
	database->loadProperties(&ingredientPropertyList,ing->ingredientID);
	addPropertyToList(recipePropertyList,ingredientPropertyList,*ing);
	}

for (IngredientProperty *prop=recipePropertyList.getFirst();prop;prop=recipePropertyList.getNext())
	{

	std::cerr<<QString("PropertyID: %1 Amount: %2 Units: %3 \n").arg(prop->id).arg(prop->amount).arg(prop->units);
	}

}

void RecipeViewDialog::addPropertyToList(IngredientPropertyList &recipePropertyList,IngredientPropertyList &ingPropertyList,Ingredient &ing)
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