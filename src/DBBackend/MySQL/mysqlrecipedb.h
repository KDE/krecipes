/***************************************************************************
 *   Copyright (C) 2003 by krecipes.sourceforge.net authors                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MYSQLRECIPEDB_H
#define MYSQLRECIPEDB_H

#define DB_DRIVER "QMYSQL3"
#define DEFAULT_DB_NAME "Krecipes"
#include <mysql/mysql.h>
#include <qsqldatabase.h>
#include <qimage.h>
#include <qfileinfo.h>
#include <qregexp.h>
#include <qstring.h>
#include <iostream>
#include <asm/unistd.h>
#include "../recipe.h"
#include "../elementlist.h"
#include "../ingredientpropertylist.h"
#include "../unitratiolist.h"
#include "recipedb.h"

/**
@author Unai Garro
*/
class MySQLRecipeDB:public RecipeDB{
private:
	QSqlDatabase *database;
	void createDB(void);
	QString DBuser;
	QString DBpass;
	QString DBhost;
public:
	MySQLRecipeDB(const QString host, const QString user=QString::null, const QString pass=QString::null, const QString DBName=DEFAULT_DB_NAME,bool init=true);
	~MySQLRecipeDB(void);

	void addAuthorToRecipe(int recipeID, int categoryID);
	void addCategoryToRecipe(int recipeID, int categoryID);


	void addProperty(QString &name, QString &units);
	void addPropertyToIngredient(int ingredientID,int propertyID,double amount, int perUnitsID);
	void addUnitToIngredient(int ingredientID,int unitID);

	void changePropertyAmountToIngredient(int ingredientID,int propertyID,double amount,int per_units);

	void createNewAuthor(const QString &authorName);
	void createNewCategory(QString &categoryName);
	void createNewIngredient(QString ingredientName);
	void createNewUnit(QString &unitName);

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

	void loadAuthors(ElementList *list);
	void loadCategories(ElementList *list);
	void loadIngredients(ElementList *list);
	void loadPossibleUnits(int ingredientID, ElementList *list);
	void loadProperties(IngredientPropertyList *list,int ingredientID=-1);
	void loadRecipe(Recipe *recipe,int recipeID=0);
	void loadRecipeAuthors(int recipeID, ElementList *list);
	void loadRecipeCategories(int recipeID, ElementList *list);
	void loadRecipeList(ElementList *list, ElementList *categoryList=0);
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

	QString recipeTitle(int recipeID);

	void removeAuthor(int categoryID);
	void removeCategory(int categoryID);
	void removeIngredient(int ingredientID);
	void removeProperty(int propertyID);
	void removePropertyFromIngredient(int ingredientID, int propertyID,int perUnitID);
	void removeRecipe(int id);
	void removeUnit(int unitID);
	void removeUnitFromIngredient(int ingredientID, int unitID);

	void saveRecipe(Recipe *recipe);
	void saveUnitRatio(const UnitRatio *ratio);

	double unitRatio(int unitID1, int unitID2);

	QCString escapeAndEncode(const QString &s);
	QString unescapeAndDecode(const QString &s);

	QString unitName(int unitID);
	bool checkIntegrity(void);

	void createTable(QString tableName);
	void splitCommands(QString& s,QStringList& sl);

	float databaseVersion(void);

private:
	void loadElementList(ElementList *elList, QSqlQuery *query);
	void loadPropertyElementList(ElementList *elList, QSqlQuery *query);
	void portOldDatabases(float version);
};




#endif
