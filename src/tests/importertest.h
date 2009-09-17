/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef IMPORTERTEST_H
#define IMPORTERTEST_H

#include <cmath>
#include <iostream>


#include "checks.h"
#include "datablocks/categorytree.h"

#include <stdio.h>

using std::cout;
using std::endl;

void check( const BaseImporter &importer, const Recipe &recipe )
{
	int recipe_num = 1;
	RecipeList recipeList = importer.recipeList();
	for ( RecipeList::const_iterator it = recipeList.begin(); it != recipeList.end(); ++it ) {
		printf("Recipe %d... ",recipe_num);
		check( *it, recipe );
		printf("successful\n");
		++recipe_num;
	}

	check( "recipe count", recipe_num-1, 2 );
}

void check( const BaseImporter &importer, const CategoryTree *baseCatStructure )
{
	printf("Checking category structure.\n");
	check( importer.categoryStructure(), baseCatStructure );
}

#endif
