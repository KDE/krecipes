 /**************************************************************************
 *   Copyright (C) 2003 by krecipes.sourceforge.net authors                *
 *                                                                         *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "literecipedb.h"
#include "kstandarddirs.h"

#define DB_FILENAME "krecipes.krecdb"

LiteRecipeDB::LiteRecipeDB(QString host, QString user, QString pass, QString DBname,bool init):RecipeDB(host, user,pass,DBname,init)
{

// Define the DB file to be working on. Right now, only hardcoded

QString  dbFile=locateLocal ("appdata",DB_FILENAME);

std::cerr<<"Connecting to the SQLite database\n";
	DBuser=user;DBpass=pass;DBhost=host;

        database= new QSQLiteDB();
	database->open(dbFile);
        if ( !database->open(dbFile) ) {
	     //Try to create the database
	     std::cerr<<"Creating the SQLite database!\n";
	     createDB();

	     //Now Reopen the Database and exit if it fails
	     if (!database->open(dbFile))
		{
		std::cerr<<"Retrying to open the db after creation\n";
		std::cerr<<QString("Could not open DB. You may not have permissions. Exiting.\n").arg(user).latin1();
		exit(1);
		}

	     // Initialize database if requested
	      if (init) initializeDB();
	     }
	 else // Check integrity of the database (tables). If not possible, exit
	 {
	 std::cerr<<"I'll check the DB integrity now\n";
	 	if (!checkIntegrity())
			{
			std::cerr<<"Failed to fix database structure. Exiting.\n";
			 exit(1);
			 }
	 }
}
LiteRecipeDB::~LiteRecipeDB()
{
database->close();
}

void LiteRecipeDB::createDB()
{

// FIXME : create database file?
}

void LiteRecipeDB::loadAllRecipeIngredients(RecipeIngredientList *list,bool withNames)
{
list->ilist.clear();
list->recipeIdList.clear();

QString command;
if (withNames)
{
command=QString("SELECT il.ingredient_id,i.name,il.amount,u.id,u.name,il.recipe_id FROM ingredient_list il LEFT JOIN ingredients i ON (i.id=il.ingredient_id) LEFT JOIN units u  ON (u.id=il.unit_id);" );
}
else
{
command=QString("SELECT ingredient_id,amount,unit_id,recipe_id FROM ingredient_list;" );
}

QSQLiteResult ingredientsToLoad=database->executeQuery( command);

	    if (ingredientsToLoad.getStatus() != QSQLiteResult::Failure) {
	    QSQLiteResultRow row = ingredientsToLoad.first();
                while ( !ingredientsToLoad.atEnd() ) {
		    Ingredient ing;
		    if (withNames)
		    {
		    ing.ingredientID=row.data(0).toInt();
		    ing.name=unescapeAndDecode(row.data(1));
		    ing.amount=row.data(2).toDouble();
		    ing.unitID=row.data(3).toInt();
		    ing.units=unescapeAndDecode(row.data(4));
		    list->recipeIdList.append(row.data(5).toInt());
		    }
		    else
		    {
		    ing.ingredientID=row.data(0).toInt();
		    ing.amount=row.data(1).toDouble();
		    ing.unitID=row.data(2).toInt();
		    list->recipeIdList.append(row.data(3).toInt());
		    }
		    list->ilist.add(ing);

		    row=ingredientsToLoad.next();

                }
            }

}

void LiteRecipeDB::loadRecipe(Recipe *recipe,int recipeID)
{

// Empty the recipe first
recipe->empty();

QString command;

// Read title, author and instructions
command=QString("SELECT title,instructions,persons FROM recipes WHERE id=%1;").arg(recipeID);


QSQLiteResult recipeToLoad = database->executeQuery(command);

if (recipeToLoad.getStatus() != QSQLiteResult::Failure)
        {
                QSQLiteResultRow row = recipeToLoad.first();
		if (!recipeToLoad.atEnd())
		{
		recipe->title=unescapeAndDecode(row.data(0));
		recipe->instructions=unescapeAndDecode(row.data(1));
		recipe->persons=row.data(2).toInt();
		recipe->recipeID=recipeID;
		}
		else
		{
		return; // Recipe doesn't exist
		}
        }
        else
        {
                std::cerr<<recipeToLoad.getError();
		return; // There were problems while loading the recipe
        }



// Read the ingredients

command=QString("SELECT il.ingredient_id,i.name,il.amount,u.id,u.name FROM ingredient_list il LEFT JOIN ingredients i ON (i.id=il.ingredient_id) LEFT JOIN units u  ON (u.id=il.unit_id) WHERE il.recipe_id=%1 ORDER BY il.order_index;" ).arg(recipeID);

recipeToLoad=database->executeQuery( command);
            if (recipeToLoad.getStatus() != QSQLiteResult::Failure) {
	    QSQLiteResultRow row = recipeToLoad.first();
                while ( !recipeToLoad.atEnd() ) {
		    Ingredient ing;
		    ing.ingredientID=row.data(0).toInt();
		    ing.name=unescapeAndDecode(row.data(1));
		    ing.amount=row.data(2).toDouble();
		    ing.unitID=row.data(3).toInt();
		    ing.units=unescapeAndDecode(row.data(4));

		    recipe->ingList.add(ing);
		    row=recipeToLoad.next();

                }
            }


//Load the Image

command=QString("SELECT photo FROM recipes WHERE id=%1;").arg(recipeID);


recipeToLoad = database->executeQuery(command);

 if (recipeToLoad.getStatus() != QSQLiteResult::Failure) {
 	QSQLiteResultRow row = recipeToLoad.first();
	if (!recipeToLoad.atEnd())
	{
   	bool ok; QString photoString=row.data(0,&ok);

	// Decode the photo
	uchar *photoArray=new uchar [photoString.length()+1];
	memcpy(photoArray,photoString.latin1(), photoString.length()*sizeof(char));
	sqlite_decode_binary((uchar*)photoArray,(uchar*)photoArray);


	if (ok) recipe->photo.loadFromData( photoArray,photoString.length());
     	// picture will now have a ready-to-use image
	}

  }




//Load the category list
command=QString("SELECT cl.category_id,c.name FROM category_list cl, categories c WHERE recipe_id=%1 AND cl.category_id=c.id;").arg(recipeID);

recipeToLoad=database->executeQuery( command);
	if (recipeToLoad.getStatus() != QSQLiteResult::Failure) {
	QSQLiteResultRow row = recipeToLoad.first();
                while (!recipeToLoad.atEnd()) {
		    Element el;
		    el.id=row.data(0).toInt();
		    el.name=unescapeAndDecode(row.data(1));
		    if (el.id!=-1) recipe->categoryList.add(el); // add to list except for default category (-1)
		    row=recipeToLoad.next();
                }
            }



//Load the author list
command=QString("SELECT al.author_id,a.name FROM author_list al, authors a WHERE recipe_id=%1 AND al.author_id=a.id;").arg(recipeID);

recipeToLoad=database->executeQuery(command);
            if ( recipeToLoad.getStatus()!=QSQLiteResult::Failure ) {
	    QSQLiteResultRow row = recipeToLoad.first();
                while (!recipeToLoad.atEnd() ) {
		    Element el;
		    el.id=row.data(0).toInt();
		    el.name=unescapeAndDecode(row.data(1));
		    recipe->authorList.add(el);
		    row=recipeToLoad.next();
                }
            }

}

/*
Loads a recipe detail list (no instructions, no photo, no ingredients)
*/

