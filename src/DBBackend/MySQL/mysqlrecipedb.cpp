/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "mysqlrecipedb.h"

#include <kstandarddirs.h>


MySQLRecipeDB::MySQLRecipeDB(QString host, QString user, QString pass, QString DBname,bool init):RecipeDB(host, user,pass,DBname,init)
{
	DBuser=user;DBpass=pass;DBhost=host;

        database= QSqlDatabase::addDatabase( DB_DRIVER );
        database->setDatabaseName(DBname);
        if (!(DBuser==QString::null)) database->setUserName(DBuser );
        if (!(DBpass==QString::null)) database->setPassword(DBpass);
        database->setHostName(DBhost);
        if ( !database->open() ) {
	     //Try to create the database
	     createDB();

	     //Now Reopen the Database and exit if it fails
	     if (!database->open())
		{
		std::cerr<<QString("Could not open DB as user: %1. You may not have permissions. Exiting.\n").arg(user).latin1();
		exit(1);
		}

	     // Initialize database if requested
	      if (init) initializeDB();
	     }
	 else // Check integrity of the database (tables). If not possible, exit
	 {
	 	if (!checkIntegrity())
			{
			std::cerr<<"Failed to fix database structure. Exiting.\n";
			 exit(1);
			 }
	 }
}
MySQLRecipeDB::~MySQLRecipeDB()
{
database->close();
}

void MySQLRecipeDB::createDB()
{

MYSQL* mysqlDB;
mysqlDB=mysql_init(NULL);
mysql_real_connect(mysqlDB,DBhost.latin1(),DBuser.latin1(),DBpass.latin1(),NULL,0,NULL,0);

QString command;

// Create the Database (Note: needs permissions)
command=QString("CREATE DATABASE %1;").arg(database->databaseName());
mysql_query(mysqlDB,command.latin1());

// Close the Database
mysql_close(mysqlDB);

}

void MySQLRecipeDB::loadAllRecipeIngredients(RecipeIngredientList *list, bool withNames)
{
list->ilist.clear();
list->recipeIdList.clear();

QString command;

if (withNames)
{
command=QString("SELECT il.ingredient_id,i.name,il.amount,u.id,u.name,il.recipe_id FROM ingredients i, ingredient_list il, units u WHERE il.recipe_id=%1 AND i.id=il.ingredient_id AND u.id=il.unit_id ORDER BY il.order_index;");
}
else
{
command=QString("SELECT ingredient_id,amount,unit_id,recipe_id  FROM ingredient_list;");
}

QSqlQuery ingredientsToLoad(command,database);
            if ( ingredientsToLoad.isActive() ) {
                while ( ingredientsToLoad.next() ) {
		    Ingredient ing;
		    if (withNames)
		    {
		    ing.ingredientID=ingredientsToLoad.value(0).toInt();
		    ing.name=unescapeAndDecode(ingredientsToLoad.value(1).toString());
		    ing.amount=ingredientsToLoad.value(2).toDouble();
		    ing.unitID=ingredientsToLoad.value(3).toInt();
		    ing.units=unescapeAndDecode(ingredientsToLoad.value(4).toString());
		    list->recipeIdList.append(ingredientsToLoad.value(5).toInt());
		    }
		    else
		    {
		    ing.ingredientID=ingredientsToLoad.value(0).toInt();
		    ing.amount=ingredientsToLoad.value(1).toDouble();
		    ing.unitID=ingredientsToLoad.value(2).toInt();
		    list->recipeIdList.append(ingredientsToLoad.value(3).toInt());

		    }
		    list->ilist.add(ing);
                }
            }
}

void MySQLRecipeDB::loadRecipe(Recipe *recipe,int recipeID)
{

// Empty the recipe first
recipe->empty();

QString command;

// Read title, author and instructions
command=QString("SELECT title,instructions,persons FROM recipes WHERE id=%1;").arg(recipeID);

QSqlQuery recipeToLoad( command,database);
if (recipeToLoad.isActive())
{
	recipeToLoad.first(); // Moves to the first record (it should be unique)
	recipe->title=unescapeAndDecode(recipeToLoad.value(0).toString());
	recipe->instructions=unescapeAndDecode(recipeToLoad.value(1).toString());
	recipe->persons=recipeToLoad.value(2).toInt();
	recipe->recipeID=recipeID;
}

// Read the ingredients
command=QString("SELECT il.ingredient_id,i.name,il.amount,u.id,u.name FROM ingredients i, ingredient_list il, units u WHERE il.recipe_id=%1 AND i.id=il.ingredient_id AND u.id=il.unit_id ORDER BY il.order_index;").arg(recipeID);

recipeToLoad.exec( command);
            if ( recipeToLoad.isActive() ) {
                while ( recipeToLoad.next() ) {
		    Ingredient ing;
		    ing.ingredientID=recipeToLoad.value(0).toInt();
		    ing.name=unescapeAndDecode(recipeToLoad.value(1).toString());
		    ing.amount=recipeToLoad.value(2).toDouble();
		    ing.unitID=recipeToLoad.value(3).toInt();
		    ing.units=unescapeAndDecode(recipeToLoad.value(4).toString());

		    recipe->ingList.add(ing);
                }
            }

//Load the Image (Using mysql, no qsql here due to problems that I could have with BLOB)
// (I'll try later with qsql)

MYSQL* mysqlDB;
mysqlDB=mysql_init(NULL);
mysql_real_connect(mysqlDB,DBhost.latin1(),DBuser.latin1(),DBpass.latin1(),NULL,0,NULL,0);

command=QString("USE %1").arg(database->databaseName());

int size = mysql_real_query(mysqlDB, command.latin1() , command.length()+1);
command=QString("SELECT photo FROM recipes WHERE id=%1").arg(recipeID); //Be careful: No semicolon here at the end


size = mysql_real_query(mysqlDB, command.latin1() , command.length()+1);

MYSQL_RES *res_set = mysql_store_result (mysqlDB);
MYSQL_ROW row;
if((row = mysql_fetch_row (res_set)) != NULL) {
   unsigned long *lengths = mysql_fetch_lengths( res_set );
   if(row[0])
     recipe->photo.loadFromData( (unsigned char *)row[0], lengths[0] );
     // picture will now have a ready-to-use image
     }


//Load the category list
command=QString("SELECT cl.category_id,c.name FROM category_list cl, categories c WHERE recipe_id=%1 AND cl.category_id=c.id;").arg(recipeID);

recipeToLoad.exec( command);
            if ( recipeToLoad.isActive() ) {
                while ( recipeToLoad.next() ) {
		    Element el;
		    el.id=recipeToLoad.value(0).toInt();
		    el.name=unescapeAndDecode(recipeToLoad.value(1).toString());
		    if (el.id!=-1) recipe->categoryList.add(el); // add to list except for default category (-1)
                }
            }

//Load the author list
command=QString("SELECT al.author_id,a.name FROM author_list al, authors a WHERE recipe_id=%1 AND al.author_id=a.id;").arg(recipeID);

recipeToLoad.exec( command);
            if ( recipeToLoad.isActive() ) {
                while ( recipeToLoad.next() ) {
		    Element el;
		    el.id=recipeToLoad.value(0).toInt();
		    el.name=unescapeAndDecode(recipeToLoad.value(1).toString());
		    recipe->authorList.add(el);
                }
            }

mysql_close(mysqlDB);
}

