/***************************************************************************
 *   Copyright (C) 2004 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#ifndef QSQLRECIPEDB_H
#define QSQLRECIPEDB_H

#define DEFAULT_DB_NAME "Krecipes"

#include <qglobal.h>
#include <qobject.h>
#include <qsqldatabase.h>
#include <qimage.h>
#include <qfileinfo.h>
#include <qregexp.h>
#include <qstring.h>

#include "recipe.h"
#include "elementlist.h"
#include "ingredientpropertylist.h"
#include "unitratiolist.h"
#include "recipedb.h"


/**
@author Unai Garro, Jason Kivlighn
*/
class QSqlRecipeDB : public RecipeDB
{

Q_OBJECT

protected:
	virtual QString qsqlDriver() const = 0;
	virtual void createDB(void) = 0;
	virtual void portOldDatabases(float version);
	virtual void storePhoto(int recipeID, const QByteArray &data);
	virtual void loadPhoto(int recipeID, QPixmap &photo);

	/** Return the next id for the given table and column.
	  * If the database supports getting this afterwards,
	  * leave the default implementation which returns -1.
	  *
	  * Note: Only call when an insert is actually going to take place.
	  *       This function will increment the sequence counter.
	  */
	virtual int getNextInsertID( const QString &/*table*/, const QString &/*column*/ ){return -1;}

	QSqlDatabase *database;
	QString DBuser;
	QString DBpass;
	QString DBhost;

public:
	QSqlRecipeDB(const QString host, const QString user=QString::null, const QString pass=QString::null, const QString DBName=DEFAULT_DB_NAME);
	~QSqlRecipeDB(void);

	void connect(bool init=true);

	void addAuthorToRecipe(int recipeID, int categoryID);
	void addCategoryToRecipe(int recipeID, int categoryID);


	void addProperty(const QString &name, const QString &units);
	void addPropertyToIngredient(int ingredientID,int propertyID,double amount, int perUnitsID);
	void addUnitToIngredient(int ingredientID,int unitID);

	void changePropertyAmountToIngredient(int ingredientID,int propertyID,double amount,int per_units);

	void createNewAuthor(const QString &authorName);
	void createNewCategory(const QString &categoryName,int parent_id=-1);
	void createNewIngredient(const QString &ingredientName);
	void createNewPrepMethod(const QString &prepMethodName);
	void createNewUnit(const QString &unitName);

	void emptyData(void);

	int  findExistingAuthorByName(const QString& name);
	int  findExistingCategoryByName(const QString& name);
	int  findExistingIngredientByName(const QString& name);
	int  findExistingPrepByName(const QString& name);
	int  findExistingRecipeByName(const QString& name);
	int  findExistingUnitByName(const QString& name);
	int  findExistingPropertyByName(const QString& name);
	int  findExistingUnitsByName(const QString& name,int ingredientID=-1, ElementList *list=0);
	void findIngredientUnitDependancies(int ingredientID,int unitID,ElementList *recipes,ElementList *ingredientInfo);
	void findIngredientDependancies(int ingredientID,ElementList *recipes);
	void findPrepMethodDependancies(int prepMethodID,ElementList *recipes);
	void findUnitDependancies(int unitID,ElementList *properties,ElementList *recipes);
	void findUseOf_Ing_Unit_InRecipes(ElementList *results, int ingredientID, int unitID);
	void findUseOfIngInRecipes(ElementList *results,int ingredientID);
	void findUseOf_Unit_InRecipes(ElementList *results, int unitID);
	void findUseOf_Unit_InProperties(ElementList *results, int unitID);

	QString getUniqueRecipeTitle( const QString &recipe_title );
	void givePermissions(const QString &dbName,const QString &username, const QString &password=QString::null, const QString &clientHost="localhost");


	bool ingredientContainsProperty(int ingredientID, int propertyID, int perUnitsID);
	bool ingredientContainsUnit(int ingredientID, int unitID);

	//void initializeDB(void);
	//void initializeData(void);

	void loadAllRecipeIngredients(RecipeIngredientList *list,bool withNames=true);
	void loadAuthors(ElementList *list);
	void loadCategories(CategoryTree *list,int parent_id=-1);
	void loadCategories(ElementList *list);
	void loadIngredients(ElementList *list);
	void loadPossibleUnits(int ingredientID, ElementList *list);
	void loadPrepMethods( ElementList *list);
	void loadProperties(IngredientPropertyList *list,int ingredientID=-2); // Loads the list of possible properties by default, all the ingredient properties with -1, and the ingredients of given property if id>=0
	void loadRecipe(Recipe *recipe,int recipeID=0);
	void loadRecipeAuthors(int recipeID, ElementList *list);
	void loadRecipeCategories(int recipeID, ElementList *list);
	void loadRecipeDetails(RecipeList *rlist,bool loadIngredients=false, bool loadCategories=false,bool loadIngredientNames=false,bool loadAuthors=false); // Read only the recipe details (no instructions, no photo,...) and when loading ingredients and categories, no names by default, just IDs
	void loadRecipeList(ElementList *list, int categoryID=0);
	void loadRecipeList(ElementList *list,int categoryID=0,QPtrList <int>*recipeCategoryList=0);
	void loadUnits(ElementList *list);
	void loadUnitRatios(UnitRatioList *ratioList);

	void mergeAuthors(int id1, int id2);
	void mergeCategories(int id1, int id2);
	void mergeIngredients(int id1, int id2);
	void mergeUnits(int id1, int id2);
	void mergePrepMethods(int id1, int id2);
	void mergeProperties(int id1, int id2);

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
	void modCategory(int categoryID, int new_parent_id);
  /**
  * set newLabel for authorID
  */
	void modAuthor(int authorID, QString newLabel);
	
	void modPrepMethod(int prepMethodID, const QString &newLabel);

	void modProperty(int propertyID, const QString &newLabel);

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

	QCString escapeAndEncode(const QString &s);
	QString unescapeAndDecode(const QString &s);

	QString categoryName(int ID);
	IngredientProperty propertyName(int ID);
	QString unitName(int ID);

	bool checkIntegrity(void);

	void splitCommands(QString& s,QStringList& sl);

	float databaseVersion(void);

private:
	void loadElementList(ElementList *elList, QSqlQuery *query);
	void loadPropertyElementList(ElementList *elList, QSqlQuery *query);
	QString getNextInsertIDStr( const QString &table, const QString &column );

	QString DBname;
};




#endif