void LiteRecipeDB::loadRecipeDetails(RecipeList *rlist,bool loadIngredients,bool loadCategories)
{

QMap <int,RecipeList::Iterator> recipeIterators; // Stores the iterator of each recipe in the list;

rlist->clear();

QString command;

command=QString("SELECT id,title,persons FROM recipes;");

QSQLiteResult recipesToLoad = database->executeQuery(command);

Recipe rec; // To be used to load the recipes one by one

            if ( recipesToLoad.getStatus()!=QSQLiteResult::Failure ) {
	    QSQLiteResultRow row=recipesToLoad.first();
                while ( !recipesToLoad.atEnd() ) {

		    rec.recipeID=row.data(0).toInt();
		    rec.title=unescapeAndDecode(row.data(1));
		    rec.persons=row.data(2).toInt();
		    RecipeList::Iterator it=rlist->append(rec);
		    recipeIterators[rec.recipeID]=it;

		    row =recipesToLoad.next();
                }
	}
if (loadIngredients) // Note that names of ingredients, units....are not loaded just the needed id's
{


command=QString("SELECT ingredient_id,amount,unit_id,recipe_id FROM ingredient_list;" );

QSQLiteResult ingredientsToLoad=database->executeQuery( command);

	    if (ingredientsToLoad.getStatus() != QSQLiteResult::Failure) {
	    QSQLiteResultRow row = ingredientsToLoad.first();
                while ( !ingredientsToLoad.atEnd() ) {
		    Ingredient ing;

		    // get this ingredient
		    ing.ingredientID=row.data(0).toInt();
		    ing.amount=row.data(1).toDouble();
		    ing.unitID=row.data(2).toInt();

		    // find the corresponding recipe iterator
		    if (recipeIterators.contains(row.data(3).toInt()))
		    {
		    RecipeList::Iterator it=recipeIterators[row.data(3).toInt()];
		    //add the ingredient to the recipe
		    (*it).ingList.add(ing);
		    }

		    row=ingredientsToLoad.next();

                }
            }


}
if (loadCategories)
{
command=QString("SELECT recipe_id,category_id FROM category_list;" ); // Note that we get no names, and the -1 default categories will leak here too.

QSQLiteResult categoriesToLoad=database->executeQuery( command);

	    if (categoriesToLoad.getStatus() != QSQLiteResult::Failure) {
	    QSQLiteResultRow row = categoriesToLoad.first();
                while ( !categoriesToLoad.atEnd() ) {
		    Element cty;

		    // get this category
		    cty.id=row.data(1).toInt();

		    // find the corresponding recipe iterator
		    if (recipeIterators.contains(row.data(0).toInt()))
		    {
		    RecipeList::Iterator it=recipeIterators[row.data(0).toInt()];
		    //add the ingredient to the recipe
		    (*it).categoryList.add(cty);
		    }

		    row=categoriesToLoad.next();

                }
            }


}

}

void LiteRecipeDB::loadIngredients(ElementList *list)
{
list->clear();

QString command;
command="SELECT id,name FROM ingredients ORDER BY name;";

QSQLiteResult ingredientToLoad = database->executeQuery(command);

            if ( ingredientToLoad.getStatus()!=QSQLiteResult::Failure ) {
	    QSQLiteResultRow row=ingredientToLoad.first();
                while ( !ingredientToLoad.atEnd() ) {
		    Element ing;
		    ing.id=row.data(0).toInt();
		    ing.name=unescapeAndDecode(row.data(1));
		    list->add(ing);
		    row =ingredientToLoad.next();
                }
	}



}

void LiteRecipeDB::loadPossibleUnits(int ingredientID, ElementList *list)
{
list->clear();

QString command;

command=QString("SELECT u.id,u.name FROM unit_list ul, units u WHERE ul.ingredient_id=%1 AND ul.unit_id=u.id;").arg(ingredientID);

QSQLiteResult unitToLoad=database->executeQuery( command);

if ( unitToLoad.getStatus()!=QSQLiteResult::Failure ) {
	QSQLiteResultRow row=unitToLoad.first();
            if (!unitToLoad.atEnd() ) {

                while ( !unitToLoad.atEnd() ) {
		    Element unit;
		    unit.id=row.data(0).toInt();
		    unit.name=unescapeAndDecode(row.data(1));
		    list->add(unit);
		    row=unitToLoad.next();
                }
	}
}

}

void LiteRecipeDB::saveRecipe(Recipe *recipe)
{

// Check if it's a new recipe or it exists (supossedly) already.

bool newRecipe; newRecipe=(recipe->recipeID==-1);
// First check if the recipe ID is set, if so, update (not create)
// Be carefull, first check if the recipe hasn't been deleted while changing.

QString command;
if (newRecipe) {command=QString("INSERT INTO recipes VALUES (NULL,'%1',%2,'%3',NULL);") // Id is autoincremented
		.arg(escapeAndEncode(recipe->title))
		.arg(recipe->persons)
		.arg(escapeAndEncode(recipe->instructions));
		}
else		{command=QString("UPDATE recipes SET title='%1',persons=%2,instructions='%3' WHERE id=%4;")
		.arg(escapeAndEncode(recipe->title))
		.arg(recipe->persons)
		.arg(escapeAndEncode(recipe->instructions))
		.arg(recipe->recipeID);
		}

int lastID=-1; QSQLiteResult insertedRecipe=database->executeQuery(command,&lastID);
// If it's a new recipe, identify the ID that was given to the recipe and store in the Recipe itself


if (newRecipe)
{
recipe->recipeID=lastID; // store the new ID in the recipe
}

int recipeID=recipe->recipeID; // Set the recipeID (either new or old)


// Let's begin storing the Image
recipe->photo.save(".krecipe_photo.jpg", "JPEG");
QFileInfo fi(".krecipe_photo.jpg");

// Open the file now, and load to photoArray
QFile f(".krecipe_photo.jpg");
char * photoArray;
long unsigned int n=fi.size();
photoArray=new char[n];
     if(f.open( IO_ReadOnly ))
     {
     f.readBlock(photoArray,n);
     f.close();
     }

char *photoEncodedArray=new char[2 +(257*n)/254+1]; //Just in case, add+1
sqlite_encode_binary((uchar*) photoArray,fi.size(), (uchar*) photoEncodedArray);
 command=QString("UPDATE recipes SET photo='%1' WHERE id=%2;").arg(photoEncodedArray).arg(recipeID);
 database->executeQuery(command);

 //_unlink(".krecipe_photo.jpg");


 // Save the ingredient list (first delete in case we are updating)

 command=QString("DELETE FROM ingredient_list WHERE recipe_id=%1;")
 	.arg(recipeID);
 database->executeQuery(command);

 int order_index=0;
for (Ingredient *ing=recipe->ingList.getFirst(); ing; ing=recipe->ingList.getNext())
	{
	order_index++;
	command=QString("INSERT INTO ingredient_list VALUES (%1,%2,%3,%4,%5);")
	.arg(recipeID)
	.arg(ing->ingredientID)
	.arg(ing->amount)
	.arg(ing->unitID)
	.arg(order_index);
	database->executeQuery(command);

	}

// Save the category list for the recipe (first delete, in case we are updating)
command=QString("DELETE FROM category_list WHERE recipe_id=%1;")
	.arg(recipeID);
database->executeQuery(command);

for (Element *cat=recipe->categoryList.getLast(); cat; cat=recipe->categoryList.getPrev()) // Start from last, mysql seems to work in lifo format... so it's read first the latest inserted one (newest)
	{
	command=QString("INSERT INTO category_list VALUES (%1,%2);")
	.arg(recipeID)
	.arg(cat->id);
	database->executeQuery(command);
}

// Add the default category -1 to ease and speed up searches

	command=QString("INSERT INTO category_list VALUES (%1,-1);")
	.arg(recipeID);
	database->executeQuery(command);

// Save the author list for the recipe (first delete, in case we are updating)
command=QString("DELETE FROM author_list WHERE recipe_id=%1;")
	.arg(recipeID);
database->executeQuery(command);

for (Element *author=recipe->authorList.getLast(); author; author=recipe->authorList.getPrev()) // Start from last, mysql seems to work in lifo format... so it's read first the latest inserted one (newest)
	{
	command=QString("INSERT INTO author_list VALUES (%1,%2);")
	.arg(recipeID)
	.arg(author->id);
	database->executeQuery(command);
	}

}


