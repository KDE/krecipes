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

	printf("Done.\n");
}
