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
#include <QFile>

#include <iostream>
using std::cout;
using std::endl;

#include "mx2importer.h"
#include "mx2exporter.h"
#include "importertest.h"
#include "exportertest.h"

int
main(int argc, char *argv[])
{
	KAboutData about("mx2test", 0, ki18n("Mx2test"), "1");
	KCmdLineArgs::init(argc, argv, &about);
	KCmdLineOptions options;
	options.add("+[file]");
	KCmdLineArgs::addCmdLineOptions(options);
	KApplication app;

	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	QString file("mx2test.txt");
	if(args->count() >= 1)
		file = args->arg(0);

	printf("Creating MX2Importer.\n");
	MX2Importer importer;

	printf("Parsing mx2test.txt.\n");
	QStringList files; files << file;
	importer.parseFiles(files);

	Recipe recipe;
	recipe.title = "Cookies Test";
	recipe.yield.setAmount(2);
	recipe.yield.setAmountOffset(0);
	recipe.yield.setType("servings");
	recipe.categoryList.append( Element("Snacks",1) );
	recipe.categoryList.append( Element("Cookies & Squares",2) );
	recipe.instructions = 
		"1. Drop by spoonful on greased cookie sheet.\n"
		"\n"
		"2. Bake in moderate oven.";
	recipe.prepTime = QTime(0,30);

	recipe.authorList.append( Element("Colleen Beamer") );
	//recipe.authorList.append( Element("Mona Beamer") );

	Ingredient ing;
	ing.name = "granulated sugar";
	ing.amount = 0.75;
	ing.amount_offset = 0.25;
	ing.units.setName("c.");
	//ing.groupID = 0; ing.group = "Dry Ingredients";
	recipe.ingList.append( ing );

	Ingredient ing2;
	ing2.name = "brown sugar";
	ing2.amount = 0.75;
	ing2.amount_offset = 0.25;
	ing2.units.setName("c.");
	//ing2.groupID = 0; ing2.group = "Dry Ingredients";
	recipe.ingList.append( ing2 );

	Ingredient ing3;
	ing3.name = "all-purpose flour";
	ing3.amount = 2;
	ing3.units.setPlural("c.");
	//ing3.groupID = 0; ing3.group = "Dry Ingredients";
	recipe.ingList.append( ing3 );

	Ingredient ing4;
	ing4.name = "baking soda";
	ing4.amount = 1;
	ing4.amount_offset = 0;
	ing4.units.setName("tsp.");
	//ing4.groupID = 0; ing4.group = "Dry Ingredients";
	recipe.ingList.append( ing4 );

	Ingredient ing8;
	ing8.name = "shortening";
	ing8.amount = 1;
	ing8.amount_offset = 0;
	ing8.units.setName("c.");
	ing8.prepMethodList.append( Element("softened, at room temperature") );
	//ing8.groupID = 1; ing8.group = "Fat & Liquids";
	recipe.ingList.append( ing8 );

	Ingredient ing6;
	ing6.name = "peanut butter";
	ing6.amount = 1;
	ing6.amount_offset = 0;
	ing6.units.setName("c.");
	//ing6.groupID = 1; ing6.group = "Fat & Liquids";
	recipe.ingList.append( ing6 );

	Ingredient ing5;
	ing5.name = "eggs";
	ing5.amount = 2;
	ing5.amount_offset = 0;
	ing5.units.setPlural("");
	//ing5.groupID = 1; ing5.group = "Fat & Liquids";
	recipe.ingList.append( ing5 );

	Ingredient ing7;
	ing7.name = "vanilla extract";
	ing7.amount = 1;
	ing7.amount_offset = 0;
	ing7.units.setName("tsp.");
	//ing7.groupID = 1; ing7.group = "Fat & Liquids";
	recipe.ingList.append( ing7 );
		
	check( importer, recipe );

	RecipeList recipeList;
	recipeList.append(recipe);
	recipeList.append(recipe);

	printf("Creating MX2Exporter.\n");
	MX2Exporter exporter("not needed",".mx2");
	check( exporter, recipeList );
	printf("Successfully exported recipes to test.txt.\n");

	printf("Creating MX2Importer to test exported recipes.\n");
	MX2Importer importer2;

	printf("Parsing test.txt.\n");
	QStringList files2; files2 << "test.txt";
	importer2.parseFiles(files2);
	QFile::remove("test.txt");
	check( importer2, recipe );
	printf("Recipe export successful.\n");

	printf("*** mx2 importer and exporter passed the tests :-) ***\n");

	printf("Done.\n");
}