void LiteRecipeDB::loadRecipeList(ElementList *list,int categoryID,QPtrList <int>*recipeCategoryList)
{
list->clear();

QString command;
QString outputData;
// Load the recipe list



if (!categoryID) // load just the list
	{
	 if (!recipeCategoryList)
	 command="SELECT id,title FROM recipes;";
	 else
	 command="SELECT r.id,r.title,cl.category_id FROM recipes r,category_list cl WHERE r.id=cl.recipe_id;";

	 }
else  // load the list of those in the specified category
	{

	if (!recipeCategoryList)
	command=QString("SELECT r.id,r.title FROM recipes r,category_list cl WHERE r.id=cl.recipe_id AND cl.category_id=%1;").arg(categoryID);
	else
	command=QString("SELECT r.id,r.title,cl.category_id FROM recipes r,category_list cl WHERE r.id=cl.recipe_id AND cl.category_id=%1;").arg(categoryID);
	}

QSQLiteResult recipeToLoad=database->executeQuery(command);
if ( recipeToLoad.getStatus()!=QSQLiteResult::Failure ) {
	QSQLiteResultRow row=recipeToLoad.first();
            while ( !recipeToLoad.atEnd() ) {

		    Element recipe;
		    recipe.id=row.data(0).toInt();
		    recipe.name=unescapeAndDecode(row.data(1));
		    list->add(recipe);

		    if (recipeCategoryList)
		    {
		    int *category=new int;
		    *category=row.data(2).toInt();
		    recipeCategoryList->append (category);
		    }

		    row=recipeToLoad.next();

		}
	}

}

void LiteRecipeDB::removeRecipe(int id)
{
QString command;

command=QString("DELETE FROM recipes WHERE id=%1;").arg(id);
database->executeQuery( command);
command=QString("DELETE FROM ingredient_list WHERE recipe_id=%1;").arg(id);
database->executeQuery( command);
command=QString("DELETE FROM category_list WHERE recipe_id=%1;").arg(id);
database->executeQuery( command);

}

void LiteRecipeDB::removeRecipeFromCategory(int ingredientID, int categoryID){
QString command;
command=QString("DELETE FROM category_list WHERE recipe_id=%1 AND category_id=%2;").arg(ingredientID).arg(categoryID);
database->executeQuery( command);
}

void LiteRecipeDB::createNewIngredient(QString ingredientName)
{
QString command;

command=QString("INSERT INTO ingredients VALUES(NULL,'%1');").arg(escapeAndEncode(ingredientName));
database->executeQuery(command);
}

void LiteRecipeDB::modIngredient(int ingredientID, QString newLabel)
{
QString command;

command=QString("UPDATE ingredients SET name='%1' WHERE id=%2;").arg(escapeAndEncode(newLabel)).arg(ingredientID);
database->executeQuery(command);
}

void LiteRecipeDB::addUnitToIngredient(int ingredientID,int unitID)
{
QString command;

command=QString("INSERT INTO unit_list VALUES(%1,%2);").arg(ingredientID).arg(unitID);
database->executeQuery(command);
}

void LiteRecipeDB::loadUnits(ElementList *list)
{
list->clear();

QString command;

command="SELECT id,name FROM units;";

QSQLiteResult unitToLoad=database->executeQuery(command);
if (unitToLoad.getStatus()!=QSQLiteResult::Failure)
{
	QSQLiteResultRow row =unitToLoad.first();
	while (!unitToLoad.atEnd())
            {
		Element unit;
		unit.id=row.data(0).toInt();
		unit.name=unescapeAndDecode(row.data(1));
		list->add(unit);
		row=unitToLoad.next();
             }
}
}

void LiteRecipeDB::removeUnitFromIngredient(int ingredientID, int unitID)
{
QString command;

command=QString("DELETE FROM unit_list WHERE ingredient_id=%1 AND unit_id=%2;").arg(ingredientID).arg(unitID);
database->executeQuery(command);

// Remove any recipe using that combination of ingredients also (user must have been warned before calling this function!)

command=QString("DELETE recipes.*  FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1 AND il.unit_id=%2;").arg(ingredientID).arg(unitID);
database->executeQuery(command);

// Remove any ingredient properties from ingredient_info where the this ingredient+unit is being used (user must have been warned before calling this function!)
command=QString("DELETE ingredient_info.* FROM ingredient_info ii WHERE ii.ingredient_id=%1 AND ii.per_units=%2;").arg(ingredientID).arg(unitID);
database->executeQuery(command);
// Clean up ingredient_list which have no recipe that they belong to. (I almost don't know how, but this seems to work ;-) Tested using MySQL 4.0.11a
command=QString("DELETE ingredient_list.* FROM ingredient_list LEFT JOIN recipes ON ingredient_list.recipe_id=recipes.id WHERE recipes.id IS NULL;");
database->executeQuery( command);
// Clean up category_list which have no recipe that they belong to. Same method as above
command=QString("DELETE category_list.* FROM category_list LEFT JOIN recipes ON category_list.recipe_id=recipes.id WHERE recipes.id IS NULL;");
database->executeQuery( command);
}


void LiteRecipeDB::findUseOf_Ing_Unit_InRecipes(ElementList *results, int ingredientID, int unitID)
{
 QString command=QString("SELECT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1 AND il.unit_id=%2").arg(ingredientID).arg(unitID);
QSQLiteResult recipeFound=database->executeQuery( command); // Find the entries

// Populate data in the ElementList*
            if ( recipeFound.getStatus()!=QSQLiteResult::Failure ) {
	    QSQLiteResultRow row=recipeFound.first();
                while ( !recipeFound.atEnd() ) {
		    Element recipe;
		    recipe.id=row.data(0).toInt();
		    recipe.name=unescapeAndDecode(row.data(1));
		    results->add(recipe);
		    row=recipeFound.next();
		}
	}
}

void LiteRecipeDB::findUseOfIngInRecipes(ElementList *results,int ingredientID)
{
 QString command=QString("SELECT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1").arg(ingredientID);
QSQLiteResult recipeFound=database->executeQuery(command); // Find the entries

// Populate data in the ElementList*
            if ( recipeFound.getStatus()!=QSQLiteResult::Failure ) {
	    QSQLiteResultRow row=recipeFound.first();
                while ( !recipeFound.atEnd() ) {
		    Element recipe;
		    recipe.id=row.data(0).toInt();
		    recipe.name=unescapeAndDecode(row.data(1));
		    results->add(recipe);
		    row=recipeFound.next();
                }
	}
}

