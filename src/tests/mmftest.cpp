/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include <kapplication.h>
#include <KCmdLineArgs>
#include <KAboutData>

#include <QString>

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
	KAboutData about("mmftest", 0, ki18n("Mmftest"), "1");
	KCmdLineArgs::init(argc, argv, &about);
	KCmdLineOptions options;
	options.add("+[file]");
	KCmdLineArgs::addCmdLineOptions(options);
	KApplication app;

	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	QString file("mmftest.txt");
	if(args->count() >= 1)
		file = args->arg(0);

	printf("Creating MMFImporter.\n");
	MMFImporter importer;

	printf("Parsing mmftest.txt.\n");
	QStringList files; files << file;
	importer.parseFiles(files);

	Recipe recipe;
	recipe.title = "Cookies_Test";
	recipe.yield.setAmount(2);
	recipe.yield.setType("servings");
	recipe.categoryList.append( Element("Snacks",1) );
	recipe.categoryList.append( Element("Cookies & Squares",2) );
	recipe.instructions = 
		"Drop by spoonful on greased cookie sheet.  Bake in moderate oven.";
	recipe.prepTime = QTime(0,0); //It's not QTime(0,30)
	//because mmf format doesn't provide preparation time.

	Ingredient ing9;
	ing9.name = "a";
	ing9.amount = 1;
	ing9.amount_offset = 0;
	ing9.units.setName("cup");
	IngredientData ing9_1;
	ing9_1.name = "b";
	ing9_1.amount = 2;
	ing9_1.amount_offset = 0;
	ing9_1.units.setPlural("cups");
	IngredientData ing9_2;
	ing9_2.name = "c";
	ing9_2.amount = 3;
	ing9_2.amount_offset = 0;
	ing9_2.units.setPlural("cups");
	ing9.substitutes.append(ing9_1);
	ing9.substitutes.append(ing9_2);
	recipe.ingList.append( ing9 );

	Ingredient ing2;
	ing2.name = "c. granulated sugar";
	ing2.amount = 0.75;
	ing2.groupID = 0; ing2.group = "Dry Ingredients";
	recipe.ingList.append( ing2 );

	Ingredient ing;
	ing.name = "c. brown sugar";
	ing.amount = 1;
	ing.amount_offset = 0;
	ing.groupID = 0; ing.group = "Dry Ingredients";
	recipe.ingList.append( ing );

	Ingredient ing3;
	ing3.name = "c. all-purpose flour";
	ing3.amount = 2;
	ing3.groupID = 0; ing3.group = "Dry Ingredients";
	recipe.ingList.append( ing3 );

	Ingredient ing4;
	ing4.name = "tsp. baking soda";
	ing4.amount = 1;
	ing4.groupID = 0; ing4.group = "Dry Ingredients";
	recipe.ingList.append( ing4 );

	Ingredient ing8;
	ing8.name = "c. shortening";
	ing8.amount = 1;
	ing8.prepMethodList.append( Element("softened") );
	ing8.prepMethodList.append( Element("at room temperature") );
	ing8.groupID = 1; ing8.group = "Fat & Liquids";
	recipe.ingList.append( ing8 );

	Ingredient ing6;
	ing6.name = "c. peanut butter";
	ing6.amount = 1;
	ing6.groupID = 1; ing6.group = "Fat & Liquids";
	recipe.ingList.append( ing6 );

	Ingredient ing5;
	ing5.name = "eggs";
	ing5.amount = 2;
	ing5.groupID = 1; ing5.group = "Fat & Liquids";
	recipe.ingList.append( ing5 );

	Ingredient ing7;
	ing7.name = "tsp. vanilla extract";
	ing7.amount = 1;
	ing7.groupID = 1; ing7.group = "Fat & Liquids";
	recipe.ingList.append( ing7 );


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
