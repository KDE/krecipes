/***************************************************************************
 *   Copyright (C) 2003                                                    *
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

#ifndef RECIPEDB_H
#define RECIPEDB_H

#include <qobject.h>
#include <qstring.h>
#include <qvaluelist.h>

#include "recipe.h"
#include "datablocks/recipelist.h"
#include "elementlist.h"
#include "ingredientpropertylist.h"
#include "unitratiolist.h"

#define DEFAULT_DB_NAME "Krecipes"

/**
@author Unai Garro
*/

class KProgressDialog;

class CategoryTree;

typedef struct
{
QValueList <int> recipeIdList;
IngredientList ilist;
} RecipeIngredientList;

class RecipeDB:public QObject {

Q_OBJECT

protected:
	RecipeDB():QObject(){dbOK=false; dbErr="";}
	
	double latestDBVersion() const{ return 0.62; }
	QString krecipes_version() const;

public:
	virtual ~RecipeDB(){};
	
	virtual void connect(bool create=true) = 0;
	
	void importSamples();
	
	// Error handling (passive)
	bool dbOK;
	QString dbErr;
	
signals:
	void authorCreated( const Element & );
	void authorRemoved( int id );

	void categoryCreated( const Element &, int parent_id );
	void categoryRemoved( int id );
	void categoryModified( const Element & );
	void categoryModified( int id, int parent_id );
	void categoriesMerged(int id1, int id2);

	void ingredientCreated( const Element & );
	void ingredientRemoved( int id );

	void prepMethodCreated( const Element & );
	void prepMethodRemoved( int id );

	void propertyCreated( const IngredientProperty & );
	void propertyRemoved( int id );

	void unitCreated( const Element & );
	void unitRemoved( int id );

	void recipeCreated( const Element &, const ElementList &categories );
	void recipeRemoved( int id );
	void recipeRemoved( int id, int cat_id );
	void recipeModified( const Element &, const ElementList &categories );

	// Public methods
public:
	/** Returns a database object of the given type or NULL upon failure.
	  * This function should be called to create a new database, rather
	  * than directly calling the constructor of a specific backend.
	  */
	static RecipeDB* createDatabase( const QString &dbType,
	  const QString &host,
	  const QString &user,
	  const QString &pass,
	  const QString &DBname,
	  const QString &file = QString::null );

	/** Convenience method.  Calls the above with arguments from KConfig. */
	static RecipeDB* createDatabase( const QString &dbType, const QString &file = QString::null );

	virtual void addAuthorToRecipe(int recipeID, int categoryID)=0;
	virtual void addCategoryToRecipe(int recipeID, int categoryID)=0;


	virtual void addProperty(const QString &name, const QString &units)=0;
	virtual void addPropertyToIngredient(int ingredientID,int propertyID,double amount, int perUnitsID)=0;
	virtual void addUnitToIngredient(int ingredientID,int unitID)=0;

	virtual void changePropertyAmountToIngredient(int ingredientID,int propertyID,double amount,int per_units)=0;

	virtual void createNewAuthor(const QString &authorName)=0;
	virtual void createNewCategory(const QString &categoryName,int parent_id=-1)=0;
	virtual void createNewIngGroup( const QString &name )=0;
	virtual void createNewIngredient(const QString &ingredientName)=0;
	virtual void createNewPrepMethod(const QString &prepMethodName)=0;
	virtual void createNewUnit(const QString &unitName)=0;

	virtual void emptyData(void)=0;

	virtual int  findExistingAuthorByName(const QString& name)=0;
	virtual int  findExistingCategoryByName(const QString& name)=0;
	virtual int  findExistingIngredientByName(const QString& name)=0;
	virtual int  findExistingPrepByName(const QString& name)=0;
	virtual int  findExistingPropertyByName(const QString& name)=0;
	virtual int  findExistingRecipeByName(const QString& name)=0;
	virtual int  findExistingUnitByName(const QString& name)=0;
	virtual int  findExistingUnitsByName(const QString& name,int ingredientID=-1, ElementList *list=0)=0;
	virtual void findIngredientUnitDependancies(int ingredientID,int unitID,ElementList *recipes,ElementList *ingredientInfo)=0;
	virtual void findIngredientDependancies(int ingredientID,ElementList *recipes)=0;
	virtual void findPrepMethodDependancies(int prepMethodID,ElementList *recipes)=0;
	virtual void findUnitDependancies(int unitID,ElementList *properties,ElementList *recipes)=0;
	virtual void findUseOf_Ing_Unit_InRecipes(ElementList *results, int ingredientID, int unitID)=0;
	virtual void findUseOfIngInRecipes(ElementList *results,int ingredientID)=0;
	virtual void findUseOf_Unit_InRecipes(ElementList *results, int unitID)=0;
	virtual void findUseOf_Unit_InProperties(ElementList *results, int unitID)=0;