void LiteRecipeDB::removeIngredient(int ingredientID)
{
QString command;

// First remove the ingredient

command=QString("DELETE ingredients.* FROM ingredients WHERE id=%1;").arg(ingredientID);
database->executeQuery(command);

// Remove all the unit entries for this ingredient

command=QString("DELETE unit_list.* FROM unit_list WHERE ingredient_id=%1;").arg(ingredientID);
database->executeQuery(command);

// Remove any recipe using that ingredient

command=QString("DELETE recipes.*  FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1;").arg(ingredientID);
database->executeQuery(command);

// Remove any ingredient in ingredient_list whis has references to inexisting recipes. (As said above, I almost don't know how, but this seems to work ;-) Tested using MySQL 4.0.11a
command=QString("DELETE ingredient_list.* FROM ingredient_list LEFT JOIN recipes ON ingredient_list.recipe_id=recipes.id WHERE recipes.id IS NULL;");
database->executeQuery( command );

// Clean up category_list which have no recipe that they belong to. Same method as above
command=QString("DELETE category_list.* FROM category_list LEFT JOIN recipes ON category_list.recipe_id=recipes.id WHERE recipes.id IS NULL;");
database->executeQuery( command);

// Remove property list of this ingredient
command=QString("DELETE FROM ingredient_info WHERE ingredient_id=%1;").arg(ingredientID);
database->executeQuery(command);
}

void LiteRecipeDB::initializeDB(void)
{

// Create the table structure

	// Read the commands form the structure file
	QString commands;
	QFile file (KGlobal::dirs()->findResource("appdata", "data/litestructure.sql"));
	if ( file.open( IO_ReadOnly ) ) {
	QTextStream stream( &file );
     	commands=stream.read();
     	file.close();
 	}


	// Split commands
	QStringList commandList;
	splitCommands(commands,commandList);
	for ( QStringList::Iterator it = commandList.begin(); it != commandList.end(); ++it )
	 {
	 database->executeQuery((*it)+QString(";")); //Split removes the semicolons
	 }



}

void LiteRecipeDB::initializeData(void)
{

// Populate with data

	QString commands;
	// Read the commands form the data file
	QFile datafile(KGlobal::dirs()->findResource("appdata", "data/data.sql"));
	if ( datafile.open( IO_ReadOnly ) ) {
	QTextStream stream( &datafile );
     	commands=stream.read();
     	datafile.close();
 	}


	// Split commands
	QStringList commandList;
	splitCommands(commands,commandList);

	// Execute commands
	for ( QStringList::Iterator it = commandList.begin(); it != commandList.end(); ++it )
	 {
	 database->executeQuery((*it)+QString(";")); //Split removes the semicolons
	 }

}

void LiteRecipeDB::addProperty(QString &name, QString &units)
{
QString command;
command=QString("INSERT INTO ingredient_properties VALUES(NULL,'%1','%2');").arg(escapeAndEncode(name)).arg(escapeAndEncode(units));
database->executeQuery(command);
}

void LiteRecipeDB::loadProperties(IngredientPropertyList *list,int ingredientID)
{
list->clear();
QString command;
bool usePerUnit;
if (ingredientID>=0) // Load properties of this ingredient
{
usePerUnit=true;
command=QString("SELECT ip.id,ip.name,ip.units,ii.per_units,u.name,ii.amount,ii.ingredient_id FROM ingredient_properties ip, ingredient_info ii, units u WHERE ii.ingredient_id=%1 AND ii.property_id=ip.id AND ii.per_units=u.id;").arg(ingredientID);
}
else if (ingredientID==-1) // Load the properties of all the ingredients
{
usePerUnit=true;
command=QString("SELECT ip.id,ip.name,ip.units,ii.per_units,u.name,ii.amount,ii.ingredient_id FROM ingredient_properties ip, ingredient_info ii, units u WHERE ii.property_id=ip.id AND ii.per_units=u.id;");
}
else // Load the whole property list (just the list of possible properties, not the ingredient properties)
{
usePerUnit=false;
command=QString("SELECT  id,name,units FROM ingredient_properties;");
}

QSQLiteResult propertiesToLoad=database->executeQuery(command);
// Load the results into the list
if (propertiesToLoad.getStatus()!=QSQLiteResult::Failure) {
	QSQLiteResultRow row=propertiesToLoad.first();
                while ( !propertiesToLoad.atEnd() ) {
		    IngredientProperty prop;
		    prop.id=row.data(0).toInt();
		    prop.name=unescapeAndDecode(row.data(1));
		    prop.units=unescapeAndDecode(row.data(2));
		    if (usePerUnit)
		    {
		    prop.perUnit.id=row.data(3).toInt();
		    prop.perUnit.name=unescapeAndDecode(row.data(4));
		    }
		    else
		    {
		    prop.perUnit.id=-1;
		    prop.perUnit.name=QString::null;
		    }

		    if (ingredientID>=0)
		      prop.amount=row.data(5).toDouble();
		    else
		      prop.amount=-1; // Property is generic, not attached to an ingredient

		    if (ingredientID>=-1) prop.ingredientID=row.data(6).toInt();

		    list->add(prop);




		    row=propertiesToLoad.next();
                }

            }
}

void LiteRecipeDB::changePropertyAmountToIngredient(int ingredientID,int propertyID,double amount,int per_units)
{
QString command;
command=QString("UPDATE ingredient_info SET amount=%1 WHERE ingredient_id=%2 AND property_id=%3 AND per_units=%4;").arg(amount).arg(ingredientID).arg(propertyID).arg(per_units);
database->executeQuery(command);
}

void LiteRecipeDB::addPropertyToIngredient(int ingredientID,int propertyID,double amount, int perUnitsID)
{
QString command;

command=QString("INSERT INTO ingredient_info VALUES(%1,%2,%3,%4);").arg(ingredientID).arg(propertyID).arg(amount).arg(perUnitsID);
database->executeQuery(command);
}


void LiteRecipeDB::removePropertyFromIngredient(int ingredientID, int propertyID, int perUnitID)
{
QString command;
// remove property from ingredient info. Note that there could be duplicates with different units (per_units). Remove just the one especified.
command=QString("DELETE FROM ingredient_info WHERE ingredient_id=%1 AND property_id=%2 AND per_units=%3;").arg(ingredientID).arg(propertyID).arg(perUnitID);
database->executeQuery(command);
}

void LiteRecipeDB::removeProperty(int propertyID)
{
QString command;

// Remove property from the ingredient_properties
command=QString("DELETE FROM ingredient_properties WHERE id=%1;").arg(propertyID);
database->executeQuery(command);

// Remove any ingredient info that uses this property
command=QString("DELETE FROM ingredient_info WHERE property_id=%1;").arg(propertyID);
database->executeQuery(command);

}

void LiteRecipeDB::removeUnit(int unitID)
{
QString command;
// Remove the unit first
command=QString("DELETE FROM units WHERE id=%1;").arg(unitID);
database->executeQuery(command);

//Remove the unit from ingredients using it

command=QString("DELETE FROM unit_list WHERE unit_id=%1;").arg(unitID);
database->executeQuery(command);


// Remove any recipe using that unit in the ingredient list (user must have been warned before calling this function!)

command=QString("DELETE recipes.*  FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.unit_id=%1;").arg(unitID);
database->executeQuery(command);

// Remove any ingredient in ingredient_list whis has references to inexisting recipes. (As said above, I almost don't know how, but this seems to work ;-) Tested using MySQL 4.0.11a
command=QString("DELETE ingredient_list.* FROM ingredient_list LEFT JOIN recipes ON ingredient_list.recipe_id=recipes.id WHERE recipes.id IS NULL;");
database->executeQuery( command );

// Clean up category_list which have no recipe that they belong to. Same method as above
command=QString("DELETE category_list.* FROM category_list LEFT JOIN recipes ON category_list.recipe_id=recipes.id WHERE recipes.id IS NULL;");
database->executeQuery( command);

// Remove the ingredient properties using this unit (user must be warned before calling this function)
command=QString("DELETE FROM ingredient_info WHERE per_units=%1;").arg(unitID);
database->executeQuery(command);

// Remove the unit conversion ratios with this unit
command=QString("DELETE FROM units_conversion WHERE unit1_id=%1 OR unit2_id=%2;").arg(unitID).arg(unitID);
database->executeQuery(command);

}