/*
Loads a recipe detail list (no instructions, no photo, no ingredients)
*/

void MySQLRecipeDB::loadRecipeDetails(RecipeList *rlist,bool loadIngredients,bool loadCategories)
{
rlist->clear();

QMap <int,RecipeList::Iterator> recipeIterators; // Stores the iterator of each recipe in the list;

QString command="SELECT id,title,persons FROM recipes";
QSqlQuery recipesToLoad( command,database);

            if ( recipesToLoad.isActive() ) {
                while ( recipesToLoad.next() ) {
		    Recipe rec;
		    rec.recipeID=recipesToLoad.value(0).toInt();
		    rec.title=unescapeAndDecode(recipesToLoad.value(1).toString());
		    rec.persons=recipesToLoad.value(2).toInt();
		    RecipeList::Iterator it=rlist->append(rec);
		    recipeIterators[rec.recipeID]=it;
                }
	}

if (loadIngredients) // Note that names of ingredients, units....are not loaded just the needed id's
{
command=QString("SELECT ingredient_id,amount,unit_id,recipe_id FROM ingredient_list;" );

QSqlQuery ingredientsToLoad(command,database);

	    if (ingredientsToLoad.isActive()) {
                while ( ingredientsToLoad.next() ) {
		    Ingredient ing;

		    // get this ingredient
		    ing.ingredientID=ingredientsToLoad.value(0).toInt();
		    ing.amount=ingredientsToLoad.value(1).toDouble();
		    ing.unitID=ingredientsToLoad.value(2).toInt();

		    // find the corresponding recipe iterator
		    if (recipeIterators.contains(ingredientsToLoad.value(3).toInt()))
		    {
		    RecipeList::Iterator it=recipeIterators[ingredientsToLoad.value(3).toInt()];

		    //add the ingredient to the recipe
		    (*it).ingList.add(ing);

		    }
                }
            }


}

if (loadCategories)
{
command=QString("SELECT recipe_id,category_id FROM category_list;" ); // Note that we get no names, and the -1 default categories will leak here too.

QSqlQuery categoriesToLoad(command,database);

	    if (categoriesToLoad.isActive()) {
                while ( categoriesToLoad.next() ) {
		    Element cty;

		    // get this category
		    cty.id=categoriesToLoad.value(1).toInt();

		    // find the corresponding recipe iterator
		    if (recipeIterators.contains(categoriesToLoad.value(0).toInt()))
		    {
		    RecipeList::Iterator it=recipeIterators[categoriesToLoad.value(0).toInt()];

		    //add the ingredient to the recipe
		    (*it).categoryList.add(cty);

		    }
                }
            }




}

}


void MySQLRecipeDB::loadIngredients(ElementList *list)
{
list->clear();

QString command;
command="SELECT id,name FROM ingredients ORDER BY name;";
QSqlQuery ingredientToLoad( command,database);

            if ( ingredientToLoad.isActive() ) {
                while ( ingredientToLoad.next() ) {
		    Element ing;
		    ing.id=ingredientToLoad.value(0).toInt();
		    ing.name=unescapeAndDecode(ingredientToLoad.value(1).toString());
		    list->add(ing);
                }
	}



}

void MySQLRecipeDB::loadPossibleUnits(int ingredientID, ElementList *list)
{
list->clear();

QString command;

command=QString("SELECT u.id,u.name FROM unit_list ul, units u WHERE ul.ingredient_id=%1 AND ul.unit_id=u.id;").arg(ingredientID);

QSqlQuery unitToLoad( command,database);

            if ( unitToLoad.isActive() ) {
                while ( unitToLoad.next() ) {
		    Element unit;
		    unit.id=unitToLoad.value(0).toInt();
		    unit.name=unescapeAndDecode(unitToLoad.value(1).toString());
		    list->add(unit);
                }
	}


}
void MySQLRecipeDB::saveRecipe(Recipe *recipe)
{

// Check if it's a new recipe or it exists (supossedly) already.

bool newRecipe; newRecipe=(recipe->recipeID==-1);
// First check if the recipe ID is set, if so, update (not create)
// Be carefull, first check if the recipe hasn't been deleted while changing.

MYSQL* mysqlDB;
mysqlDB=mysql_init(NULL);
mysql_real_connect(mysqlDB,DBhost.latin1(),DBuser.latin1(),DBpass.latin1(),NULL,0,NULL,0);

QString command;

command=QString("USE %1").arg(database->databaseName());
int size = mysql_real_query(mysqlDB, command.latin1() , command.length()+1);

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
size = mysql_real_query(mysqlDB, command.latin1() , command.length()+1);

// If it's a new recipe, identify the ID that was given to the recipe and store in the Recipe itself
int recipeID;
if (newRecipe)
{
	recipeID=mysql_insert_id(mysqlDB);
	recipe->recipeID=recipeID;
}
recipeID=recipe->recipeID;

// Let's begin storing the Image!

recipe->photo.save(".krecipe_photo.jpg", "JPEG");
QFileInfo fi(".krecipe_photo.jpg");

 char *from, *to, *p;
 if((from = (char *)malloc( fi.size()+1024 )) != NULL) {
   if((to = (char *)malloc( (fi.size()*2)+2048 )) != NULL) {
     memset(to, 0, (fi.size()*2)+2048);
     QFile f(".krecipe_photo.jpg");
     if(f.open( IO_ReadOnly )) {
       f.readBlock( from, fi.size() );
         f.close();
         strcpy(to, "UPDATE recipes SET photo='");
       p = &to[strlen(to)];
       mysql_real_escape_string( mysqlDB, p, from, fi.size() );
       strcat(to, "'");
       QString st; st=to; st+= " WHERE id="; st+=QString::number(recipeID); // Note, we assume this ID exists (created or existing)!
         size=mysql_real_query( mysqlDB, st.latin1(), st.length()+1);

     }
     free(to);
   }
   free(from);
 }
 //_unlink(".krecipe_photo.jpg");


 // Save the ingredient list (first delete if we are updating)

 command=QString("DELETE FROM ingredient_list WHERE recipe_id=%1;")
 	.arg(recipeID);
 size = mysql_real_query(mysqlDB, command.latin1() , command.length()+1);

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
	size = mysql_real_query(mysqlDB, command.latin1() , command.length()+1);

}

