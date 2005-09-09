/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include <kapplication.h>

#include <qstring.h>

#include <iostream>
using std::cout;
using std::endl;

#include "mmfimporter.h"
#include "mmfexporter.h"
#include "importertest.h"
#include "exportertest.h"

int
main(int argc, char *argv[])
{
	KApplication a(argc, argv, "mmftest");

	printf("Creating MMFImporter.\n");
	MMFImporter importer;

	printf("Parsing mmftest.txt.\n");
	QStringList files; files << "mmftest.txt";
	importer.parseFiles(files);

	Recipe recipe;
	recipe.title = "Title 1";
	recipe.yield.amount = 2;
	recipe.yield.type = "servings";
	recipe.categoryList.append( Element("Category 1") );
	recipe.categoryList.append( Element("Category 2") );
	recipe.instructions = 
		"Instruction line 1\n"
		"Instruction line 2\n"
		"Instruction line 3";

	Ingredient ing;
	ing.name = "ingredient 1";
	ing.amount = 1;
	ing.units.name = "teaspoon";
	recipe.ingList.append( ing );

	Ingredient ing2;
	ing2.name = "ingredient 2";
	ing2.amount = 3.5;
	ing2.units.plural = QString::null;
	recipe.ingList.append( ing2 );

	Ingredient ing3;
	ing3.name = "ingredient 3";
	ing3.amount = 3.5;
	ing3.units.plural = "ounces";
	recipe.ingList.append( ing3 );

	Ingredient ing4;
	ing4.name = "ingredient 4";
	ing4.amount = 3.5;
	ing4.units.plural = "ounces";
	recipe.ingList.append( ing4 );
	
	check( importer, recipe );

	RecipeList recipeList;
	recipeList.append(recipe);
	recipeList.append(recipe);

	printf("Creating MMFExporter.\n");
	MMFExporter exporter("not needed",".mmf");
	check( exporter, recipeList );
	printf("Successfully exported recipes to test.txt.\n");

	printf("Creating MMFImporter to test exported recipes.\n");
	MMFImporter importer2;

	printf("Parsing test.txt.\n");
	QStringList files2; files2 << "test.txt";
	importer2.parseFiles(files2);
	QFile::remove("test.txt");
	check( importer2, recipe );
	printf("Recipe export successful.\n");

	printf("*** MM format importer and exporter passed the tests :-) ***\n");
}