void LiteRecipeDB::createNewUnit(QString &unitName)
{
QString command;

command=QString("INSERT INTO units VALUES(NULL,'%1');").arg(escapeAndEncode(unitName));
database->executeQuery(command);
}


void LiteRecipeDB::modUnit(int unitID, QString newLabel)
{
QString command;

command=QString("UPDATE units SET name='%1' WHERE id=%2;").arg(escapeAndEncode(newLabel)).arg(unitID);
database->executeQuery(command);
}

void LiteRecipeDB::findUseOf_Unit_InRecipes(ElementList *results, int unitID)
{
 QString command=QString("SELECT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.unit_id=%1;").arg(unitID);
QSQLiteResult recipeFound=database->executeQuery( command ); // Find the entries

// Populate data in the ElementList*
            if ( recipeFound.getStatus()!=QSQLiteResult::Failure ) {
	    QSQLiteResultRow row=recipeFound.first();
                while ( !recipeFound.atEnd() ) {
		    Element recipe;
		    recipe.id=row.data(0).toInt();
		    recipe.name=unescapeAndDecode(row.data(1));
		    results->add(recipe);
		    row=recipeFound.next();
                }
	}
}

void LiteRecipeDB::findUseOf_Unit_InProperties(ElementList *results, int unitID)
{
 QString command=QString("SELECT ip.id,ip.name FROM ingredient_info ii, ingredient_properties ip WHERE ii.per_units=%1 AND ip.id=ii.property_id;").arg(unitID);
QSQLiteResult recipeFound= database->executeQuery(command); // Find the entries

// Populate data in the ElementList*
            if ( recipeFound.getStatus()!=QSQLiteResult::Failure ) {
	    QSQLiteResultRow row=recipeFound.first();
                while (!recipeFound.atEnd()) {
		    Element recipe;
		    recipe.id=row.data(0).toInt();
		    recipe.name=row.data(1);
		    results->add(recipe);
		    row=recipeFound.next();
                }
	}


}

void LiteRecipeDB::loadUnitRatios(UnitRatioList *ratioList)
{
ratioList->clear();

QString command;
command="SELECT unit1_id,unit2_id,ratio FROM units_conversion;";
QSQLiteResult ratiosToLoad=database->executeQuery(command);

            if (ratiosToLoad.getStatus()!=QSQLiteResult::Failure ) {
	    QSQLiteResultRow row=ratiosToLoad.first();
                while ( !ratiosToLoad.atEnd() ) {
		    UnitRatio ratio;
		    ratio.uID1=row.data(0).toInt();
		    ratio.uID2=row.data(1).toInt();
		    ratio.ratio=row.data(2).toDouble();
		    ratioList->add(ratio);
		    row=ratiosToLoad.next();
                }
	}
}

void LiteRecipeDB::saveUnitRatio(const UnitRatio *ratio)
{
QString command;

// Check if it's a new ratio or it exists already.
command=QString("SELECT * FROM units_conversion WHERE unit1_id=%1 AND unit2_id=%2;").arg(ratio->uID1).arg(ratio->uID2); // Find ratio between units

QSQLiteResult ratioFound=database->executeQuery(command); // Find the entries
bool newRatio=(ratioFound.size()==0);

if (newRatio)
	command=QString("INSERT INTO units_conversion VALUES(%1,%2,%3);").arg(ratio->uID1).arg(ratio->uID2).arg(ratio->ratio);
else
	command=QString("UPDATE units_conversion SET ratio=%3 WHERE unit1_id=%1 AND unit2_id=%2").arg(ratio->uID1).arg(ratio->uID2).arg(ratio->ratio);

database->executeQuery(command); // Enter the new ratio
}

double LiteRecipeDB::unitRatio(int unitID1, int unitID2)
{

if (unitID1==unitID2) return(1.0);
QString command;

command=QString("SELECT ratio FROM units_conversion WHERE unit1_id=%1 AND unit2_id=%2;").arg(unitID1).arg(unitID2);
QSQLiteResult ratioToLoad=database->executeQuery(command);


	    if (ratioToLoad.getStatus()!=QSQLiteResult::Failure)
	    {
	    if ( !ratioToLoad.atEnd() )
	    	{
	    	QSQLiteResultRow row=ratioToLoad.first();
	    	return(row.data(0).toDouble());
	    	}
		else return(-1);
            }
	    else return(-1);// There is no ratio defined between the units
}


//Finds data dependant on this Ingredient/Unit combination
void LiteRecipeDB::findIngredientUnitDependancies(int ingredientID,int unitID,ElementList *recipes,ElementList *ingredientInfo)
{

// Recipes using that combination

QString command=QString("SELECT DISTINCT r.id,r.title  FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1 AND il.unit_id=%2;").arg(ingredientID).arg(unitID);
QSQLiteResult unitToRemove=database->executeQuery(command);
loadElementList(recipes,&unitToRemove);
// Ingredient info using that combination
command=QString("SELECT i.name,ip.name,ip.units,u.name FROM ingredients i, ingredient_info ii, ingredient_properties ip, units u WHERE i.id=ii.ingredient_id AND ii.ingredient_id=%1 AND ii.per_units=%2 AND ii.property_id=ip.id AND ii.per_units=u.id;").arg(ingredientID).arg(unitID);

unitToRemove=database->executeQuery(command);
loadPropertyElementList(ingredientInfo,&unitToRemove);
}

void LiteRecipeDB::findIngredientDependancies(int ingredientID,ElementList *recipes)
{
QString command=QString("SELECT DISTINCT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1").arg(ingredientID);

QSQLiteResult ingredientToRemove=database->executeQuery(command);
loadElementList(recipes,&ingredientToRemove);
}



//Finds data dependant on the removal of this Unit
void LiteRecipeDB::findUnitDependancies(int unitID,ElementList *properties,ElementList *recipes)
{

// Ingredient-Info (ingredient->property) using this Unit

QString command=QString("SELECT i.name,ip.name,ip.units,u.name  FROM ingredients i, ingredient_info ii, ingredient_properties ip, units u WHERE i.id=ii.ingredient_id AND ii.per_units=%1 AND ii.property_id=ip.id  AND ii.per_units=u.id;").arg(unitID);
QSQLiteResult unitToRemove=database->executeQuery(command);
loadPropertyElementList(properties,&unitToRemove);

// Recipes using this Unit
command=QString("SELECT DISTINCT r.id,r.title  FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.unit_id=%1;").arg(unitID); // Without "DISTINCT" we get duplicates since ingredient_list has no unique recipe_id's
unitToRemove=database->executeQuery(command);
loadElementList(recipes,&unitToRemove);

}

void LiteRecipeDB::loadElementList(ElementList *elList, QSQLiteResult *query)
{
if ( query->getStatus()!=QSQLiteResult::Failure )
	{
	QSQLiteResultRow row =query->first();
	while ( !query->atEnd() )
		{
		    Element el;
		    el.id=row.data(0).toInt();
		    el.name=unescapeAndDecode(row.data(1));
		    elList->add(el); // Note that ElementList _copies_, does not take the pointer while adding.
		    row=query->next();

		}
	}
}

// See function "findUnitDependancies" for use
void LiteRecipeDB::loadPropertyElementList(ElementList *elList, QSQLiteResult *query)
{
if ( query->getStatus()!=QSQLiteResult::Failure ) {
	QSQLiteResultRow row=query->first();
                while ( !query->atEnd() ) {
		    Element el;
		    el.id=-1; // There's no ID for the ingredient-property combination
		    QString ingName=row.data(0);
		    QString propName=unescapeAndDecode(row.data(1));
		    QString propUnits=unescapeAndDecode(row.data(2));
		    QString propPerUnits=unescapeAndDecode(row.data(3));

		    el.name=QString("In ingredient %1: property \"%2\" [%3/%4]").arg(ingName).arg(propName).arg(propUnits).arg(propPerUnits);
		    elList->add(el); // Note that ElementList _copies_, does not take the pointer while adding.
                }
            }
}