// Save the category list for the recipe (first delete, in case we are updating)
command=QString("DELETE FROM category_list WHERE recipe_id=%1;")
	.arg(recipeID);
size=mysql_real_query(mysqlDB,command.latin1(), command.length()+1);

for (Element *cat=recipe->categoryList.getLast(); cat; cat=recipe->categoryList.getPrev()) // Start from last, mysql seems to work in lifo format... so it's read first the latest inserted one (newest)
	{
	command=QString("INSERT INTO category_list VALUES (%1,%2);")
	.arg(recipeID)
	.arg(cat->id);
	size = mysql_real_query(mysqlDB, command.latin1() , command.length()+1);
}

//Add the default category -1 to ease and speed up searches

command=QString("INSERT INTO category_list VALUES (%1,-1);")
	.arg(recipeID);
size = mysql_real_query(mysqlDB, command.latin1() , command.length()+1);


// Save the author list for the recipe (first delete, in case we are updating)
command=QString("DELETE FROM author_list WHERE recipe_id=%1;")
	.arg(recipeID);
size=mysql_real_query(mysqlDB,command.latin1(), command.length()+1);

for (Element *author=recipe->authorList.getLast(); author; author=recipe->authorList.getPrev()) // Start from last, mysql seems to work in lifo format... so it's read first the latest inserted one (newest)
	{
	command=QString("INSERT INTO author_list VALUES (%1,%2);")
	.arg(recipeID)
	.arg(author->id);
	size = mysql_real_query(mysqlDB, command.latin1() , command.length()+1);
}



mysql_close(mysqlDB);
}

void MySQLRecipeDB::loadRecipeList(ElementList *list,int categoryID,QPtrList <int>*recipeCategoryList)
{
list->clear();

QString command;



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


QSqlQuery recipeToLoad( command,database);

            if ( recipeToLoad.isActive() ) {
                while ( recipeToLoad.next() ) {
		    Element recipe;
		    recipe.id=recipeToLoad.value(0).toInt();
		    recipe.name=unescapeAndDecode(recipeToLoad.value(1).toString());
		    list->add(recipe);

		    if (recipeCategoryList)
		    {
		    int *category=new int;
		    *category=recipeToLoad.value(2).toInt();
		    recipeCategoryList->append (category);
 		    }
                }
	}


}


void MySQLRecipeDB::removeRecipe(int id)
{
QString command;

command=QString("DELETE FROM recipes WHERE id=%1;").arg(id);
QSqlQuery recipeToRemove( command,database);
command=QString("DELETE FROM ingredient_list WHERE recipe_id=%1;").arg(id);
recipeToRemove.exec( command);
command=QString("DELETE FROM category_list WHERE recipe_id=%1;").arg(id);
recipeToRemove.exec( command);

}

void MySQLRecipeDB::removeRecipeFromCategory(int ingredientID, int categoryID){
QString command;
command=QString("DELETE FROM category_list WHERE recipe_id=%1 AND category_id=%2;").arg(ingredientID).arg(categoryID);
QSqlQuery recipeToRemove( command,database);
}

void MySQLRecipeDB::createNewIngredient(QString ingredientName)
{
QString command;

command=QString("INSERT INTO ingredients VALUES(NULL,'%1');").arg(escapeAndEncode(ingredientName));
QSqlQuery ingredientToCreate( command,database);
}

void MySQLRecipeDB::modIngredient(int ingredientID, QString newLabel)
{
QString command;

command=QString("UPDATE ingredients SET name='%1' WHERE id=%2;").arg(escapeAndEncode(newLabel)).arg(ingredientID);
QSqlQuery ingredientToCreate( command,database);
}

void MySQLRecipeDB::addUnitToIngredient(int ingredientID,int unitID)
{
QString command;

command=QString("INSERT INTO unit_list VALUES(%1,%2);").arg(ingredientID).arg(unitID);
QSqlQuery ingredientToCreate( command,database);
}

void MySQLRecipeDB::loadUnits(ElementList *list)
{
list->clear();

QString command;

command="SELECT id,name FROM units;";

QSqlQuery unitToLoad( command,database);

            if ( unitToLoad.isActive() ) {
                while ( unitToLoad.next() ) {
		    Element unit;
		    unit.id=unitToLoad.value(0).toInt();
		    unit.name=unescapeAndDecode(unitToLoad.value(1).toString());
		    list->add(unit);
                }
	}


}

void MySQLRecipeDB::removeUnitFromIngredient(int ingredientID, int unitID)
{
QString command;

command=QString("DELETE FROM unit_list WHERE ingredient_id=%1 AND unit_id=%2;").arg(ingredientID).arg(unitID);
QSqlQuery unitToRemove( command,database);

// Remove any recipe using that combination of ingredients also (user must have been warned before calling this function!)

command=QString("DELETE recipes.*  FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1 AND il.unit_id=%2;").arg(ingredientID).arg(unitID);
unitToRemove.exec(command);

// Remove any ingredient properties from ingredient_info where the this ingredient+unit is being used (user must have been warned before calling this function!)
command=QString("DELETE ingredient_info.* FROM ingredient_info ii WHERE ii.ingredient_id=%1 AND ii.per_units=%2;").arg(ingredientID).arg(unitID);
unitToRemove.exec(command);
// Clean up ingredient_list which have no recipe that they belong to. (I almost don't know how, but this seems to work ;-) Tested using MySQL 4.0.11a
command=QString("DELETE ingredient_list.* FROM ingredient_list LEFT JOIN recipes ON ingredient_list.recipe_id=recipes.id WHERE recipes.id IS NULL;");
unitToRemove.exec( command);
// Clean up category_list which have no recipe that they belong to. Same method as above
command=QString("DELETE category_list.* FROM category_list LEFT JOIN recipes ON category_list.recipe_id=recipes.id WHERE recipes.id IS NULL;");
unitToRemove.exec( command);
}


void MySQLRecipeDB::findUseOf_Ing_Unit_InRecipes(ElementList *results, int ingredientID, int unitID)
{
 QString command=QString("SELECT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1 AND il.unit_id=%2").arg(ingredientID).arg(unitID);
QSqlQuery recipeFound( command,database); // Find the entries

// Populate data in the ElementList*
            if ( recipeFound.isActive() ) {
                while ( recipeFound.next() ) {
		    Element recipe;
		    recipe.id=recipeFound.value(0).toInt();
		    recipe.name=unescapeAndDecode(recipeFound.value(1).toString());
		    results->add(recipe);
		}
	}
}

void MySQLRecipeDB::findUseOfIngInRecipes(ElementList *results,int ingredientID)
{
 QString command=QString("SELECT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1").arg(ingredientID);
QSqlQuery recipeFound( command,database); // Find the entries

// Populate data in the ElementList*
            if ( recipeFound.isActive() ) {
                while ( recipeFound.next() ) {
		    Element recipe;
		    recipe.id=recipeFound.value(0).toInt();
		    recipe.name=unescapeAndDecode(recipeFound.value(1).toString());
		    results->add(recipe);
                }
	}
}

