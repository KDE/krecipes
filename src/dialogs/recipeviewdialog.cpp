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

this->calculateIngredients();


 }

RecipeViewDialog::~RecipeViewDialog()
{
}

void RecipeViewDialog::loadRecipe(int recipeID)
{

// Load specified Recipe ID

database->loadRecipe(loadedRecipe,recipeID);
showRecipe();
calculateIngredients();

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



void RecipeViewDialog::calculateIngredients(void)
{
Ingredient *ing;
IDList ingList;
QPtrList <int> unitList;
QPtrList <double> amountList;


	for (ing=loadedRecipe->ingList.getFirst();ing; ing=loadedRecipe->ingList.getNext())
	{
	// Find out if ingredient exists in list already
	int pos=ingList.find(&(ing->ingredientID));
	if (pos>=0) // the ingredient is already listed
		{
		std::cerr<<"Increasing ingredient"<<ing->ingredientID<<"\n";

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
		std::cerr<<"I was unable to find a proper conversion\n";

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
		std::cerr<<"Adding new ingredient"<<ing->ingredientID<<"\n";

		// Set values for this ingredient
		int *unitID=new int; *unitID=ing->unitID;
		unitList.append(unitID);
		double *amount=new double; *amount=ing->amount;
		amountList.append(amount);

		}

	}


	for (int *id=ingList.first();id; id=ingList.next()) //warning, use "first" not getFirst with QPtrList
	{
	int pos =ingList.at();
	double *amount=amountList.at(pos);
	int *unitID=unitList.at(pos);
	std::cerr<<"Ingredient: "<<*id<<" Amount: "<<*amount<<" UnitID: "<<*unitID<<"\n";
	}

}

int RecipeViewDialog::autoConvert(double amount1,int unit1,double amount2,int unit2, double &newAmount, int &newID)
{

std::cerr<<"Finding ratio between: "<<unit1<<" and "<<unit2<<"\n";

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