QCString LiteRecipeDB::escapeAndEncode(const QString &s)
{
QString s_escaped;

// Escape
s_escaped=escape(s);

// Return encoded
return(s_escaped.utf8());
}

QString LiteRecipeDB::escape(const QString &s)
{
QString s_escaped=s;

// Escape using SQLite's function
char * escaped= sqlite_mprintf("%q",s.latin1()); // Escape the string(allocates memory)
s_escaped=escaped;
sqlite_freemem(escaped); // free allocated memory

return(s_escaped);
}

QString LiteRecipeDB::unescapeAndDecode(const QString &s)
{
QString s_escaped=QString::fromUtf8(s.latin1());
s_escaped.replace(";@",";");
return (s_escaped); // Use unicode encoding
}

bool LiteRecipeDB::ingredientContainsUnit(int ingredientID, int unitID)
{
QString command=QString("SELECT *  FROM unit_list WHERE ingredient_id= %1 AND unit_id=%2;").arg(ingredientID).arg(unitID);
QSQLiteResult recipeToLoad=database->executeQuery( command);
if (!recipeToLoad.getStatus()!=QSQLiteResult::Failure)
	{
	QSQLiteResultRow row=recipeToLoad.first();
	return(recipeToLoad.size()>0);
	}
return false;
}

bool LiteRecipeDB::ingredientContainsProperty(int ingredientID, int propertyID, int perUnitsID)
{
QString command=QString("SELECT *  FROM ingredient_info WHERE ingredient_id=%1 AND property_id=%2 AND per_units=%3;").arg(ingredientID).arg(propertyID).arg(perUnitsID);
QSQLiteResult recipeToLoad=database->executeQuery(command);

if (recipeToLoad.getStatus()!=QSQLiteResult::Failure)
{
	return(recipeToLoad.size()>0);
}
return false;
}

QString LiteRecipeDB::unitName(int unitID)
{
QString command=QString("SELECT * FROM units WHERE id=%1;").arg(unitID);
QSQLiteResult unitToLoad=database->executeQuery(command);
if (unitToLoad.getStatus()!=QSQLiteResult::Failure)
{
QSQLiteResultRow row=unitToLoad.first();
if (!unitToLoad.atEnd()) // Go to the first record (there should be only one anyway.
 return(row.data(0));
}

return(QString::null);
}

bool LiteRecipeDB::checkIntegrity(void)
{


// Check existence of the necessary tables (the database may be created, but empty)
QStringList tables; tables<<"ingredient_info"<<"ingredient_list"<<"ingredient_properties"<<"ingredients"<<"recipes"<<"unit_list"<<"units"<<"units_conversion"<<"categories"<<"category_list"<<"authors"<<"author_list"<<"db_info";

QString command=QString("SELECT name FROM sqlite_master WHERE type='table' UNION ALL SELECT name FROM sqlite_temp_master WHERE type='table';"); // Get the table names (equivalent to MySQL's "SHOW TABLES;" Easy to remember, right? ;)

QSQLiteResult tablesToCheck=database->executeQuery(command);
QStringList existingTableList;

// Get the existing list first

if (tablesToCheck.getStatus()!=QSQLiteResult::Failure)
{
QSQLiteResultRow row=tablesToCheck.first();
	while (!tablesToCheck.atEnd())
		{
		QString tableName=row.data(0);
		existingTableList<<tableName;
		row=tablesToCheck.next();
		}
}

else return(false);


for (QStringList::Iterator it = tables.begin(); it != tables.end(); ++it)
{
	bool found=false;

	for (QStringList::Iterator ex_it = existingTableList.begin(); ((ex_it != existingTableList.end()) && (!found)); ++ex_it)
	{
	found=(*ex_it==*it);
	}

	if (!found)
	{
	std::cerr<<"Recreating missing table: "<<*it<<"\n";
	createTable(*it);
	}
}

// Check for older versions, and port

std::cerr<<"Checking database version...\n";
float version=databaseVersion();
std::cerr<<"version found... "<<version<<" \n";
portOldDatabases(version);
return true;
}

void LiteRecipeDB::createTable(QString tableName)
{

QString command;
bool createIndex=false;

if (tableName=="recipes") command=QString("CREATE TABLE recipes (id INTEGER NOT NULL,title VARCHAR(%1),persons INTEGER,instructions TEXT, photo BLOB,   PRIMARY KEY (id));").arg(maxRecipeTitleLength());

else if (tableName=="ingredients") command=QString("CREATE TABLE ingredients (id INTEGER NOT NULL, name VARCHAR(%1), PRIMARY KEY (id));").arg(maxIngredientNameLength());

else if (tableName=="ingredient_list")
	{
	command="CREATE TABLE ingredient_list (recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, unit_id INTEGER, order_index INTEGER);";
	createIndex=true;
	}

else if (tableName=="unit_list") command="CREATE TABLE unit_list (ingredient_id INTEGER, unit_id INTEGER);";

else if (tableName== "units") command=QString("CREATE TABLE units (id INTEGER NOT NULL, name VARCHAR(%1), PRIMARY KEY (id));").arg(maxUnitNameLength());

else if  (tableName=="ingredient_info") command="CREATE TABLE ingredient_info (ingredient_id INTEGER, property_id INTEGER, amount FLOAT, per_units INTEGER);";

else if (tableName=="ingredient_properties") command="CREATE TABLE ingredient_properties (id INTEGER NOT NULL,name VARCHAR(20), units VARCHAR(20), PRIMARY KEY (id));";

else if (tableName=="units_conversion") command="CREATE TABLE units_conversion (unit1_id INTEGER, unit2_id INTEGER, ratio FLOAT);";

else if (tableName=="categories") command=QString("CREATE TABLE categories (id INTEGER NOT NULL, name varchar(%1) default NULL,PRIMARY KEY (id));").arg(maxCategoryNameLength());

else if (tableName=="category_list")
	{
	 command="CREATE TABLE category_list (recipe_id INTEGER NOT NULL,category_id INTEGER NOT NULL);";
	 createIndex=true;
	 }

else if (tableName=="authors") command=QString("CREATE TABLE authors (id INTEGER NOT NULL, name varchar(%1) default NULL,PRIMARY KEY (id));").arg(maxAuthorNameLength());

else if (tableName=="author_list") command="CREATE TABLE author_list (recipe_id INTEGER NOT NULL,author_id INTEGER NOT NULL);";

else if (tableName=="db_info") command="CREATE TABLE db_info (ver FLOAT NOT NULL,generated_by varchar(200) default NULL);";

else return;


database->executeQuery(command);

// Create necessary indexes

if (createIndex)
{
if (tableName=="category_list")
	{
	database->executeQuery("CREATE index rid_index ON category_list(recipe_id);");
	database->executeQuery("CREATE index cid_index ON category_list(category_id);");
	}

else if (tableName=="ingredient_list")
	{
	database->executeQuery("CREATE index ridil_index ON ingredient_list(recipe_id);");
	database->executeQuery("CREATE index iidil_index ON ingredient_list(ingredient_id);");
	}

}


}

void LiteRecipeDB::splitCommands(QString& s,QStringList& sl)
{
sl=QStringList::split(QRegExp(";{1}(?!@)"),s);
}

void LiteRecipeDB::portOldDatabases(float version)
{
// This is the first SQLite version (0.4). There's no need to upgrade anything

}

