/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#include "propertycalculator.h"

#include <cmath> // For fabs()

#include <kdebug.h>

#include "backends/recipedb.h"
#include "datablocks/elementlist.h"
#include "datablocks/ingredientpropertylist.h"
#include "datablocks/recipe.h"

bool autoConvert( RecipeDB* database, const Ingredient &from, const Ingredient &to, Ingredient &result )
{
	RecipeDB::ConversionStatus status = database->convertIngredientUnits( from, to.units, result );
	bool converted = status == RecipeDB::Success || status == RecipeDB::MismatchedPrepMethodUsingApprox;

	if ( converted )  // There is a ratio
	{
		double ratio = result.amount / from.amount;

		if ( ratio > 1 )  // Convert to unit 1, since unit1 is bigger
		{
			result.units = from.units;
			result.amount = from.amount + to.amount / ratio;
		}
		else { //Convert to unit2, since unit2 is bigger (just add, units are now correct)
			result.amount += to.amount;
		}
		return true;
	}
	else
		return false;
}

/*
** Version with database I/O. DB must be provided
*/

void calculateProperties( Recipe& recipe, RecipeDB* database )
{
	recipe.properties.clear();
	// Note that recipePropertyList is not attached to any ingredient. It's just the total of the recipe
	IngredientPropertyList ingredientPropertyList; // property list for each ingredient

	int ingredientNo = 1;

	for ( IngredientList::const_iterator ing_it = recipe.ingList.begin(); ing_it != recipe.ingList.end(); ++ing_it ) {
		database->loadProperties( &ingredientPropertyList, ( *ing_it ).ingredientID );
		ingredientPropertyList.divide( recipe.yield.amount() ); // calculates properties per yield unit
		addPropertyToList( database, &recipe.properties, ingredientPropertyList, *ing_it, ingredientNo );
		ingredientNo++;
	}
}


void addPropertyToList( RecipeDB *database, IngredientPropertyList *recipePropertyList, IngredientPropertyList &ingPropertyList, const Ingredient &ing, int /*ingredientNo*/ )
{
	QMap<int,double> ratioCache; //unit->ratio

	IngredientPropertyList::const_iterator prop_it;
	for ( prop_it = ingPropertyList.constBegin(); prop_it != ingPropertyList.constEnd(); ++prop_it ) {
		// Find if property was listed before
		int pos = recipePropertyList->findIndex( *prop_it );
		if ( pos >= 0 )  //Exists. Add to it
		{
			Ingredient result;

			bool converted;
			QMap<int,double>::const_iterator cache_it = ratioCache.constFind((*prop_it).perUnit.id());
			if ( cache_it == ratioCache.constEnd() ) {
				RecipeDB::ConversionStatus status = database->convertIngredientUnits( ing, (*prop_it).perUnit, result );
				converted = status == RecipeDB::Success || status == RecipeDB::MismatchedPrepMethodUsingApprox;

				if ( converted )
					ratioCache.insert((*prop_it).perUnit.id(),result.amount / ing.amount);
				else
					ratioCache.insert((*prop_it).perUnit.id(),-1);
			}
			else {
				result.units = (*prop_it).perUnit;
				result.amount = ing.amount * (*cache_it);
				converted = result.amount > 0;
			}

			if ( converted )  // Could convert units to perUnit
				(*recipePropertyList)[pos].amount += ( (*prop_it).amount ) * result.amount;
		}
		else // Append new property
		{
			IngredientProperty property;
			property.id = (*prop_it).id;
			property.name = (*prop_it).name;
			property.perUnit.setId(-1); // It's not per unit, it's total sum of the recipe
			property.perUnit.setName(QString()); // "
			property.units = (*prop_it).units;

			Ingredient result;
			bool converted;
			QMap<int,double>::const_iterator cache_it = ratioCache.constFind((*prop_it).perUnit.id());
			if ( cache_it == ratioCache.constEnd() ) {
				RecipeDB::ConversionStatus status = database->convertIngredientUnits( ing, (*prop_it).perUnit, result );
				converted = status == RecipeDB::Success || status == RecipeDB::MismatchedPrepMethodUsingApprox;
				if ( converted )
					ratioCache.insert((*prop_it).perUnit.id(),result.amount / ing.amount);
				else
					ratioCache.insert((*prop_it).perUnit.id(),-1);
			}
			else {
				result.units = (*prop_it).perUnit;
				result.amount = ing.amount * (*cache_it);
				converted = result.amount > 0;
			}

			if ( converted )  // Could convert units to perUnit
			{
				property.amount = ( (*prop_it).amount ) * result.amount;
				recipePropertyList->append( property );
			}
		}
	}
}