void MySQLRecipeDB::removeIngredient(int ingredientID)
{
QString command;

// First remove the ingredient

command=QString("DELETE ingredients.* FROM ingredients WHERE id=%1;").arg(ingredientID);
QSqlQuery ingredientToDelete(command,database);

// Remove all the unit entries for this ingredient

command=QString("DELETE unit_list.* FROM unit_list WHERE ingredient_id=%1;").arg(ingredientID);
ingredientToDelete.exec(command);

// Remove any recipe using that ingredient

command=QString("DELETE recipes.*  FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1;").arg(ingredientID);
ingredientToDelete.exec(command);

// Remove any ingredient in ingredient_list whis has references to inexisting recipes. (As said above, I almost don't know how, but this seems to work ;-) Tested using MySQL 4.0.11a
command=QString("DELETE ingredient_list.* FROM ingredient_list LEFT JOIN recipes ON ingredient_list.recipe_id=recipes.id WHERE recipes.id IS NULL;");
ingredientToDelete.exec( command );

// Clean up category_list which have no recipe that they belong to. Same method as above
command=QString("DELETE category_list.* FROM category_list LEFT JOIN recipes ON category_list.recipe_id=recipes.id WHERE recipes.id IS NULL;");
ingredientToDelete.exec( command);

// Remove property list of this ingredient
command=QString("DELETE FROM ingredient_info WHERE ingredient_id=%1;").arg(ingredientID);
ingredientToDelete.exec(command);
}

void MySQLRecipeDB::initializeDB(void)
{
// Select the database
QString command=QString("USE %1;").arg(database->databaseName());
QSqlQuery initializeQuery(command,database);

// Create the table structure

	// Read the commands form the structure file
	QString commands;
	QFile file (KGlobal::dirs()->findResource("appdata", "data/mysqlstructure.sql"));
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
	 initializeQuery.exec((*it)+QString(";")); //Split removes the semicolons
	 }



}

void MySQLRecipeDB::initializeData(void)
{

// Select the database
QString command=QString("USE %1;").arg(database->databaseName());
QSqlQuery initializeQuery(command,database);

//

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
	 initializeQuery.exec((*it)+QString(";")); //Split removes the semicolons
	 }

}

void MySQLRecipeDB::addProperty(QString &name, QString &units)
{
QString command;
command=QString("INSERT INTO ingredient_properties VALUES(NULL,'%1','%2');").arg(escapeAndEncode(name)).arg(escapeAndEncode(units));
QSqlQuery propertyToAdd(command,database);
}

void MySQLRecipeDB::loadProperties(IngredientPropertyList *list,int ingredientID)
{
list->clear();
QString command;
bool usePerUnit;
if (ingredientID>=0) // Load properties of this ingredient
{
usePerUnit=true;
command=QString("SELECT ip.id,ip.name,ip.units,ii.per_units,u.name,ii.amount,ii.ingredient_id  FROM ingredient_properties ip, ingredient_info ii, units u WHERE ii.ingredient_id=%1 AND ii.property_id=ip.id AND ii.per_units=u.id;").arg(ingredientID);
}
else if (ingredientID==-1) // Load the properties of all the ingredients
{
usePerUnit=true;
command=QString("SELECT ip.id,ip.name,ip.units,ii.per_units,u.name,ii.amount,ii.ingredient_id FROM ingredient_properties ip, ingredient_info ii, units u WHERE ii.property_id=ip.id AND ii.per_units=u.id;").arg(ingredientID);
}
else // Load the whole property list (just the list of possible properties, not the ingredient properties)
{
usePerUnit=false;
command=QString("SELECT  id,name,units FROM ingredient_properties;");
}

QSqlQuery propertiesToLoad (command,database);
// Load the results into the list
if ( propertiesToLoad.isActive() ) {
                while ( propertiesToLoad.next() ) {
		    IngredientProperty prop;
		    prop.id=propertiesToLoad.value(0).toInt();
		    prop.name=unescapeAndDecode(propertiesToLoad.value(1).toString());
		    prop.units=unescapeAndDecode(propertiesToLoad.value(2).toString());
		    if (usePerUnit)
		    {
		    prop.perUnit.id=propertiesToLoad.value(3).toInt();
		    prop.perUnit.name=unescapeAndDecode(propertiesToLoad.value(4).toString());
		    }
		    else
		    {
		    prop.perUnit.id=-1;
		    prop.perUnit.name=QString::null;
		    }

		    if (ingredientID>=0)
		      prop.amount=propertiesToLoad.value(5).toDouble();
		    else
		      prop.amount=-1; // Property is generic, not attached to an ingredient

		    if (ingredientID>=-1) prop.ingredientID=propertiesToLoad.value(6).toInt();

		    list->add(prop);
                }

            }
}

void MySQLRecipeDB::changePropertyAmountToIngredient(int ingredientID,int propertyID,double amount,int per_units)
{
QString command;
command=QString("UPDATE ingredient_info SET amount=%1 WHERE ingredient_id=%2 AND property_id=%3 AND per_units=%4;").arg(amount).arg(ingredientID).arg(propertyID).arg(per_units);
QSqlQuery infoToChange(command,database);
}

void MySQLRecipeDB::addPropertyToIngredient(int ingredientID,int propertyID,double amount, int perUnitsID)
{
QString command;

command=QString("INSERT INTO ingredient_info VALUES(%1,%2,%3,%4);").arg(ingredientID).arg(propertyID).arg(amount).arg(perUnitsID);
QSqlQuery propertyToAdd( command,database);
}


void MySQLRecipeDB::removePropertyFromIngredient(int ingredientID, int propertyID, int perUnitID)
{
QString command;
// remove property from ingredient info. Note that there could be duplicates with different units (per_units). Remove just the one especified.
command=QString("DELETE FROM ingredient_info WHERE ingredient_id=%1 AND property_id=%2 AND per_units=%3;").arg(ingredientID).arg(propertyID).arg(perUnitID);
QSqlQuery propertyToRemove( command,database);
}

void MySQLRecipeDB::removeProperty(int propertyID)
{
QString command;

// Remove property from the ingredient_properties
command=QString("DELETE FROM ingredient_properties WHERE id=%1;").arg(propertyID);
QSqlQuery propertyToRemove( command,database);

// Remove any ingredient info that uses this property
command=QString("DELETE FROM ingredient_info WHERE property_id=%1;").arg(propertyID);
propertyToRemove.exec( command);

}