float LiteRecipeDB::databaseVersion(void)
{

QString command="SELECT ver FROM db_info;";
QSQLiteResult dbVersion=database->executeQuery(command);

if ( dbVersion.getStatus()!=QSQLiteResult::Failure ) {
QSQLiteResultRow row=dbVersion.first();
	if (!dbVersion.atEnd())
		return(row.data(0).toDouble());// There should be only one (or none for old DB) element, so go to first
	else return (0.4); // if table is empty, assume oldest (0.4), and port
}
else return(0.4); // By default go for oldest (0.4)
}

void LiteRecipeDB::loadCategories(ElementList *list)
{
list->clear();
QString command="SELECT * FROM categories ORDER BY name;";
QSQLiteResult categoryToLoad=database->executeQuery(command);
if (categoryToLoad.getStatus()!=QSQLiteResult::Failure) {
QSQLiteResultRow row=categoryToLoad.first();
	while (!categoryToLoad.atEnd())
	{
	Element el;
	el.id=row.data(0).toInt();
	el.name=unescapeAndDecode(row.data(1));
	list->add(el);
	row=categoryToLoad.next();
	}
}
}

void LiteRecipeDB::loadRecipeCategories(int recipeID, ElementList *list)
{
QString command=QString("SELECT c.id,c.name FROM categories c,category_list cl WHERE cl.recipe_id=%1 AND c.id=cl.category_id;").arg(recipeID);
QSQLiteResult categoryToLoad=database->executeQuery(command);
if (categoryToLoad.getStatus()!=QSQLiteResult::Failure) {
QSQLiteResultRow row=categoryToLoad.first();
	while (!categoryToLoad.atEnd())
	{
	Element el;
	el.id=row.data(0).toInt();
	el.name=unescapeAndDecode(row.data(1));
	list->add(el);
	row=categoryToLoad.next();
	}
}
}

void LiteRecipeDB::createNewCategory(QString &categoryName)
{
QString command;

command=QString("INSERT INTO categories VALUES(NULL,'%1');").arg(escapeAndEncode(categoryName));
database->executeQuery( command);
}

void LiteRecipeDB::modCategory(int categoryID, QString newLabel)
{
QString command;

command=QString("UPDATE categories SET name='%1' WHERE id=%2;").arg(escapeAndEncode(newLabel)).arg(categoryID);
database->executeQuery( command);
}

void LiteRecipeDB::removeCategory(int categoryID)
{
QString command;

command=QString("DELETE FROM categories WHERE id=%1;").arg(categoryID);
database->executeQuery(command);

command=QString("DELETE FROM category_list WHERE category_id=%1;").arg(categoryID);
database->executeQuery(command);
}

void LiteRecipeDB::addCategoryToRecipe(int recipeID, int categoryID)
{
QString command=QString("INSERT INTO categories_list VALUES(%1,%2);").arg(recipeID).arg(categoryID);
database->executeQuery(command);
}



void LiteRecipeDB::loadAuthors(ElementList *list)
{
list->clear();
QString command="SELECT * FROM authors;";
QSQLiteResult authorToLoad=database->executeQuery(command);
if (authorToLoad.getStatus()!=QSQLiteResult::Failure) {
QSQLiteResultRow row=authorToLoad.first();
	while (!authorToLoad.atEnd())
	{
	Element el;
	el.id=row.data(0).toInt();
	el.name=unescapeAndDecode(row.data(1));
	list->add(el);
	row=authorToLoad.next();
	}
}
}

void LiteRecipeDB::loadRecipeAuthors(int recipeID, ElementList *list)
{
list->clear();
QString command=QString("SELECT a.id,a.name FROM authors a, author_list al WHERE al.recipe_id=%1 AND a.id=al.author_id;").arg(recipeID);
QSQLiteResult authorToLoad=database->executeQuery(command);
if (authorToLoad.getStatus()!=QSQLiteResult::Failure) {
QSQLiteResultRow row=authorToLoad.first();
	while (!authorToLoad.atEnd())
	{
	Element el;
	el.id=row.data(0).toInt();
	el.name=unescapeAndDecode(row.data(1));
	list->add(el);
	row=authorToLoad.next();
	}
}
}

void LiteRecipeDB::createNewAuthor(const QString &authorName)
{
QString command;

command=QString("INSERT INTO authors VALUES(NULL,'%1');").arg(escapeAndEncode(authorName));
database->executeQuery(command);
}

void LiteRecipeDB::modAuthor(int authorID, QString newLabel)
{
QString command;

command=QString("UPDATE authors SET name='%1' WHERE id=%2;").arg(escapeAndEncode(newLabel)).arg(authorID);
database->executeQuery(command);
}

void LiteRecipeDB::removeAuthor(int authorID)
{
QString command;

command=QString("DELETE FROM authors WHERE id=%1;").arg(authorID);
database->executeQuery(command);
}

void LiteRecipeDB::addAuthorToRecipe(int recipeID, int authorID)
{
QString command=QString("INSERT INTO author_list VALUES(%1,%2);").arg(recipeID).arg(authorID);
database->executeQuery(command);
}


int LiteRecipeDB::findExistingUnitsByName(const QString& name,int ingredientID, ElementList *list)
{
QString command;
	if (ingredientID<0) // We're looking for units with that name all through the table, no specific ingredient
	{
	command=QString("SELECT id,name FROM units WHERE name='%1';").arg(name);
	}
	else // Look for units  with that name for the specified ingredient
	{
	command=QString("SELECT u.id,u.name FROM units u, unit_list ul WHERE u.id=ul.unit_id AND ul.ingredient_id=%1 AND u.name='%2';").arg(ingredientID).arg(name);
	}

	QSQLiteResult unitsToLoad=database->executeQuery(command); // Run the query

	if (list) //If the pointer exists, then load all the values found into it
	{
	if (unitsToLoad.getStatus()!=QSQLiteResult::Failure) {
	QSQLiteResultRow row=unitsToLoad.first();
		while (!unitsToLoad.atEnd())
		{
		Element el;
		el.id=row.data(0).toInt();
		el.name=unescapeAndDecode(row.data(1));
		list->add(el);
		row=unitsToLoad.next();
		}
	}
	}

	return(unitsToLoad.size());
}

int LiteRecipeDB::findExistingAuthorByName( const QString& name )
{
	QCString search_str = escapeAndEncode(name);
	search_str.truncate(maxAuthorNameLength()); //truncate to the maximum size db holds

	QString command=QString("SELECT id FROM authors WHERE name='%1';").arg(search_str);
	QSQLiteResult elementToLoad=database->executeQuery(command); // Run the query
	int id = -1;

	if (elementToLoad.getStatus()!=QSQLiteResult::Failure)
		{
		QSQLiteResultRow row=elementToLoad.first();
		if (!elementToLoad.atEnd())
			id=row.data(0).toInt();
		}

	return id;
}

int LiteRecipeDB::findExistingCategoryByName( const QString& name )
{
	QCString search_str = escapeAndEncode(name);
	search_str.truncate(maxCategoryNameLength()); //truncate to the maximum size db holds

	QString command=QString("SELECT id FROM categories WHERE name='%1';").arg(search_str);
	QSQLiteResult elementToLoad=database->executeQuery(command); // Run the query
	int id = -1;

	if (elementToLoad.getStatus()!=QSQLiteResult::Failure)
		{
		QSQLiteResultRow row=elementToLoad.first();
		if (!elementToLoad.atEnd())
			id=row.data(0).toInt();
		}

	return id;
}

