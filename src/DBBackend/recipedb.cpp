 /***************************************************************************
 *   Copyright (C) 2003 by krecipes.sourceforge.net authors                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "recipedb.h"

void RecipeDB::loadRecipes( RecipeList *recipes, const QValueList<int>& ids )
{
	recipes->empty();

	for ( QValueList<int>::const_iterator it = ids.begin(); it != ids.end(); ++it )
	{
		Recipe recipe;
		loadRecipe( &recipe, *it );
		recipes->append( recipe );
	}
}
