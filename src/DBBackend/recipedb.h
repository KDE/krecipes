/***************************************************************************
 *   Copyright (C) 2003 by krecipes.sourceforge.net authors                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef RECIPEDB_H
#define RECIPEDB_H

#include <qsqldatabase.h>
#include <qstring.h>
#include "recipe.h"
#include "datablocks/recipelist.h"
#include "elementlist.h"
#include "ingredientpropertylist.h"
#include "unitratiolist.h"

/**
@author Unai Garro
*/
class RecipeDB{

protected:
	RecipeDB(QString host, QString user, QString pass, QString DBname,bool init){};
	virtual ~RecipeDB(){};
public:
	virtual void addAuthorToRecipe(int recipeID, int categoryID)=0;
	virtual void addCategoryToRecipe(int recipeID, int categoryID)=0;


	virtual void addProperty(QString &name, QString &units)=0;
	virtual void addPropertyToIngredient(int ingredientID,int propertyID,double amount, int perUnitsID)=0;
	virtual void addUnitToIngredient(int ingredientID,int unitID)=0;

	virtual void changePropertyAmountToIngredient(int ingredientID,int propertyID,double amount,int per_units)=0;

	virtual void createNewAuthor(const QString &authorName)=0;
	virtual void createNewCategory(QString &categoryName)=0;
	virtual void createNewIngredient(QString ingredientName)=0;
	virtual void createNewUnit(QString &unitName)=0;

	virtual void emptyData(void)=0;

	virtual int  findExistingAuthorByName(const QString& name)=0;
	virtual int  findExistingCategoryByName(const QString& name)=0;
	virtual int  findExistingIngredientByName(const QString& name)=0;
	virtual int  findExistingRecipeByName(const QString& name)=0;
	virtual int  findExistingUnitByName(const QString& name)=0;
	virtual int  findExistingUnitsByName(const QString& name,int ingredientID=-1, ElementList *list=0)=0;
	virtual void findIngredientUnitDependancies(int ingredientID,int unitID,ElementList *recipes,ElementList *ingredientInfo)=0;
	virtual void findIngredientDependancies(int ingredientID,ElementList *recipes)=0;
	virtual void findUnitDependancies(int unitID,ElementList *properties,ElementList *recipes)=0;
	virtual void findUseOf_Ing_Unit_InRecipes(ElementList *results, int ingredientID, int unitID)=0;
	virtual void findUseOfIngInRecipes(ElementList *results,int ingredientID)=0;
	virtual void findUseOf_Unit_InRecipes(ElementList *results, int unitID)=0;
	virtual void findUseOf_Unit_InProperties(ElementList *results, int unitID)=0;

	virtual QString getUniqueRecipeTitle( const QString &recipe_title )=0;
	virtual void givePermissions(const QString &dbName,const QString &username, const QString &password=QString::null, const QString &clientHost="localhost")=0;


	virtual bool ingredientContainsProperty(int ingredientID, int propertyID, int perUnitsID)=0;
	virtual bool ingredientContainsUnit(int ingredientID, int unitID)=0;

	virtual void initializeDB(void)=0;
	virtual void initializeData(void)=0;

	virtual int lastInsertID()=0;

	virtual void loadAuthors(ElementList *list)=0;
	virtual void loadCategories(ElementList *list)=0;
	virtual void loadIngredients(ElementList *list)=0;
	virtual void loadPossibleUnits(int ingredientID, ElementList *list)=0;
	virtual void loadProperties(IngredientPropertyList *list,int ingredientID=-1)=0;
	virtual void loadRecipe(Recipe *recipe,int recipeID=0)=0;
	virtual void loadRecipes(RecipeList *rlist,bool getInstructions=false,bool getPhoto=false)=0;
	virtual void loadRecipeAuthors(int recipeID, ElementList *list)=0;
	virtual void loadRecipeCategories(int recipeID, ElementList *categoryList)=0;
	virtual void loadRecipeList(ElementList *list,int categoryID=0,QPtrList <int>*recipeCategoryList=0)=0;
	virtual void loadUnits(ElementList *list)=0;
	virtual void loadUnitRatios(UnitRatioList *ratioList)=0;

  /**
  * set newLabel for ingredientID
  */
	virtual void modIngredient(int ingredientID, QString newLabel)=0;
  /**
  * set newLabel for unitID
  */
	virtual void modUnit(int unitID, QString newLabel)=0;
  /**
  * set newLabel for categoryID
  */
	virtual void modCategory(int categoryID, QString newLabel)=0;
  /**
  * set newLabel for authorID
  */
	virtual void modAuthor(int authorID, QString newLabel)=0;

	virtual QString recipeTitle(int recipeID)=0;

	virtual void removeAuthor(int categoryID)=0;
	virtual void removeCategory(int categoryID)=0;
	virtual void removeIngredient(int ingredientID)=0;
	virtual void removeProperty(int propertyID)=0;
	virtual void removePropertyFromIngredient(int ingredientID, int propertyID,int perUnitID)=0;
	virtual void removeRecipe(int id)=0;
	virtual void removeRecipeFromCategory(int ingredientID, int categoryID)=0;
	virtual void removeUnit(int unitID)=0;
	virtual void removeUnitFromIngredient(int ingredientID, int unitID)=0;

	virtual void saveRecipe(Recipe *recipe)=0;
	virtual void saveUnitRatio(const UnitRatio *ratio)=0;

	virtual double unitRatio(int unitID1, int unitID2)=0;

	virtual QCString escapeAndEncode(const QString &s)=0;
	virtual QString unescapeAndDecode(const QString &s)=0;

	virtual QString unitName(int unitID)=0;
	virtual bool checkIntegrity(void)=0;

	virtual void createTable(QString tableName)=0;
	virtual void splitCommands(QString& s,QStringList& sl)=0;

	virtual float databaseVersion(void)=0;

	int maxAuthorNameLength() const{ return 20; }
	int maxCategoryNameLength() const{ return 20; }
	int maxIngredientNameLength() const{ return 50; }
	int maxRecipeTitleLength() const{ return 200; }
	int maxUnitNameLength() const{ return 20; }

protected:
	virtual void portOldDatabases(float version)=0;
};




#endif