void MySQLRecipeDB::removeUnit(int unitID)
{
QString command;
// Remove the unit first
command=QString("DELETE FROM units WHERE id=%1;").arg(unitID);
QSqlQuery unitToRemove( command,database);

//Remove the unit from ingredients using it

command=QString("DELETE FROM unit_list WHERE unit_id=%1;").arg(unitID);
unitToRemove.exec(command);


// Remove any recipe using that unit in the ingredient list (user must have been warned before calling this function!)

command=QString("DELETE recipes.*  FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.unit_id=%1;").arg(unitID);
unitToRemove.exec(command);

// Remove any ingredient in ingredient_list whis has references to inexisting recipes. (As said above, I almost don't know how, but this seems to work ;-) Tested using MySQL 4.0.11a
command=QString("DELETE ingredient_list.* FROM ingredient_list LEFT JOIN recipes ON ingredient_list.recipe_id=recipes.id WHERE recipes.id IS NULL;");
unitToRemove.exec( command );

// Clean up category_list which have no recipe that they belong to. Same method as above
command=QString("DELETE category_list.* FROM category_list LEFT JOIN recipes ON category_list.recipe_id=recipes.id WHERE recipes.id IS NULL;");
unitToRemove.exec( command);

// Remove the ingredient properties using this unit (user must be warned before calling this function)
command=QString("DELETE FROM ingredient_info WHERE per_units=%1;").arg(unitID);
unitToRemove.exec(command);

// Remove the unit conversion ratios with this unit
command=QString("DELETE FROM units_conversion WHERE unit1_id=%1 OR unit2_id=%2;").arg(unitID).arg(unitID);
unitToRemove.exec(command);

}


void MySQLRecipeDB::createNewUnit(QString &unitName)
{
QString command;

command=QString("INSERT INTO units VALUES(NULL,'%1');").arg(escapeAndEncode(unitName));
QSqlQuery unitToCreate( command,database);
}


void MySQLRecipeDB::modUnit(int unitID, QString newLabel)
{
QString command;

command=QString("UPDATE units SET name='%1' WHERE id=%2;").arg(escapeAndEncode(newLabel)).arg(unitID);
QSqlQuery ingredientToCreate( command,database);
}

void MySQLRecipeDB::findUseOf_Unit_InRecipes(ElementList *results, int unitID)
{
 QString command=QString("SELECT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.unit_id=%1;").arg(unitID);
QSqlQuery recipeFound( command,database); // Find the entries

// Populate data in the ElementList*
            if ( recipeFound.isActive() ) {
                while ( recipeFound.next() ) {
		    Element recipe;
		    recipe.id=recipeFound.value(0).toInt();
		    recipe.name=unescapeAndDecode(recipeFound.value(1).toString());
		    results->add(recipe);
                }
	}
}

void MySQLRecipeDB::findUseOf_Unit_InProperties(ElementList *results, int unitID)
{
 QString command=QString("SELECT ip.id,ip.name FROM ingredient_info ii, ingredient_properties ip WHERE ii.per_units=%1 AND ip.id=ii.property_id;").arg(unitID);
QSqlQuery recipeFound( command,database); // Find the entries

// Populate data in the ElementList*
            if ( recipeFound.isActive() ) {
                while ( recipeFound.next() ) {
		    Element recipe;
		    recipe.id=recipeFound.value(0).toInt();
		    recipe.name=recipeFound.value(1).toString();
		    results->add(recipe);
                }
	}


}

void MySQLRecipeDB::loadUnitRatios(UnitRatioList *ratioList)
{
ratioList->clear();

QString command;
command="SELECT unit1_id,unit2_id,ratio FROM units_conversion;";
QSqlQuery ratiosToLoad( command,database);

            if ( ratiosToLoad.isActive() ) {
                while ( ratiosToLoad.next() ) {
		    UnitRatio ratio;
		    ratio.uID1=ratiosToLoad.value(0).toInt();
		    ratio.uID2=ratiosToLoad.value(1).toInt();
		    ratio.ratio=ratiosToLoad.value(2).toDouble();
		    ratioList->add(ratio);
                }
	}
}

void MySQLRecipeDB::saveUnitRatio(const UnitRatio *ratio)
{
QString command;

// Check if it's a new ratio or it exists already.
command=QString("SELECT * FROM units_conversion WHERE unit1_id=%1 AND unit2_id=%2;").arg(ratio->uID1).arg(ratio->uID2); // Find ratio between units

QSqlQuery ratioFound(command,database); // Find the entries
bool newRatio=(ratioFound.size()==0);

if (newRatio)
	command=QString("INSERT INTO units_conversion VALUES(%1,%2,%3);").arg(ratio->uID1).arg(ratio->uID2).arg(ratio->ratio);
else
	command=QString("UPDATE units_conversion SET ratio=%3 WHERE unit1_id=%1 AND unit2_id=%2").arg(ratio->uID1).arg(ratio->uID2).arg(ratio->ratio);

ratioFound.exec(command); // Enter the new ratio
}

double MySQLRecipeDB::unitRatio(int unitID1, int unitID2)
{

if (unitID1==unitID2) return(1.0);
QString command;

command=QString("SELECT ratio FROM units_conversion WHERE unit1_id=%1 AND unit2_id=%2;").arg(unitID1).arg(unitID2);
QSqlQuery ratioToLoad( command,database);

            if ( ratioToLoad.isActive() )
	    {
            if (ratioToLoad.next())
	    	return(ratioToLoad.value(0).toDouble());
	    else return(-1);// There is no ratio defined between the units
            }

	    else
	    {
	    return(-1);
	    }
}

//Finds data dependant on this Ingredient/Unit combination
void MySQLRecipeDB::findIngredientUnitDependancies(int ingredientID,int unitID,ElementList *recipes,ElementList *ingredientInfo)
{

// Recipes using that combination

QString command=QString("SELECT DISTINCT r.id,r.title  FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1 AND il.unit_id=%2;").arg(ingredientID).arg(unitID);
QSqlQuery unitToRemove( command,database);
loadElementList(recipes,&unitToRemove);
// Ingredient info using that combination
command=QString("SELECT i.name,ip.name,ip.units,u.name FROM ingredients i, ingredient_info ii, ingredient_properties ip, units u WHERE i.id=ii.ingredient_id AND ii.ingredient_id=%1 AND ii.per_units=%2 AND ii.property_id=ip.id AND ii.per_units=u.id;").arg(ingredientID).arg(unitID);

unitToRemove.exec(command);
loadPropertyElementList(ingredientInfo,&unitToRemove);
}

void MySQLRecipeDB::findIngredientDependancies(int ingredientID,ElementList *recipes)
{
QString command=QString("SELECT DISTINCT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1").arg(ingredientID);

QSqlQuery ingredientToRemove( command,database);
loadElementList(recipes,&ingredientToRemove);
}



