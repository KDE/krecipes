 /**************************************************************************
 *   Copyright (C) 2003 by                                                 *
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

#ifndef LITERECIPEDB_H
#define LITERECIPEDB_H

#define DEFAULT_DB_NAME "Krecipes"

#include <qimage.h>
#include <qfileinfo.h>
#include <qregexp.h>
#include <qstring.h>
#include <iostream>
#include <asm/unistd.h>
#include "recipe.h"
#include "datablocks/recipelist.h"
#include "elementlist.h"
#include "ingredientpropertylist.h"
#include "unitratiolist.h"
#include "recipedb.h"
#include "libqsqlite/krecqsqlitedb.h"

/**
@author Unai Garro
*/
class LiteRecipeDB:public RecipeDB{
private:
	QSQLiteDB *database;
	void createDB(void);
	QString DBuser;
	QString DBpass;
	QString DBhost;
public:
	LiteRecipeDB(const QString host, const QString user=QString::null, const QString pass=QString::null, const QString DBName=DEFAULT_DB_NAME,bool init=true);
	~LiteRecipeDB(void);

	void addAuthorToRecipe(int recipeID, int categoryID);
	void addCategoryToRecipe(int recipeID, int categoryID);


	void addProperty(const QString &name, const QString &units);
	void addPropertyToIngredient(int ingredientID,int propertyID,double amount, int perUnitsID);
	void addUnitToIngredient(int ingredientID,int unitID);

	void changePropertyAmountToIngredient(int ingredientID,int propertyID,double amount,int per_units);

	void createNewAuthor(const QString &authorName);
	void createNewCategory(const QString &categoryName);
	void createNewIngredient(const QString &ingredientName);
	void createNewPrepMethod(const QString &prepMethodName);
	void createNewUnit(const QString &unitName);

	void emptyData(void);

	int  findExistingAuthorByName(const QString& name);
	int  findExistingCategoryByName(const QString& name);
	int  findExistingIngredientByName(const QString& name);
	int  findExistingRecipeByName(const QString& name);
	int  findExistingUnitByName(const QString& name);
	int  findExistingUnitsByName(const QString& name,int ingredientID=-1, ElementList *list=0);
	void findIngredientUnitDependancies(int ingredientID,int unitID,ElementList *recipes,ElementList *ingredientInfo);
	void findIngredientDependancies(int ingredientID,ElementList *recipes);
	void findUnitDependancies(int unitID,ElementList *properties,ElementList *recipes);
	void findUseOf_Ing_Unit_InRecipes(ElementList *results, int ingredientID, int unitID);
	void findUseOfIngInRecipes(ElementList *results,int ingredientID);
	void findUseOf_Unit_InRecipes(ElementList *results, int unitID);
	void findUseOf_Unit_InProperties(ElementList *results, int unitID);

	QString getUniqueRecipeTitle( const QString &recipe_title );
	void givePermissions(const QString &dbName,const QString &username, const QString &password=QString::null, const QString &clientHost="localhost");


	bool ingredientContainsProperty(int ingredientID, int propertyID, int perUnitsID);
	bool ingredientContainsUnit(int ingredientID, int unitID);

	void initializeDB(void);
	void initializeData(void);

	int lastInsertID();

	void loadAllRecipeIngredients(RecipeIngredientList *list,bool withNames=true);
	void loadAuthors(ElementList *list);
	void loadCategories(ElementList *list);
	void loadIngredients(ElementList *list);
	void loadPossibleUnits(int ingredientID, ElementList *list);
	void loadPrepMethods( ElementList *list);
	void loadProperties(IngredientPropertyList *list,int ingredientID=-2); // Loads the list of possible properties by default, all the ingredient properties with -1, and the ingredients of given property if id>=0
	void loadRecipe(Recipe *recipe,int recipeID=0);
	void loadRecipeAuthors(int recipeID, ElementList *list);
	void loadRecipeCategories(int recipeID, ElementList *list);
	void loadRecipeDetails(RecipeList *rlist,bool loadIngredients=false,bool loadCategories=false); // Read only the recipe details (no instructions, no photo,...) and when loading ingredients and categories, no names, just IDs)
	void loadRecipeList(ElementList *list,int categoryID=0,QPtrList <int>*recipeCategoryList=0);
	void loadUnits(ElementList *list);
	void loadUnitRatios(UnitRatioList *ratioList);

  /**
  * set newLabel for ingredientID
  */
	void modIngredient(int ingredientID, QString newLabel);
  /**
  * set newLabel for unitID
  */
	void modUnit(int unitID, QString newLabel);
  /**
  * set newLabel for categoryID
  */
	void modCategory(int categoryID, QString newLabel);
  /**
  * set newLabel for authorID
  */
	void modAuthor(int authorID, QString newLabel);
	
	void modPrepMethod(int prepMethodID, const QString &newLabel);

	QString recipeTitle(int recipeID);

	void removeAuthor(int categoryID);
	void removeCategory(int categoryID);
	void removeIngredient(int ingredientID);
	void removePrepMethod(int prepMethodID);
	void removeProperty(int propertyID);
	void removePropertyFromIngredient(int ingredientID, int propertyID,int perUnitID);
	void removeRecipe(int id);
	void removeRecipeFromCategory(int ingredientID, int categoryID);
	void removeUnit(int unitID);
	void removeUnitFromIngredient(int ingredientID, int unitID);

	void saveRecipe(Recipe *recipe);
	void saveUnitRatio(const UnitRatio *ratio);

	double unitRatio(int unitID1, int unitID2);

	QString escape(const QString &s);
	QCString escapeAndEncode(const QString &s);
	QString unescapeAndDecode(const QString &s);

	QString unitName(int unitID);
	bool checkIntegrity(void);

	void createTable(QString tableName);
	void splitCommands(QString& s,QStringList& sl);

	float databaseVersion(void);

private:
	void loadElementList(ElementList *elList, QSQLiteResult *query);
	void loadPropertyElementList(ElementList *elList, QSQLiteResult *query);
	void portOldDatabases(float version);
	int sqlite_encode_binary(const unsigned char *in, int n, unsigned char *out);
	int sqlite_decode_binary(const unsigned char *in, unsigned char *out);
};




#endif
