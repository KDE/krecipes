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

#include "mx2importer.h"
#include "importertest.h"

int
main(int argc, char *argv[])
{
	KAboutData about("mx2test", 0, ki18n("Mx2test"), "1");
	KCmdLineArgs::init(argc, argv, &about);
	KApplication app;

	printf("Creating MX2Importer.\n");
	MX2Importer importer;

	printf("Parsing mx2test.txt.\n");
	QStringList files; files << "mx2test.txt";
	importer.parseFiles(files);

	Recipe recipe;
	recipe.title = "Cookies Test";
	recipe.yield.amount = 2;
	recipe.yield.amount_offset = 0;
	recipe.yield.type = "servings";
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
	ing.units.name = "c.";
	//ing.groupID = 0; ing.group = "Dry Ingredients";
	recipe.ingList.append( ing );

	Ingredient ing2;
	ing2.name = "brown sugar";
	ing2.amount = 0.75;
	ing2.amount_offset = 0.25;
	ing2.units.name = "c.";
	//ing2.groupID = 0; ing2.group = "Dry Ingredients";
	recipe.ingList.append( ing2 );

	Ingredient ing3;
	ing3.name = "all-purpose flour";
	ing3.amount = 2;
	ing3.units.plural = "c.";
	//ing3.groupID = 0; ing3.group = "Dry Ingredients";
	recipe.ingList.append( ing3 );

	Ingredient ing4;
	ing4.name = "baking soda";
	ing4.amount = 1;
	ing4.amount_offset = 0;
	ing4.units.name = "tsp.";
	//ing4.groupID = 0; ing4.group = "Dry Ingredients";
	recipe.ingList.append( ing4 );

	Ingredient ing8;
	ing8.name = "shortening";
	ing8.amount = 1;
	ing8.amount_offset = 0;
	ing8.units.name = "c.";
	ing8.prepMethodList.append( Element("softened, at room temperature") );
	//ing8.groupID = 1; ing8.group = "Fat & Liquids";
	recipe.ingList.append( ing8 );

	Ingredient ing6;
	ing6.name = "peanut butter";
	ing6.amount = 1;
	ing6.amount_offset = 0;
	ing6.units.name = "c.";
	//ing6.groupID = 1; ing6.group = "Fat & Liquids";
	recipe.ingList.append( ing6 );

	Ingredient ing5;
	ing5.name = "eggs";
	ing5.amount = 2;
	ing5.amount_offset = 0;
	ing5.units.plural = "";
	//ing5.groupID = 1; ing5.group = "Fat & Liquids";
	recipe.ingList.append( ing5 );

	Ingredient ing7;
	ing7.name = "vanilla extract";
	ing7.amount = 1;
	ing7.amount_offset = 0;
	ing7.units.name = "tsp.";
	//ing7.groupID = 1; ing7.group = "Fat & Liquids";
	recipe.ingList.append( ing7 );
		
	check( importer, recipe );

	printf("Done.\n");
}