//Finds data dependant on the removal of this Unit
void MySQLRecipeDB::findUnitDependancies(int unitID,ElementList *properties,ElementList *recipes)
{

// Ingredient-Info (ingredient->property) using this Unit

QString command=QString("SELECT i.name,ip.name,ip.units,u.name  FROM ingredients i, ingredient_info ii, ingredient_properties ip, units u WHERE i.id=ii.ingredient_id AND ii.per_units=%1 AND ii.property_id=ip.id  AND ii.per_units=u.id;").arg(unitID);
QSqlQuery unitToRemove( command,database);
loadPropertyElementList(properties,&unitToRemove);

// Recipes using this Unit
command=QString("SELECT DISTINCT r.id,r.title  FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.unit_id=%1;").arg(unitID); // Without "DISTINCT" we get duplicates since ingredient_list has no unique recipe_id's
unitToRemove.exec(command);
loadElementList(recipes,&unitToRemove);

}

void MySQLRecipeDB::loadElementList(ElementList *elList, QSqlQuery *query)
{
if ( query->isActive() ) {
                while ( query->next() ) {
		    Element el;
		    el.id=query->value(0).toInt();
		    el.name=unescapeAndDecode(query->value(1).toString());
		    elList->add(el); // Note that ElementList _copies_, does not take the pointer while adding.
                }
            }
}
// See function "findUnitDependancies" for use
void MySQLRecipeDB::loadPropertyElementList(ElementList *elList, QSqlQuery *query)
{
if ( query->isActive() ) {
                while ( query->next() ) {
		    Element el;
		    el.id=-1; // There's no ID for the ingredient-property combination
		    QString ingName=query->value(0).toString();
		    QString propName=unescapeAndDecode(query->value(1).toString());
		    QString propUnits=unescapeAndDecode(query->value(2).toString());
		    QString propPerUnits=unescapeAndDecode(query->value(3).toString());

		    el.name=QString("In ingredient %1: property \"%2\" [%3/%4]").arg(ingName).arg(propName).arg(propUnits).arg(propPerUnits);
		    elList->add(el); // Note that ElementList _copies_, does not take the pointer while adding.
                }
            }
}

QCString MySQLRecipeDB::escapeAndEncode(const QString &s)
{
QString s_escaped=s;

s_escaped.replace ("'","\\'");
s_escaped.replace (";","\";\@"); // Small trick for only for parsing later on

return(s_escaped.utf8());
}

QString MySQLRecipeDB::unescapeAndDecode(const QString &s)
{
QString s_escaped=QString::fromUtf8(s.latin1());
s_escaped.replace("\";@",";");
return (s_escaped); // Use unicode encoding
}

bool MySQLRecipeDB::ingredientContainsUnit(int ingredientID, int unitID)
{
QString command=QString("SELECT *  FROM unit_list WHERE ingredient_id= %1 AND unit_id=%2;").arg(ingredientID).arg(unitID);
QSqlQuery recipeToLoad( command,database);
if (recipeToLoad.isActive())
{
	return(recipeToLoad.size()>0);
}
return false;
}

bool MySQLRecipeDB::ingredientContainsProperty(int ingredientID, int propertyID, int perUnitsID)
{
QString command=QString("SELECT *  FROM ingredient_info WHERE ingredient_id=%1 AND property_id=%2 AND per_units=%3;").arg(ingredientID).arg(propertyID).arg(perUnitsID);
QSqlQuery recipeToLoad( command,database);
if (recipeToLoad.isActive())
{
	return(recipeToLoad.size()>0);
}
return false;
}

QString MySQLRecipeDB::unitName(int unitID)
{
QString command=QString("SELECT * FROM units WHERE id=%1;").arg(unitID);
QSqlQuery unitToLoad( command,database);
if (unitToLoad.isActive() && unitToLoad.next()) // Go to the first record (there should be only one anyway.
	return(unitToLoad.value(1).toString());

return(QString::null);
}

