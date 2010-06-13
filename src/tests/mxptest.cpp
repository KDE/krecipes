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

#include "mxpimporter.h"
#include "importertest.h"

int
main(int argc, char *argv[])
{
	KAboutData about("mxptest", 0, ki18n("Mxptest"), "1");
	KCmdLineArgs::init(argc, argv, &about);
	KCmdLineOptions options;
	options.add("+[file]");
	KCmdLineArgs::addCmdLineOptions(options);
	KApplication app;

	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	QString file("mxptest.txt");
	if(args->count() >= 1)
		file = args->arg(0);

	printf("Creating MXPImporter.\n");
	MXPImporter importer;

	printf("Parsing mxptest.txt.\n");
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
		"Drop by spoonful on greased cookie sheet.  Bake in moderate oven.\n"
		"\n"
		"Nutr. Assoc. : 0 1374 1021 927 0 1638 1358 797 0 0 0 568 0 532 1611";
	recipe.prepTime = QTime(0,45);

	recipe.authorList.append( Element("Mona Beamer, Colleen Beamer") );

	Ingredient ing;
	ing.name = "granulated sugar";
	ing.amount = 0.75;
	//ing.amount_offset = 0.25;
	ing.units.setName("c.");
	//ing.groupID = 0; ing.group = "Dry Ingredients";
	recipe.ingList.append( ing );

	Ingredient ing2;
	ing2.name = "brown sugar";
	ing2.amount = 1;
	ing2.amount_offset = 0;
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
	ing8.prepMethodList.append( Element("softened,at room temperature") );
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

	printf("Done.\n");
}
