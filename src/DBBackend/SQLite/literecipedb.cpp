 /***************************************************************************
 *   Copyright (C) 2003 by krecipes.sourceforge.net authors                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "literecipedb.h"
#include "kstandarddirs.h"
#define DB_FILENAME "/home/uga/try.krecdb"

LiteRecipeDB::LiteRecipeDB(QString host, QString user, QString pass, QString DBname,bool init)
{


	DBuser=user;DBpass=pass;DBhost=host;

        database= new QSQLiteDB();
	database->open(DB_FILENAME);
        if ( !database->open(DB_FILENAME) ) {
	     //Try to create the database
	     createDB();

	     //Now Reopen the Database and exit if it fails
	     if (!database->open(DB_FILENAME))
		{
		std::cerr<<QString("Could not open DB. You may not have permissions. Exiting.\n").arg(user).latin1();
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
LiteRecipeDB::~LiteRecipeDB()
{
database->close();
}

void LiteRecipeDB::createDB()
{

// FIXME : create database file?
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

		recipe->title=unescapeAndDecode(row.data(0));
		recipe->instructions=unescapeAndDecode(row.data(1));
		recipe->persons=row.data(2).toInt();
		recipe->recipeID=recipeID;
        }
        else
        {
                std::cerr<<recipeToLoad.getError();
        }



// Read the ingredients
command=QString("SELECT il.ingredient_id,i.name,il.amount,u.id,u.name FROM ingredients i, ingredient_list il, units u WHERE il.recipe_id=%1 AND i.id=il.ingredient_id AND u.id=il.unit_id ORDER BY il.order_index;").arg(recipeID);

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
   	bool ok; QString photoString=row.data(0,&ok);
	if (ok) recipe->photo.loadFromData( (unsigned char*) photoString.ascii(),photoString.length());
     	// picture will now have a ready-to-use image

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
		    recipe->categoryList.add(el);
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

QSQLiteResult insertedRecipe=database->executeQuery(command);

// If it's a new recipe, identify the ID that was given to the recipe and store in the Recipe itself
int recipeID;
if (newRecipe)
{
// FIXME: get the ID of the last inserted recipe
// The following WON'T WORK. Need to extend API to use int sqlite_last_insert_rowid(sqlite*);

	if (insertedRecipe.getStatus()!=QSQLiteResult::Failure)
	{
	QSQLiteResultRow row=insertedRecipe.first();
	recipeID=row.data(0).toInt();
	recipe->recipeID=recipeID;
	}

// FIXME: The function above is WRONG. Read comment

}

recipeID=recipe->recipeID;

// Let's begin storing the Image!
//FIXME: I need to do this in plain SQLite to do the escaping
/*
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
*/

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


