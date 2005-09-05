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
#include <qfile.h>

#include <iostream>

#include "kreimporter.h"
#include "kreexport.h"
#include "importertest.h"
#include "exportertest.h"
#include "image.h"

int
main(int argc, char *argv[])
{
	KApplication a(argc, argv, "kretest");

	printf("Creating KreImporter.\n");
	KreImporter importer;

	printf("Parsing kretest.txt.\n");
	QStringList files; files << "kretest.txt";
	importer.parseFiles(files);

	Recipe recipe;
	recipe.title = "Title 1";
	recipe.yield.amount = 2;
	recipe.yield.amount_offset = 3;
	recipe.yield.type = "type";
	recipe.categoryList.append( Element("Subcategory 1",2) );
	recipe.categoryList.append( Element("Category 2",3) );
	recipe.instructions = 
		"Instruction line 1\n"
		"Instruction line 2\n"
		"Instruction line 3";
	recipe.photo = QPixmap(defaultPhoto);
	recipe.prepTime = QTime(1,30);

	recipe.authorList.append( Element("Author 1") );
	recipe.authorList.append( Element("Author 2") );

	Ingredient ing;
	ing.name = "ingredient 1";
	ing.amount = 1;
	ing.amount_offset = 2;
	ing.units.plural = "teaspoons";
	ing.prepMethod = QString::null;
	recipe.ingList.append( ing );

	Ingredient ing2;
	ing2.name = "ingredient 2";
	ing2.amount = 3.5;
	ing2.units.plural = "";
	ing2.prepMethod = "prep";
	recipe.ingList.append( ing2 );

	Ingredient ing3;
	ing3.name = "ingredient 3";
	ing3.amount = 3.5;
	ing3.units.plural = "ounces";
	ing3.prepMethod = QString::null;
	recipe.ingList.append( ing3 );

	Ingredient ing4;
	ing4.name = "ingredient 4";
	ing4.amount = 3.5;
	ing4.amount_offset = 0.5;
	ing4.units.plural = "ounces";
	ing4.prepMethod = QString::null;
	recipe.ingList.append( ing4 );


	CategoryTree *catTree = new CategoryTree;
	CategoryTree *child = catTree->add( Element("Category 1",1) );
		(void)child->add( Element("Subcategory 1",2) );
	(void)catTree->add( Element("Category 2",3) );
	
	check( importer, recipe );


	RecipeList recipeList;
	recipeList.append(recipe);
	recipeList.append(recipe);

	printf("Creating KreExporter.\n");
	KreExporter exporter(catTree,"not needed",".kreml");
	check( exporter, recipeList );
	printf("Successfully exported recipes to test.txt.\n");

	printf("Creating KreImporter to test exported recipes.\n");
	KreImporter importer2;

	printf("Parsing test.txt.\n");
	QStringList files2; files2 << "test.txt";
	importer2.parseFiles(files2);
	QFile::remove("test.txt");
	check( importer2, recipe );
	printf("Recipe export successful.\n");

	printf("*** Krecipes importer and exporter passed the tests :-) ***\n");
}