	virtual QString getUniqueRecipeTitle( const QString &recipe_title )=0;
	virtual void givePermissions(const QString &dbName,const QString &username, const QString &password=QString::null, const QString &clientHost="localhost")=0;

	void importUSDADatabase( KProgressDialog *progress_dlg = 0 );

	virtual bool ingredientContainsProperty(int ingredientID, int propertyID, int perUnitsID)=0;
	virtual bool ingredientContainsUnit(int ingredientID, int unitID)=0;

	virtual void initializeData(void)=0;

	virtual int lastInsertID()=0;

	virtual void loadAllRecipeIngredients(RecipeIngredientList *list,bool withNames=true)=0;
	virtual void loadAuthors(ElementList *list)=0;
	virtual void loadCategories(CategoryTree *list,int parent_id=-1)=0;
	virtual void loadCategories(ElementList *list)=0;
	virtual void loadIngredientGroups(ElementList *list)=0;
	virtual void loadIngredients(ElementList *list)=0;
	virtual void loadPossibleUnits(int ingredientID, ElementList *list)=0;
	virtual void loadPrepMethods( ElementList *list)=0;
	virtual void loadProperties(IngredientPropertyList *list,int ingredientID=-2)=0; // Loads the list of possible properties by default, all the ingredient properties with -1, and the ingredients of given property if id>=0
	virtual void loadRecipe(Recipe *recipe,int recipeID=0)=0;
	/** Load all recipes with the ids in @param ids into the @ref RecipeList @param recipes */
	void loadRecipes(RecipeList *recipes,const QValueList<int> &ids,KProgressDialog *progress_dlg = 0); //note: isn't virtual because this can be done with loadRecipe()
	virtual void loadRecipeAuthors(int recipeID, ElementList *list)=0;
	virtual void loadRecipeCategories(int recipeID, ElementList *categoryList)=0;
	virtual void loadRecipeDetails(RecipeList *rlist,bool loadIngredients=false,bool loadCategories=false, bool loadIngredientNames=false,bool loadAuthors=false)=0;// Read only the recipe details (no instructions, no photo,...) and when loading ingredients and categories, no names by default, just IDs
	virtual void loadRecipeList(ElementList *list,int categoryID=0,QPtrList <int>*recipeCategoryList=0)=0;
	virtual void loadUnits(ElementList *list)=0;
	virtual void loadUnitRatios(UnitRatioList *ratioList)=0;

	/** Change all instances of authors with id @param id2 to @param id1 */
	virtual void mergeAuthors(int id1, int id2)=0;

	/** Change all instances of categories with id @param id2 to @param id1 */
	virtual void mergeCategories(int id1, int id2)=0;

	/** Change all instances of ingredients with id @param id2 to @param id1 */
	virtual void mergeIngredients(int id1, int id2)=0;

	/** Change all instances of units with id @param id2 to @param id1 */
	virtual void mergeUnits(int id1, int id2)=0;

	/** Change all instances of prep methods with id @param id2 to @param id1 */
	virtual void mergePrepMethods(int id1, int id2)=0;

	virtual void mergeProperties(int id1, int id2)=0;

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
	virtual void modCategory(int categoryID, int new_parent_id)=0;
  /**
  * set newLabel for authorID
  */
	virtual void modAuthor(int authorID, QString newLabel)=0;
	
	virtual void modPrepMethod(int prepMethodID, const QString &newLabel)=0;

	virtual void modProperty(int propertyID, const QString &newLabel)=0;

	virtual QString recipeTitle(int recipeID)=0;

	virtual void removeAuthor(int categoryID)=0;
	virtual void removeCategory(int categoryID)=0;
	virtual void removeIngredient(int ingredientID)=0;
	virtual void removePrepMethod(int prepMethodID)=0;
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

	virtual QString categoryName(int ID)=0;
	virtual IngredientProperty propertyName(int ID)=0;
	virtual QString unitName(int ID)=0;

	virtual bool checkIntegrity(void)=0;

	virtual void createTable(QString tableName)=0;
	virtual void splitCommands(QString& s,QStringList& sl)=0;

	virtual float databaseVersion(void)=0;

	int maxAuthorNameLength() const{ return 50; }
	int maxCategoryNameLength() const{ return 40; }
	int maxIngredientNameLength() const{ return 50; }
	int maxIngGroupNameLength() const{ return 50; }
	int maxRecipeTitleLength() const{ return 200; }
	int maxUnitNameLength() const{ return 20; }
	int maxPrepMethodNameLength() const{ return 20; }
	int maxPropertyNameLength() const{ return 20; }

protected:
	virtual void portOldDatabases(float version)=0;
public:
	virtual bool ok(){return (dbOK);}
	virtual QString err(){return (dbErr);}
};




#endif