void LiteRecipeDB::loadRecipeList(ElementList *list)
{
list->clear();

QString command;

command="SELECT id,title FROM recipes;";

QSQLiteResult recipeToLoad=database->executeQuery(command);
if ( recipeToLoad.getStatus()!=QSQLiteResult::Failure ) {
	QSQLiteResultRow row=recipeToLoad.first();
            if ( !recipeToLoad.atEnd() ) {

		    Element recipe;
		    recipe.id=row.data(0).toInt();
		    recipe.name=unescapeAndDecode(row.data(1));
		    list->add(recipe);
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
	QFile file (KGlobal::dirs()->findResource("appdata", "data/structure.sql"));
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
command=QString("SELECT ip.id,ip.name,ip.units,ii.per_units,u.name,ii.amount FROM ingredient_properties ip, ingredient_info ii, units u WHERE ii.ingredient_id=%1 AND ii.property_id=ip.id AND ii.per_units=u.id").arg(ingredientID);
}
else // Load the whole property list
{
usePerUnit=false;
command=QString("SELECT  id,name,units FROM ingredient_properties;");
}

QSQLiteResult propertiesToLoad=database->executeQuery(command);
// Load the results into the list
if (propertiesToLoad.getStatus()!=QSQLiteResult::Failure) {
                while ( !propertiesToLoad.atEnd() ) {
		QSQLiteResultRow row=propertiesToLoad.first();
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
		    list->add(prop);
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
		    ratio.ingID1=row.data(0).toInt();
		    ratio.ingID2=row.data(1).toInt();
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
command=QString("SELECT * FROM units_conversion WHERE unit1_id=%1 AND unit2_id=%2;").arg(ratio->ingID1).arg(ratio->ingID2); // Find ratio between units

QSQLiteResult ratioFound=database->executeQuery(command); // Find the entries
bool newRatio/*=(ratioFound.size()==0)*/; // FIXME: I need size() in the API

if (newRatio)
	command=QString("INSERT INTO units_conversion VALUES(%1,%2,%3);").arg(ratio->ingID1).arg(ratio->ingID2).arg(ratio->ratio);
else
	command=QString("UPDATE units_conversion SET ratio=%3 WHERE unit1_id=%1 AND unit2_id=%2").arg(ratio->ingID1).arg(ratio->ingID2).arg(ratio->ratio);

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
	    if ( !ratioToLoad.atEnd() ) //FIXME: Check this. This should check if there's no data found
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
QString s_escaped=s;

s_escaped.replace ("'","\\'");
s_escaped.replace (";","\";\@"); // Small trick for only for parsing later on

return(s_escaped.utf8());
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
	/*return(recipeToLoad.size()>0);*/ //FIXME: I need size() in the API of QSQLiteResult
	}
return false;
}

bool LiteRecipeDB::ingredientContainsProperty(int ingredientID, int propertyID, int perUnitsID)
{
QString command=QString("SELECT *  FROM ingredient_info WHERE ingredient_id=%1 AND property_id=%2 AND per_units=%3;").arg(ingredientID).arg(propertyID).arg(perUnitsID);
QSQLiteResult recipeToLoad=database->executeQuery(command);

if (recipeToLoad.getStatus()!=QSQLiteResult::Failure)
{
	/*return(recipeToLoad.size()>0);*/ //FIXME: I need size() implemented in the API
}
return false;
}

QString LiteRecipeDB::unitName(int unitID)
{
QString command=QString("SELECT * FROM units WHERE id=%1;").arg(unitID);
QSQLiteResult unitToLoad=database->executeQuery(command);
if (unitToLoad.getResults()!=QSQLiteResult::Failure)
{
if (unitToLoad.next()) // Go to the first record (there should be only one anyway.
 return(unitToLoad.data(0));
}

return(QString::null);
}

/*bool LiteRecipeDB::checkIntegrity(void)
{


// Check existence of the necessary tables (the database may be created, but empty)
QStringList tables; tables<<"ingredient_info"<<"ingredient_list"<<"ingredient_properties"<<"ingredients"<<"recipes"<<"unit_list"<<"units"<<"units_conversion"<<"categories"<<"category_list"<<"authors"<<"author_list"<<"db_info";
QString command=QString("SHOW TABLES;");
QSqlQuery tablesToCheck( command,database);
if (tablesToCheck.isActive())
{
	for (QStringList::Iterator it = tables.begin(); it != tables.end(); ++it)
	{
	bool found=false;
	while (tablesToCheck.next() && !found)
		{
		QString tableName=tablesToCheck.value(0).toString();
		found=(tableName==*it); // Table exists
		}
	if (!found) createTable(*it);
	}
}

else return(false);

// Check for older versions, and port

std::cerr<<"Checking database version...\n";
float version=databaseVersion();
portOldDatabases(version);
return true;
}

void LiteRecipeDB::createTable(QString tableName)
{

QString command;

if (tableName=="recipes") command="CREATE TABLE recipes (id INTEGER NOT NULL AUTO_INCREMENT,title VARCHAR(200),persons int(11),instructions TEXT, photo BLOB,   PRIMARY KEY (id));";

else if (tableName=="ingredients") command="CREATE TABLE ingredients (id INTEGER NOT NULL AUTO_INCREMENT, name VARCHAR(50), PRIMARY KEY (id));";

else if (tableName=="ingredient_list") command="CREATE TABLE ingredient_list (recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, unit_id INTEGER);";

else if (tableName=="unit_list") command="CREATE TABLE unit_list (ingredient_id INTEGER, unit_id INTEGER);";

else if (tableName== "units") command="CREATE TABLE units (id INTEGER NOT NULL AUTO_INCREMENT, name VARCHAR(20), PRIMARY KEY (id));";

else if  (tableName=="ingredient_info") command="CREATE TABLE ingredient_info (ingredient_id INTEGER, property_id INTEGER, amount FLOAT, per_units INTEGER);";

else if (tableName=="ingredient_properties") command="CREATE TABLE ingredient_properties (id INTEGER NOT NULL AUTO_INCREMENT,name VARCHAR(20), units VARCHAR(20), PRIMARY KEY (id));";

else if (tableName=="units_conversion") command="CREATE TABLE units_conversion (unit1_id INTEGER, unit2_id INTEGER, ratio FLOAT);";

else if (tableName=="categories") command="CREATE TABLE categories (id int(11) NOT NULL auto_increment, name varchar(20) default NULL,PRIMARY KEY (id));";

else if (tableName=="category_list") command="CREATE TABLE category_list (recipe_id int(11) NOT NULL,category_id int(11) NOT NULL);";

else if (tableName=="authors") command="CREATE TABLE authors (id int(11) NOT NULL auto_increment, name varchar(20) default NULL,PRIMARY KEY (id));";

else if (tableName=="author_list") command="CREATE TABLE author_list (recipe_id int(11) NOT NULL,author_id int(11) NOT NULL);";

else if (tableName=="db_info") command="CREATE TABLE db_info (ver FLOAT NOT NULL,generated_by varchar(200) default NULL);";



QSqlQuery tableToCreate(command,database);

}

void LiteRecipeDB::splitCommands(QString& s,QStringList& sl)
{
sl=QStringList::split(QRegExp(";{1}(?!@)"),s);
}

void LiteRecipeDB::portOldDatabases(float version)
{
std::cerr<<"Current database version is..."<<version<<"\n";
QString command;
if (version<0.3)	// The database was generated with an old version of Krecipes, needs upgrade to 			//the current v 0.3 (version no means the version in which this DB structure 				//was introduced)
	{

	// Add new columns to existing tables (creating new tables is not necessary. Integrity check does that before)
	command="ALTER TABLE recipes ADD COLUMN persons int(11) AFTER title;";
	QSqlQuery tableToAlter(command,database);

	// Set the version to the new one (0.3)

	command="DELETE FROM db_info;"; // Remove previous version records if they exist
		tableToAlter.exec(command);
	command="INSERT INTO db_info VALUES(0.3,'Krecipes 0.3');"; // Set the new version
		tableToAlter.exec(command);
	}

if (version<0.4)  // Upgrade to DB version 0.4
	{

	// Add new columns to existing tables (creating any new tables is not necessary. Integrity check does that before)
	command="ALTER TABLE ingredient_list ADD COLUMN order_index int(11) AFTER unit_id;";
	QSqlQuery tableToAlter(command,database);

	// Set the version to the new one (0.4)

	command="DELETE FROM db_info;"; // Remove previous version records if they exist
		tableToAlter.exec(command);
	command="INSERT INTO db_info VALUES(0.4,'Krecipes 0.3+(CVS)');"; // Set the new version
		tableToAlter.exec(command);
	}
}

float LiteRecipeDB::databaseVersion(void)
{

QString command="SELECT ver FROM db_info";
QSqlQuery dbVersion(command,database);

if ( dbVersion.isActive() ) {
	if (dbVersion.next())
		return(dbVersion.value(0).toDouble());// There should be only one (or none for old DB) element, so go to first
	else return (0.2); // if table is empty, assume oldest (0.2), and port
}
else return(0.2); // 0.2 didn't have this table yet
}

void LiteRecipeDB::loadCategories(ElementList *list)
{
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

void LiteRecipeDB::loadRecipeCategories(int recipeID, ElementList *list)
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

void LiteRecipeDB::createNewCategory(QString &categoryName)
{
QString command;

command=QString("INSERT INTO categories VALUES(NULL,'%1');").arg(escapeAndEncode(categoryName));
QSqlQuery categoryToCreate( command,database);
}

void LiteRecipeDB::removeCategory(int categoryID)
{
QString command;

command=QString("DELETE FROM categories WHERE id=%1;").arg(categoryID);
QSqlQuery categoryToRemove( command,database);
}

void LiteRecipeDB::addCategoryToRecipe(int recipeID, int categoryID)
{
QString command=QString("INSERT INTO categories_list VALUES(%1,%2);").arg(recipeID).arg(categoryID);
QSqlQuery categoryToAdd( command,database);
}



void LiteRecipeDB::loadAuthors(ElementList *list)
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

void LiteRecipeDB::loadRecipeAuthors(int recipeID, ElementList *list)
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

void LiteRecipeDB::createNewAuthor(const QString &authorName)
{
QString command;

command=QString("INSERT INTO authors VALUES(NULL,'%1');").arg(escapeAndEncode(authorName));
QSqlQuery authorToCreate( command,database);
}

void LiteRecipeDB::removeAuthor(int authorID)
{
QString command;

command=QString("DELETE FROM authors WHERE id=%1;").arg(authorID);
QSqlQuery authorToRemove( command,database);
}

void LiteRecipeDB::addAuthorToRecipe(int recipeID, int authorID)
{
QString command=QString("INSERT INTO author_list VALUES(%1,%2);").arg(recipeID).arg(authorID);
QSqlQuery authorToAdd( command,database);
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

int LiteRecipeDB::findExistingElementByName( const QString& name, const QString &element )
{
	QString command=QString("SELECT id FROM %1 WHERE name='%2';").arg(element).arg(escapeAndEncode(name));
	QSqlQuery elementToLoad(command,database); // Run the query
	int id = -1;

	if (elementToLoad.isActive() && elementToLoad.first())
		id=elementToLoad.value(0).toInt();

	return id;
}

int LiteRecipeDB::findExistingAuthorByName( const QString& name )
{
	return findExistingElementByName( name, "authors" );
}

int LiteRecipeDB::findExistingCategoryByName( const QString& name )
{
	return findExistingElementByName( name, "categories" );
}

int LiteRecipeDB::findExistingIngredientByName( const QString& name )
{
	return findExistingElementByName( name, "ingredients" );
}

int LiteRecipeDB::findExistingUnitByName( const QString& name )
{
	return findExistingElementByName( name, "units" );
}

int LiteRecipeDB::findExistingRecipeByName( const QString& name )
{
	QString command=QString("SELECT id FROM recipes WHERE title='%1';").arg(escapeAndEncode(name));
	QSqlQuery elementToLoad(command,database); // Run the query
	int id = -1;

	if (elementToLoad.isActive() && elementToLoad.first())
		id=elementToLoad.value(0).toInt();

	return id;
}

void LiteRecipeDB::givePermissions(const QString &dbName,const QString &username, const QString &password, const QString &clientHost)
{
QString command;

if ((password!="")&&(password!=QString::null)) command=QString("GRANT ALL ON %1.* TO %2@%3 IDENTIFIED BY '%4';").arg(dbName).arg(username).arg(clientHost).arg(password);
else command=QString("GRANT ALL ON %1.* TO %2@%3;").arg(dbName).arg(username).arg(clientHost);

std::cerr<<"I'm doing the query to setup permissions\n";

QSqlQuery permissionsToSet( command,database);
}

QString LiteRecipeDB::getUniqueRecipeTitle( const QString &recipe_title )
{
	//already is unique
	if ( findExistingRecipeByName( recipe_title ) == -1 )
		return recipe_title;

	QString command = QString( "SELECT COUNT(DISTINCT title) FROM recipes WHERE title LIKE '%1 (%)';" ).arg(escapeAndEncode(recipe_title));
	QSqlQuery alikeRecipes( command, database );

	alikeRecipes.first();
	int count = alikeRecipes.value(0).toInt();

	QString return_title = QString("%1 (%2)").arg(recipe_title).arg(count+2);

	//make sure this newly created title is unique (just in case)
	while ( findExistingRecipeByName( return_title ) != -1 )
	{
		count--;
		return_title = QString("%1 (%2)").arg(recipe_title).arg(count+2);
	}

	return return_title;
}

QString LiteRecipeDB::recipeTitle(int recipeID)
{
QString command=QString("SELECT * FROM recipes WHERE id=%1;").arg(recipeID);
QSqlQuery recipeToLoad( command,database);
if (recipeToLoad.isActive())
{
if(recipeToLoad.next()) // Go to the first record (there should be only one anyway.
 return(recipeToLoad.value(1).toString());
}
return(QString::null);
}

int LiteRecipeDB::lastInsertID()
{
	QSqlQuery lastInsertID("SELECT LAST_INSERT_ID();", database);

	int id = -1;
	if (lastInsertID.isActive() && lastInsertID.next()) //this will always return a value
		id = lastInsertID.value(0).toInt();

	return id;
}

void LiteRecipeDB::emptyData(void)
{
QStringList tables; tables<<"ingredient_info"<<"ingredient_list"<<"ingredient_properties"<<"ingredients"<<"recipes"<<"unit_list"<<"units"<<"units_conversion"<<"categories"<<"category_list"<<"authors"<<"author_list";
QSqlQuery tablesToEmpty(QString::null,database);
for (QStringList::Iterator it = tables.begin(); it != tables.end(); ++it)
	{
	QString command=QString("DELETE FROM %1;").arg(*it);
	tablesToEmpty.exec(command);
	}
}
*/