int LiteRecipeDB::findExistingIngredientByName( const QString& name )
{
	QCString search_str = escapeAndEncode(name);
	search_str.truncate(maxIngredientNameLength()); //truncate to the maximum size db holds

	QString command=QString("SELECT id FROM ingredients WHERE name='%1';").arg(search_str);
	QSQLiteResult elementToLoad=database->executeQuery(command); // Run the query
	int id = -1;

	if (elementToLoad.getStatus()!=QSQLiteResult::Failure)
		{
		QSQLiteResultRow row=elementToLoad.first();
		if (!elementToLoad.atEnd())
			id=row.data(0).toInt();
		}

	return id;
}

int LiteRecipeDB::findExistingUnitByName( const QString& name )
{
	QCString search_str = escapeAndEncode(name);
	search_str.truncate(maxUnitNameLength()); //truncate to the maximum size db holds

	QString command=QString("SELECT id FROM units WHERE name='%1';").arg(search_str);
	QSQLiteResult elementToLoad=database->executeQuery(command); // Run the query
	int id = -1;

	if (elementToLoad.getStatus()!=QSQLiteResult::Failure)
		{
		QSQLiteResultRow row=elementToLoad.first();
		if (!elementToLoad.atEnd())
			id=row.data(0).toInt();
		}

	return id;
}

int LiteRecipeDB::findExistingRecipeByName( const QString& name )
{
	QCString search_str = escapeAndEncode(name);
	search_str.truncate(maxRecipeTitleLength()); //truncate to the maximum size db holds

	QString command=QString("SELECT id FROM recipes WHERE title='%1';").arg(search_str);
	QSQLiteResult elementToLoad=database->executeQuery(command); // Run the query
	int id = -1;

	if (elementToLoad.getStatus()!=QSQLiteResult::Failure)
	{
	 QSQLiteResultRow row= elementToLoad.first();
	 	if (!elementToLoad.atEnd())
			id=row.data(0).toInt();
	}
	return id;
}

void LiteRecipeDB::givePermissions(const QString & /*dbName*/,const QString &/*username*/, const QString &/*password*/, const QString &/*clientHost*/)
{
// FIXME: you can't give permissions in SQLite :)
/*QString command;

if ((password!="")&&(password!=QString::null)) command=QString("GRANT ALL ON %1.* TO %2@%3 IDENTIFIED BY '%4';").arg(dbName).arg(username).arg(clientHost).arg(password);
else command=QString("GRANT ALL ON %1.* TO %2@%3;").arg(dbName).arg(username).arg(clientHost);

std::cerr<<"I'm doing the query to setup permissions\n";

QSqlQuery permissionsToSet( command,database);*/
}
QString LiteRecipeDB::getUniqueRecipeTitle( const QString &recipe_title )
{
	//already is unique
	if ( findExistingRecipeByName( recipe_title ) == -1 )
		return recipe_title;

	QString return_title=recipe_title; //If any error is produced, just go for default value (always return something)

	QString command = QString( "SELECT COUNT(DISTINCT title) FROM recipes WHERE title LIKE '%1 (%)';" ).arg(escapeAndEncode(recipe_title));

	QSQLiteResult alikeRecipes=database->executeQuery( command);
	if (alikeRecipes.getStatus()!=QSQLiteResult::Failure)
	{
	QSQLiteResultRow row =alikeRecipes.first();
	int count=0;
	if (!alikeRecipes.atEnd());
		{
		count = row.data(0).toInt();
		return_title = QString("%1 (%2)").arg(recipe_title).arg(count+2);

		//make sure this newly created title is unique (just in case)
		while ( findExistingRecipeByName( return_title ) != -1 )
		{
		count--; //go down to find the skipped recipe(s)
		return_title = QString("%1 (%2)").arg(recipe_title).arg(count+2);
		}
		}
	}

	return return_title;
}

QString LiteRecipeDB::recipeTitle(int recipeID)
{
QString command=QString("SELECT * FROM recipes WHERE id=%1;").arg(recipeID);
QSQLiteResult recipeToLoad=database->executeQuery(command);
if (recipeToLoad.getStatus()!=QSQLiteResult::Failure)
{
QSQLiteResultRow row=recipeToLoad.first();
if(!recipeToLoad.atEnd()) // Go to the first record (there should be only one anyway.
 return(row.data(1));
}
return(QString::null);
}

int LiteRecipeDB::lastInsertID()
{
	int lastID; QSQLiteResult lastInsertID=database->executeQuery("SELECT name FROM recipes;",&lastID); // Execute whatever query that doesn't insert rows

	return lastID;
}

void LiteRecipeDB::emptyData(void)
{
QStringList tables; tables<<"ingredient_info"<<"ingredient_list"<<"ingredient_properties"<<"ingredients"<<"recipes"<<"unit_list"<<"units"<<"units_conversion"<<"categories"<<"category_list"<<"authors"<<"author_list";

for (QStringList::Iterator it = tables.begin(); it != tables.end(); ++it)
	{
	QString command=QString("DELETE FROM %1;").arg(*it);
	database->executeQuery(command);
	}
}

/*
** Encode a binary buffer "in" of size n bytes so that it contains
** no instances of characters '\'' or '\000'.  The output is
** null-terminated and can be used as a string value in an INSERT
** or UPDATE statement.  Use sqlite_decode_binary() to convert the
** string back into its original binary.
**
** The result is written into a preallocated output buffer "out".
** "out" must be able to hold at least 2 +(257*n)/254 bytes.
** In other words, the output will be expanded by as much as 3
** bytes for every 254 bytes of input plus 2 bytes of fixed overhead.
** (This is approximately 2 + 1.0118*n or about a 1.2% size increase.)
**
** The return value is the number of characters in the encoded
** string, excluding the "\000" terminator.
*/

int LiteRecipeDB::sqlite_encode_binary(const unsigned char *in, int n, unsigned char *out){
  int i, j, e, m;
  int cnt[256];
  if( n<=0 ){
    out[0] = 'x';
    out[1] = 0;
    return 1;
  }
  memset(cnt, 0, sizeof(cnt));
  for(i=n-1; i>=0; i--){ cnt[in[i]]++; }
  m = n;
  for(i=1; i<256; i++){
    int sum;
    if( i=='\'' ) continue;
    sum = cnt[i] + cnt[(i+1)&0xff] + cnt[(i+'\'')&0xff];
    if( sum<m ){
      m = sum;
      e = i;
      if( m==0 ) break;
    }
  }
  out[0] = e;
  j = 1;
  for(i=0; i<n; i++){
    int c = (in[i] - e)&0xff;
    if( c==0 ){
      out[j++] = 1;
      out[j++] = 1;
    }else if( c==1 ){
      out[j++] = 1;
      out[j++] = 2;
    }else if( c=='\'' ){
      out[j++] = 1;
      out[j++] = 3;
    }else{
      out[j++] = c;
    }
  }
  out[j] = 0;
  return j;
}

/*
** Decode the string "in" into binary data and write it into "out".
** This routine reverses the encoding created by sqlite_encode_binary().
** The output will always be a few bytes less than the input.  The number
** of bytes of output is returned.  If the input is not a well-formed
** encoding, -1 is returned.
**
** The "in" and "out" parameters may point to the same buffer in order
** to decode a string in place.
*/
int LiteRecipeDB::sqlite_decode_binary(const unsigned char *in, unsigned char *out){
  int i, c, e;
  e = *(in++);
  i = 0;
  while( (c = *(in++))!=0 ){
    if( c==1 ){
      c = *(in++);
      if( c==1 ){
        c = 0;
      }else if( c==2 ){
        c = 1;
      }else if( c==3 ){
        c = '\'';
      }else{
        return -1;
      }
    }
    out[i++] = (c + e)&0xff;
  }
  return i;
}

