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

#include "nycgenericimporter.h"
#include "importertest.h"

int
main(int argc, char *argv[])
{
	KAboutData about("nyctest", 0, ki18n("Nyctest"), "1");
	KCmdLineArgs::init(argc, argv, &about);
    KCmdLineOptions options;
    options.add("+[file]");
    KCmdLineArgs::addCmdLineOptions(options);
	KApplication app;

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    QString file("nyctest.txt");
    if(args->count() >= 1)
        file = args->arg(0);

	printf("Creating NYCGenericImporter.\n");
	NYCGenericImporter importer;

	printf("Parsing nyctest.txt.\n");
	QStringList files; files << file;
	importer.parseFiles(files);

	Recipe recipe;
	recipe.title = "Cookies_Test";
	recipe.yield.setAmount(2);
	recipe.yield.setType("dozen");
	recipe.categoryList.append( Element("Snacks",1) );
	recipe.categoryList.append( Element("Cookies & Squares",2) );
	recipe.instructions = 
		"Drop by spoonful on greased cookie sheet.  Bake in moderate oven.";
	//recipe.prepTime = QTime(0,30);

	recipe.authorList.append( Element("Colleen Beamer") );

	Ingredient ing;
	ing.name = "granulated sugar";
	ing.amount = 0.75;
	ing.units.setName("c.");
	recipe.ingList.append( ing );

	Ingredient ing2;
	ing2.name = "brown sugar";
	ing2.amount = 1;
	ing2.units.setName("c.");
	recipe.ingList.append( ing2 );

	Ingredient ing3;
	ing3.name = "all-purpose flour";
	ing3.amount = 2;
	ing3.units.setPlural("c.");
	recipe.ingList.append( ing3 );

	Ingredient ing4;
	ing4.name = "baking soda";
	ing4.amount = 1;
	ing4.units.setName("tsp.");
	recipe.ingList.append( ing4 );

	Ingredient ing8;
	ing8.name = "shortening";
	ing8.amount = 1;
	ing8.units.setName("c.");
	ing8.prepMethodList.append( Element("softened") );
	ing8.prepMethodList.append( Element("at room temperature") );
	recipe.ingList.append( ing8 );

	Ingredient ing6;
	ing6.name = "peanut butter";
	ing6.amount = 1;
	ing6.units.setName("c.");
	recipe.ingList.append( ing6 );

	Ingredient ing5;
	ing5.name = "eggs";
	ing5.amount = 2;
	ing5.units.setPlural("whole");
	recipe.ingList.append( ing5 );

	Ingredient ing7;
	ing7.name = "vanilla extract";
	ing7.amount = 1;
	ing7.units.setName("tsp.");

	recipe.ingList.append( ing7 );

	check( importer, recipe );

	printf("Done.\n");
}
