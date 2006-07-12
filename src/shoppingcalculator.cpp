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

#include "propertycalculator.h"
#include "datablocks/elementlist.h"
#include "datablocks/ingredientlist.h"

#include "backends/recipedb.h"

//NOTE: The code as-is uses the lower value if an ingredient range exists
//	However, the shopping list is calculated using the upper value
//	because the values are already adjusted prior to being passed
//	to these functions

void calculateShopping( const ElementList &recipeList, IngredientList *ingredientList, RecipeDB *db )
{
	for ( ElementList::const_iterator recipe_it = recipeList.begin(); recipe_it != recipeList.end(); ++recipe_it ) {
		Recipe rec;
		db->loadRecipe( &rec, RecipeDB::Ingredients, ( *recipe_it ).id );
		sum( ingredientList, &( rec.ingList ), db );
	}
}

void sum( IngredientList *totalIngredientList, IngredientList *newIngredientList, RecipeDB *db )
{
	for ( IngredientList::const_iterator ing_it = newIngredientList->begin(); ing_it != newIngredientList->end(); ++ing_it ) {
		IngredientList::iterator pos_it;

		// Find out if ingredient exists in list already
		int pos = totalIngredientList->find( ( *ing_it ).ingredientID );

		if ( pos >= 0 )  // the ingredient is already listed
		{
			pos_it = totalIngredientList->at( pos );

			// Variables to store the new total
			Ingredient result;
			bool converted;

			// Do the conversion
			// try to with this and next in the list until conversion rate is
			// found or end of list is reached
			IngredientList::iterator lastpos_it; // for 'backup'
			do
			{
				lastpos_it = pos_it;

				// Try to convert
				converted = autoConvert( db, *ing_it, (*pos_it).units, result );
			}
			while ( ( !converted ) && ( ( ( pos_it = totalIngredientList->find( ++pos_it, ( *ing_it ).ingredientID ) ) ) != totalIngredientList->end() ) );

			// If the conversion was succesful, Set the New Values
			if ( converted >= 0 )
				*lastpos_it = result;
			else // Otherwise append this ingredient at the end of the list
			{
				// Insert ingredient ID in the list
				totalIngredientList->append( *ing_it );
			}
		}
		else // The ingredient is not in the list, just append
		{
			totalIngredientList->append( *ing_it );
		}
	}
}