bool MySQLRecipeDB::checkIntegrity(void)
{


// Check existence of the necessary tables (the database may be created, but empty)
QStringList tables; tables<<"ingredient_info"<<"ingredient_list"<<"ingredient_properties"<<"ingredients"<<"recipes"<<"unit_list"<<"units"<<"units_conversion"<<"categories"<<"category_list"<<"authors"<<"author_list"<<"db_info";
QString command=QString("SHOW TABLES;");

QSqlQuery tablesToCheck( command,database);

QStringList existingTableList;

// Get the existing list first

if (tablesToCheck.isActive())
{
	while (tablesToCheck.next())
		{
		QString tableName=tablesToCheck.value(0).toString();
		existingTableList<<tableName;
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
portOldDatabases(version);
return true;
}

void MySQLRecipeDB::createTable(QString tableName)
{

QString command;
bool createIndex;

if (tableName=="recipes") command=QString("CREATE TABLE recipes (id INTEGER NOT NULL AUTO_INCREMENT,title VARCHAR(%1),persons int(11),instructions TEXT, photo BLOB,   PRIMARY KEY (id));").arg(maxRecipeTitleLength());

else if (tableName=="ingredients") command=QString("CREATE TABLE ingredients (id INTEGER NOT NULL AUTO_INCREMENT, name VARCHAR(%1), PRIMARY KEY (id));").arg(maxIngredientNameLength());

else if (tableName=="ingredient_list") command="CREATE TABLE ingredient_list (recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, unit_id INTEGER, order_index INTEGER, INDEX ridil_index(recipe_id), INDEX iidil_index(ingredient_id));";

else if (tableName=="unit_list") command="CREATE TABLE unit_list (ingredient_id INTEGER, unit_id INTEGER);";

else if (tableName== "units") command=QString("CREATE TABLE units (id INTEGER NOT NULL AUTO_INCREMENT, name VARCHAR(%1), PRIMARY KEY (id));").arg(maxUnitNameLength());

else if  (tableName=="ingredient_info") command="CREATE TABLE ingredient_info (ingredient_id INTEGER, property_id INTEGER, amount FLOAT, per_units INTEGER);";

else if (tableName=="ingredient_properties") command="CREATE TABLE ingredient_properties (id INTEGER NOT NULL AUTO_INCREMENT,name VARCHAR(20), units VARCHAR(20), PRIMARY KEY (id));";

else if (tableName=="units_conversion") command="CREATE TABLE units_conversion (unit1_id INTEGER, unit2_id INTEGER, ratio FLOAT);";

else if (tableName=="categories") command=QString("CREATE TABLE categories (id int(11) NOT NULL auto_increment, name varchar(%1) default NULL,PRIMARY KEY (id));").arg(maxCategoryNameLength());

else if (tableName=="category_list") command="CREATE TABLE category_list (recipe_id int(11) NOT NULL,category_id int(11) NOT NULL, INDEX  rid_index (recipe_id), INDEX cid_index (category_id));";

else if (tableName=="authors") command=QString("CREATE TABLE authors (id int(11) NOT NULL auto_increment, name varchar(%1) default NULL,PRIMARY KEY (id));").arg(maxAuthorNameLength());

else if (tableName=="author_list") command="CREATE TABLE author_list (recipe_id int(11) NOT NULL,author_id int(11) NOT NULL);";

else if (tableName=="db_info") command="CREATE TABLE db_info (ver FLOAT NOT NULL,generated_by varchar(200) default NULL);";

else return;

QSqlQuery databaseToCreate(command,database); // execute the query

}

void MySQLRecipeDB::splitCommands(QString& s,QStringList& sl)
{
sl=QStringList::split(QRegExp(";{1}(?!@)"),s);
}

void MySQLRecipeDB::portOldDatabases(float version)
{
std::cerr<<"Current database version is..."<<version<<"\n";
QString command;
if (version<0.3)	// The database was generated with a version older than v 0.3. First update to 0.3 version
			// ( note that version no. means the version in which this DB structure
			// was introduced)
	{

	// Add new columns to existing tables (creating new tables is not necessary. Integrity check does that before)
	command="ALTER TABLE recipes ADD COLUMN persons int(11) AFTER title;";
	QSqlQuery tableToAlter(command,database);

	// Set the version to the new one (0.3)

	command="DELETE FROM db_info;"; // Remove previous version records if they exist
		tableToAlter.exec(command);
	command="INSERT INTO db_info VALUES(0.3,'Krecipes 0.4');"; // Set the new version
		tableToAlter.exec(command);
	}

if (version<0.4)  // Upgrade to the current DB version 0.4
	{

	// Add new columns to existing tables (creating any new tables is not necessary. Integrity check does that before)
	command="ALTER TABLE ingredient_list ADD COLUMN order_index int(11) AFTER unit_id;";
	QSqlQuery tableToAlter(command,database);

	// Missing indexes in the previous versions
	command="CREATE index rid_index ON category_list(recipe_id)";
	tableToAlter.exec(command);

	command="CREATE index cid_index ON category_list(category_id)";
	tableToAlter.exec(command);

	command="CREATE index ridil_index ON ingredient_list(recipe_id)";
	tableToAlter.exec(command);

	command="CREATE index iidil_index ON ingredient_list(ingredient_id)";
	tableToAlter.exec(command);

	// Port data

	//*1:: Recipes have always category -1 to speed up searches (no JOINs needed)
	command="SELECT r.id FROM recipes r;"; // Find all recipes
	QSqlQuery categoryToAdd(QString::null,database);
	tableToAlter.exec( command);
            if ( tableToAlter.isActive() ) {
                while ( tableToAlter.next() ) {
		    int recipeId=tableToAlter.value(0).toInt();
		    QString cCommand=QString("INSERT INTO category_list VALUES (%1,-1);").arg(recipeId);
		    categoryToAdd.exec(cCommand);
                }
            }

	// Set the version to the new one (0.4)

	command="DELETE FROM db_info;"; // Remove previous version records if they exist
		tableToAlter.exec(command);
	command="INSERT INTO db_info VALUES(0.4,'Krecipes 0.4');"; // Set the new version
		tableToAlter.exec(command);
	}
}

float MySQLRecipeDB::databaseVersion(void)
{

QString command="SELECT ver FROM db_info";
QSqlQuery dbVersion(command,database);

if ( dbVersion.isActive() && dbVersion.next() )
	return(dbVersion.value(0).toDouble());// There should be only one (or none for old DB) element, so go to first
else return (0.2); // if table is empty, assume oldest (0.2), and port
}

void MySQLRecipeDB::loadCategories(ElementList *list)
{
list->clear();
QString command="SELECT * FROM categories ORDER BY name;";
QSqlQuery categoryToLoad(command,database);
if (categoryToLoad.isActive()) {
	while (categoryToLoad.next())
	{
	Element el;
	el.id=categoryToLoad.value(0).toInt();
	el.name=unescapeAndDecode(categoryToLoad.value(1).toString());
	list->add(el);
	}
}
}

void MySQLRecipeDB::loadRecipeCategories(int recipeID, ElementList *list)
{
QString command=QString("SELECT c.id,c.name FROM categories c,category_list cl WHERE cl.recipe_id=%1 AND c.id=cl.category_id;").arg(recipeID);
QSqlQuery categoryToLoad(command,database);
if (categoryToLoad.isActive()) {
	while (categoryToLoad.next())
	{
	Element el;
	el.id=categoryToLoad.value(0).toInt();
	el.name=unescapeAndDecode(categoryToLoad.value(1).toString());
	list->add(el);
	}
}
}

void MySQLRecipeDB::createNewCategory(QString &categoryName)
{
QString command;

command=QString("INSERT INTO categories VALUES(NULL,'%1');").arg(escapeAndEncode(categoryName));
QSqlQuery categoryToCreate( command,database);
}

void MySQLRecipeDB::modCategory(int categoryID, QString newLabel)
{
QString command;

command=QString("UPDATE categories SET name='%1' WHERE id=%2;").arg(escapeAndEncode(newLabel)).arg(categoryID);
QSqlQuery categoryToCreate( command,database);
}

void MySQLRecipeDB::removeCategory(int categoryID)
{
QString command;

command=QString("DELETE FROM categories WHERE id=%1;").arg(categoryID);
QSqlQuery categoryToRemove( command,database);

command=QString("DELETE FROM category_list WHERE category_id=%1;").arg(categoryID);
categoryToRemove.exec(command);
}

void MySQLRecipeDB::addCategoryToRecipe(int recipeID, int categoryID)
{
QString command=QString("INSERT INTO categories_list VALUES(%1,%2);").arg(recipeID).arg(categoryID);
QSqlQuery categoryToAdd( command,database);
}



void MySQLRecipeDB::loadAuthors(ElementList *list)
{
list->clear();
QString command="SELECT * FROM authors;";
QSqlQuery authorToLoad(command,database);
if (authorToLoad.isActive()) {
	while (authorToLoad.next())
	{
	Element el;
	el.id=authorToLoad.value(0).toInt();
	el.name=unescapeAndDecode(authorToLoad.value(1).toString());
	list->add(el);
	}
}
}

void MySQLRecipeDB::loadRecipeAuthors(int recipeID, ElementList *list)
{
list->clear();
QString command=QString("SELECT a.id,a.name FROM authors a, author_list al WHERE al.recipe_id=%1 AND a.id=al.author_id;").arg(recipeID);
QSqlQuery authorToLoad(command,database);
if (authorToLoad.isActive()) {
	while (authorToLoad.next())
	{
	Element el;
	el.id=authorToLoad.value(0).toInt();
	el.name=unescapeAndDecode(authorToLoad.value(1).toString());
	list->add(el);
	}
}
}

void MySQLRecipeDB::createNewAuthor(const QString &authorName)
{
QString command;

command=QString("INSERT INTO authors VALUES(NULL,'%1');").arg(escapeAndEncode(authorName));
QSqlQuery authorToCreate( command,database);
}

void MySQLRecipeDB::modAuthor(int authorID, QString newLabel)
{
QString command;

command=QString("UPDATE authors SET name='%1' WHERE id=%2;").arg(escapeAndEncode(newLabel)).arg(authorID);
QSqlQuery authorToCreate( command,database);
}

void MySQLRecipeDB::removeAuthor(int authorID)
{
QString command;

command=QString("DELETE FROM authors WHERE id=%1;").arg(authorID);
QSqlQuery authorToRemove( command,database);
}

void MySQLRecipeDB::addAuthorToRecipe(int recipeID, int authorID)
{
QString command=QString("INSERT INTO author_list VALUES(%1,%2);").arg(recipeID).arg(authorID);
QSqlQuery authorToAdd( command,database);
}


int MySQLRecipeDB::findExistingUnitsByName(const QString& name,int ingredientID, ElementList *list)
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

	QSqlQuery unitsToLoad(command,database); // Run the query

	if (list) //If the pointer exists, then load all the values found into it
	{
	if (unitsToLoad.isActive()) {
		while (unitsToLoad.next())
		{
		Element el;
		el.id=unitsToLoad.value(0).toInt();
		el.name=unescapeAndDecode(unitsToLoad.value(1).toString());
		list->add(el);
		}
	}
	}

	return(unitsToLoad.size());
}

int MySQLRecipeDB::findExistingAuthorByName( const QString& name )
{
	QCString search_str = escapeAndEncode(name);
	search_str.truncate(maxAuthorNameLength()); //truncate to the maximum size db holds

	QString command=QString("SELECT id FROM authors WHERE name='%1';").arg(search_str);
	QSqlQuery elementToLoad(command,database); // Run the query
	int id = -1;

	if (elementToLoad.isActive() && elementToLoad.first())
		id=elementToLoad.value(0).toInt();

	return id;
}

int MySQLRecipeDB::findExistingCategoryByName( const QString& name )
{
	QCString search_str = escapeAndEncode(name);
	search_str.truncate(maxCategoryNameLength()); //truncate to the maximum size db holds

	QString command=QString("SELECT id FROM categories WHERE name='%1';").arg(search_str);
	QSqlQuery elementToLoad(command,database); // Run the query
	int id = -1;

	if (elementToLoad.isActive() && elementToLoad.first())
		id=elementToLoad.value(0).toInt();

	return id;
}

int MySQLRecipeDB::findExistingIngredientByName( const QString& name )
{
	QCString search_str = escapeAndEncode(name);
	search_str.truncate(maxIngredientNameLength()); //truncate to the maximum size db holds

	QString command=QString("SELECT id FROM ingredients WHERE name='%1';").arg(search_str);
	QSqlQuery elementToLoad(command,database); // Run the query
	int id = -1;

	if (elementToLoad.isActive() && elementToLoad.first())
		id=elementToLoad.value(0).toInt();

	return id;
}

int MySQLRecipeDB::findExistingUnitByName( const QString& name )
{
	QCString search_str = escapeAndEncode(name);
	search_str.truncate(maxUnitNameLength()); //truncate to the maximum size db holds

	QString command=QString("SELECT id FROM units WHERE name='%1';").arg(search_str);
	QSqlQuery elementToLoad(command,database); // Run the query
	int id = -1;

	if (elementToLoad.isActive() && elementToLoad.first())
		id=elementToLoad.value(0).toInt();

	return id;
}

int MySQLRecipeDB::findExistingRecipeByName( const QString& name )
{
	QCString search_str = escapeAndEncode(name);
	search_str.truncate(maxRecipeTitleLength()); //truncate to the maximum size db holds

	QString command=QString("SELECT id FROM recipes WHERE title='%1';").arg(search_str);
	QSqlQuery elementToLoad(command,database); // Run the query

	int id = -1;
	if (elementToLoad.isActive() && elementToLoad.first())
		id=elementToLoad.value(0).toInt();

	return id;
}

void MySQLRecipeDB::givePermissions(const QString &dbName,const QString &username, const QString &password, const QString &clientHost)
{
QString command;

if ((password!="")&&(password!=QString::null)) command=QString("GRANT ALL ON %1.* TO %2@%3 IDENTIFIED BY '%4';").arg(dbName).arg(username).arg(clientHost).arg(password);
else command=QString("GRANT ALL ON %1.* TO %2@%3;").arg(dbName).arg(username).arg(clientHost);

std::cerr<<"I'm doing the query to setup permissions\n";

QSqlQuery permissionsToSet( command,database);
}

QString MySQLRecipeDB::getUniqueRecipeTitle( const QString &recipe_title )
{
	//already is unique
	if ( findExistingRecipeByName( recipe_title ) == -1 )
		return recipe_title;

	QString return_title=recipe_title; //If any error is produced, just go for default value (always return something)

	QString command = QString( "SELECT COUNT(DISTINCT title) FROM recipes WHERE title LIKE '%1 (%)';" ).arg(escapeAndEncode(recipe_title));

	QSqlQuery alikeRecipes( command, database );
	if (alikeRecipes.isActive() && alikeRecipes.first());
	{
		int count = alikeRecipes.value(0).toInt();
		return_title = QString("%1 (%2)").arg(recipe_title).arg(count+2);

		//make sure this newly created title is unique (just in case)
		while ( findExistingRecipeByName( return_title ) != -1 )
		{
			count--; //go down to find the skipped recipe(s)
			return_title = QString("%1 (%2)").arg(recipe_title).arg(count+2);
		}
	}

	return return_title;
}

QString MySQLRecipeDB::recipeTitle(int recipeID)
{
QString command=QString("SELECT * FROM recipes WHERE id=%1;").arg(recipeID);
QSqlQuery recipeToLoad( command,database);
if (recipeToLoad.isActive() && recipeToLoad.next()) // Go to the first record (there should be only one anyway.
	return(recipeToLoad.value(1).toString());

return(QString::null);
}

int MySQLRecipeDB::lastInsertID()
{
	QSqlQuery lastInsertID("SELECT LAST_INSERT_ID();", database);

	int id = -1;
	if (lastInsertID.isActive() && lastInsertID.next())
		id = lastInsertID.value(0).toInt();

	return id;
}

void MySQLRecipeDB::emptyData(void)
{
QStringList tables; tables<<"ingredient_info"<<"ingredient_list"<<"ingredient_properties"<<"ingredients"<<"recipes"<<"unit_list"<<"units"<<"units_conversion"<<"categories"<<"category_list"<<"authors"<<"author_list";
QSqlQuery tablesToEmpty(QString::null,database);
for (QStringList::Iterator it = tables.begin(); it != tables.end(); ++it)
	{
	QString command=QString("DELETE FROM %1;").arg(*it);
	tablesToEmpty.exec(command);
	}
}

