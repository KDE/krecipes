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

#include <QString>

#include <iostream>
using std::cout;
using std::endl;

#include "rezkonvimporter.h"
#include "rezkonvexporter.h"
#include "importertest.h"
#include "exportertest.h"

int
main(int argc, char *argv[])
{
	KApplication a(argc, argv, "rezkonvtest");

	printf("Creating RezkonvImporter.\n");
	RezkonvImporter importer;

	printf("Parsing rezkonvtest.txt.\n");
	QStringList files; files << "rezkonvtest.txt";
	importer.parseFiles(files);

	Recipe recipe;
	recipe.title = "Cookies_Test";
	recipe.yield.amount = 2;
	recipe.yield.amount_offset = 1;
	recipe.yield.type = "dozen";
	recipe.categoryList.append( Element("Snacks") );
	recipe.categoryList.append( Element("Cookies & Squares") );
	recipe.instructions = 
		"\n\nDrop by spoonful on greased cookie sheet.  Bake in moderate oven.";

	recipe.authorList.append( Element("Mona Beamer") );
	recipe.authorList.append( Element("Colleen Beamer") );

	Ingredient ing9;
	ing9.name = "a";
	ing9.amount = 1;
	ing9.amount_offset = 0;
	ing9.units.name = "cup";
	IngredientData ing9_1;
	ing9_1.name = "b";
	ing9_1.amount = 2;
	ing9_1.amount_offset = 0;
	ing9_1.units.plural = "cups";
	IngredientData ing9_2;
	ing9_2.name = "c";
	ing9_2.amount = 3;
	ing9_2.amount_offset = 0;
	ing9_2.units.plural = "cups";
	ing9.substitutes.append(ing9_1);
	ing9.substitutes.append(ing9_2);
	recipe.ingList.append( ing9 );

	Ingredient ing;
	ing.name = "granulated sugar";
	ing.amount = 0.75;
	ing.amount_offset = 0.25;
	ing.units.name = "c.";
	ing.groupID = 0; ing.group = "Dry Ingredients";
	recipe.ingList.append( ing );

	Ingredient ing2;
	ing2.name = "brown sugar";
	ing2.amount = 1;
	ing2.amount_offset = 0;
	ing2.units.name = "c.";
	ing2.groupID = 0; ing2.group = "Dry Ingredients";
	recipe.ingList.append( ing2 );

	Ingredient ing3;
	ing3.name = "all-purpose flour";
	ing3.amount = 2;
	ing3.units.plural = "c.";
	ing3.groupID = 0; ing3.group = "Dry Ingredients";
	recipe.ingList.append( ing3 );

	Ingredient ing4;
	ing4.name = "baking soda";
	ing4.amount = 1;
	ing4.amount_offset = 0;
	ing4.units.name = "tsp.";
	ing4.groupID = 0; ing4.group = "Dry Ingredients";
	recipe.ingList.append( ing4 );

	Ingredient ing8;
	ing8.name = "shortening";
	ing8.amount = 1;
	ing8.amount_offset = 0;
	ing8.units.name = "c.";
	ing8.prepMethodList.append( Element("softened") );
	ing8.prepMethodList.append( Element("at room temperature") );
	ing8.groupID = 1; ing8.group = "Fat & Liquids";
	recipe.ingList.append( ing8 );

	Ingredient ing6;
	ing6.name = "peanut butter";
	ing6.amount = 1;
	ing6.amount_offset = 0;
	ing6.units.name = "c.";
	ing6.groupID = 1; ing6.group = "Fat & Liquids";
	recipe.ingList.append( ing6 );

	Ingredient ing5;
	ing5.name = "eggs";
	ing5.amount = 2;
	ing5.amount_offset = 0;
	ing5.units.plural = "";
	ing5.groupID = 1; ing5.group = "Fat & Liquids";
	recipe.ingList.append( ing5 );

	Ingredient ing7;
	ing7.name = "vanilla extract";
	ing7.amount = 1;
	ing7.amount_offset = 0;
	ing7.units.name = "tsp.";
	ing7.groupID = 1; ing7.group = "Fat & Liquids";
	recipe.ingList.append( ing7 );

	check( importer, recipe );

	RecipeList recipeList;
	recipeList.append(recipe);
	recipeList.append(recipe);

	printf("Creating RezkonvExporter.\n");
	RezkonvExporter exporter("not needed",".rk");
	check( exporter, recipeList );
	printf("Successfully exported recipes to test.txt.\n");

	printf("Creating RezkonvImporter to test exported recipes.\n");
	RezkonvImporter importer2;

	printf("Parsing test.txt.\n");
	QStringList files2; files2 << "test.txt";
	importer2.parseFiles(files2);
	QFile::remove("test.txt");
	check( importer2, recipe );
	printf("Recipe export successful.\n");

	printf("Done.\n");
}
