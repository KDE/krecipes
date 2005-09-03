/***************************************************************************
*   Copyright (C) 2003                                                    *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *

*   Copyright (C) 2003-2005                                               *
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

#include <dcopclient.h>

#include "krecipesdbiface.h"

#include "datablocks/recipe.h"
#include "datablocks/recipelist.h"
#include "datablocks/elementlist.h"
#include "datablocks/ingredientpropertylist.h"
#include "datablocks/unitratiolist.h"
#include "datablocks/unit.h"

#define DEFAULT_DB_NAME "Krecipes"

/**
@author Unai Garro
*/

class KProcess;
class QTextStream;

class CategoryTree;

typedef struct
{
	QValueList <int> recipeIdList;
	IngredientList ilist;
}
RecipeIngredientList;

class RecipeDB: public QObject, virtual public KrecipesDBIface
{
	Q_OBJECT

public:
	RecipeDB();
	virtual ~RecipeDB();

	virtual void connect( bool create_db = true, bool create_tables = true ) = 0;

	void importSamples();

	bool backup( const QString &file ){ return backup(file,0); }
	bool backup( const QString &file, QString *errMsg = 0 );
	bool restore( const QString &file, QString *errMsg = 0 );

	// Error handling (passive)
	bool dbOK;
	QString dbErr;

	enum RecipeItems { None = 0, NamesOnly = 256, Photo = 1, Instructions = 2, Ingredients = 4, Authors = 8, Categories = 16, PrepTime = 32, Yield = 64, Title = 128, All = 0xFFFF ^ NamesOnly };

public slots:
	void cancelOperation(){ haltOperation = true; }

signals:
	void progressBegin(int,const QString &c=QString::null,const QString &t=QString::null,int rate=1);
	void progressDone();
	void progress();

	void authorCreated( const Element & );
	void authorRemoved( int id );

	void categoryCreated( const Element &, int parent_id );
	void categoryRemoved( int id );
	void categoryModified( const Element & );
	void categoryModified( int id, int parent_id );
	void categoriesMerged( int id1, int id2 );

	void ingredientCreated( const Element & );
	void ingredientRemoved( int id );

	void prepMethodCreated( const Element & );
	void prepMethodRemoved( int id );

	void propertyCreated( const IngredientProperty & );
	void propertyRemoved( int id );

	void unitCreated( const Unit & );
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

	virtual void addProperty( const QString &name, const QString &units ) = 0;
	virtual void addPropertyToIngredient( int ingredientID, int propertyID, double amount, int perUnitsID ) = 0;
	virtual void addUnitToIngredient( int ingredientID, int unitID ) = 0;

	virtual void changePropertyAmountToIngredient( int ingredientID, int propertyID, double amount, int per_units ) = 0;

	virtual void createNewAuthor( const QString &authorName ) = 0;
	virtual void createNewCategory( const QString &categoryName, int parent_id = -1 ) = 0;
	virtual void createNewIngGroup( const QString &name ) = 0;
	virtual void createNewIngredient( const QString &ingredientName ) = 0;
	virtual void createNewPrepMethod( const QString &prepMethodName ) = 0;
	virtual void createNewUnit( const QString &unitName, const QString &unitPlural ) = 0;
	virtual void createNewYieldType( const QString &type ) = 0;

	virtual void emptyData( void ) = 0;
	virtual void empty( void ) = 0;

	virtual int findExistingAuthorByName( const QString& name ) = 0;
	virtual int findExistingCategoryByName( const QString& name ) = 0;
	virtual int findExistingIngredientByName( const QString& name ) = 0;
	virtual int findExistingPrepByName( const QString& name ) = 0;
	virtual int findExistingPropertyByName( const QString& name ) = 0;
	virtual int findExistingRecipeByName( const QString& name ) = 0;
	virtual int findExistingUnitByName( const QString& name ) = 0;
	virtual int findExistingUnitsByName( const QString& name, int ingredientID = -1, ElementList *list = 0 ) = 0;
	virtual int findExistingYieldTypeByName( const QString& name ) = 0;
	virtual void findIngredientUnitDependancies( int ingredientID, int unitID, ElementList *recipes, ElementList *ingredientInfo ) = 0;
	virtual void findIngredientDependancies( int ingredientID, ElementList *recipes ) = 0;
	virtual void findPrepMethodDependancies( int prepMethodID, ElementList *recipes ) = 0;
	virtual void findUnitDependancies( int unitID, ElementList *properties, ElementList *recipes ) = 0;
	virtual void findUseOf_Ing_Unit_InRecipes( ElementList *results, int ingredientID, int unitID ) = 0;
	virtual void findUseOfIngInRecipes( ElementList *results, int ingredientID ) = 0;
	virtual void findUseOf_Unit_InRecipes( ElementList *results, int unitID ) = 0;
	virtual void findUseOf_Unit_InProperties( ElementList *results, int unitID ) = 0;

	void getIDList( const CategoryTree *categoryTree, QStringList &ids );
	virtual QString getUniqueRecipeTitle( const QString &recipe_title ) = 0;
	virtual void givePermissions( const QString &dbName, const QString &username, const QString &password = QString::null, const QString &clientHost = "localhost" ) = 0;

	void importUSDADatabase();

	virtual bool ingredientContainsProperty( int ingredientID, int propertyID, int perUnitsID ) = 0;
	virtual bool ingredientContainsUnit( int ingredientID, int unitID ) = 0;

	void initializeData( void );

	virtual int lastInsertID() = 0;

	virtual void loadAuthors( ElementList *list, int limit = -1, int offset = 0 ) = 0;
	virtual void loadCategories( CategoryTree *list, int limit = -1, int offset = 0, int parent_id = -1, bool recurse = true ) = 0;
	virtual void loadCategories( ElementList *list, int limit = -1, int offset = 0 ) = 0;
	virtual void loadIngredientGroups( ElementList *list ) = 0;
	virtual void loadIngredients( ElementList *list, int limit = -1, int offset = 0 ) = 0;
	virtual void loadPossibleUnits( int ingredientID, UnitList *list ) = 0;
	virtual void loadPrepMethods( ElementList *list, int limit = -1, int offset = 0 ) = 0;
	virtual void loadProperties( IngredientPropertyList *list, int ingredientID = -2 ) = 0; // Loads the list of possible properties by default, all the ingredient properties with -1, and the ingredients of given property if id>=0
	void loadRecipe( Recipe *recipe, int items, int id );
	/** Load all recipes with the ids in @param ids into the @ref RecipeList @param recipes */
	virtual void loadRecipes( RecipeList *, int items = All, QValueList<int> ids = QValueList<int>()/*, KProgressDialog *progress_dlg = 0*/ ) = 0;
	virtual void loadRecipeList( ElementList *list, int categoryID = -1, bool recursive = false ) = 0;
	virtual void loadUncategorizedRecipes( ElementList *list ) = 0;
	virtual void loadUnits( UnitList *list, int limit = -1, int offset = 0 ) = 0;
	virtual void loadUnitRatios( UnitRatioList *ratioList ) = 0;
	virtual void loadYieldTypes( ElementList *list, int limit = -1, int offset = 0 ) = 0;

	/** Change all instances of authors with id @param id2 to @param id1 */
	virtual void mergeAuthors( int id1, int id2 ) = 0;

	/** Change all instances of categories with id @param id2 to @param id1 */
	virtual void mergeCategories( int id1, int id2 ) = 0;

	/** Change all instances of ingredients with id @param id2 to @param id1 */
	virtual void mergeIngredients( int id1, int id2 ) = 0;

	/** Change all instances of units with id @param id2 to @param id1 */
	virtual void mergeUnits( int id1, int id2 ) = 0;

	/** Change all instances of prep methods with id @param id2 to @param id1 */
	virtual void mergePrepMethods( int id1, int id2 ) = 0;

	virtual void mergeProperties( int id1, int id2 ) = 0;

	/**
	* set newLabel for ingredientID
	*/
	virtual void modIngredient( int ingredientID, const QString &newLabel ) = 0;
	/**
	* set newLabel for unitID
	*/
	virtual void modUnit( int unitID, const QString &newName, const QString &newPlural ) = 0;
	/**
	* set newLabel for categoryID
	*/
	virtual void modCategory( int categoryID, const QString &newLabel ) = 0;
	virtual void modCategory( int categoryID, int new_parent_id ) = 0;
	/**
	* set newLabel for authorID
	*/
	virtual void modAuthor( int authorID, const QString &newLabel ) = 0;

	virtual void modPrepMethod( int prepMethodID, const QString &newLabel ) = 0;

	virtual void modProperty( int propertyID, const QString &newLabel ) = 0;

	virtual QString recipeTitle( int recipeID ) = 0;

	virtual void removeAuthor( int categoryID ) = 0;
	virtual void removeCategory( int categoryID ) = 0;
	virtual void removeIngredient( int ingredientID ) = 0;
	virtual void removePrepMethod( int prepMethodID ) = 0;
	virtual void removeProperty( int propertyID ) = 0;
	virtual void removePropertyFromIngredient( int ingredientID, int propertyID, int perUnitID ) = 0;
	virtual void removeRecipe( int id ) = 0;
	virtual void removeRecipeFromCategory( int ingredientID, int categoryID ) = 0;
	virtual void removeUnit( int unitID ) = 0;
	virtual void removeUnitFromIngredient( int ingredientID, int unitID ) = 0;

	virtual void saveRecipe( Recipe *recipe ) = 0;
	virtual void saveUnitRatio( const UnitRatio *ratio ) = 0;
	virtual void search( RecipeList *list, int items,
			const QStringList &titleKeywords, bool requireAllTitleWords,
			const QStringList &instructionsKeywords, bool requireAllInstructionWords,
			const QStringList &ingsOr,
			const QStringList &catsOr,
			const QStringList &authorsOr,
			const QTime &time, int prep_param,
			int servings, int servings_param ) = 0;

	virtual double unitRatio( int unitID1, int unitID2 ) = 0;

	virtual QCString escapeAndEncode( const QString &s ) const = 0;
	virtual QString unescapeAndDecode( const QString &s ) const = 0;

	virtual QString categoryName( int ID ) = 0;
	virtual IngredientProperty propertyName( int ID ) = 0;
	virtual Unit unitName( int ID ) = 0;

	virtual int categoryTopLevelCount() = 0;
	virtual int getCount( const QString &table_name ) = 0;
	int authorCount();
	int ingredientCount();
	int prepMethodCount();
	int unitCount();
	int categoryCount();

	virtual bool checkIntegrity( void ) = 0;

	virtual void createTable( const QString &tableName ) = 0;
	virtual void splitCommands( QString& s, QStringList& sl ) = 0;

	virtual float databaseVersion( void ) = 0;

	int maxAuthorNameLength() const
	{
		return 50;
	}
	int maxCategoryNameLength() const
	{
		return 40;
	}
	int maxIngredientNameLength() const
	{
		return 50;
	}
	int maxIngGroupNameLength() const
	{
		return 50;
	}
	int maxRecipeTitleLength() const
	{
		return 200;
	}
	int maxUnitNameLength() const
	{
		return 20;
	}
	int maxPrepMethodNameLength() const
	{
		return 20;
	}
	int maxPropertyNameLength() const
	{
		return 20;
	}
	int maxYieldTypeLength() const
	{
		return 20;
	}

	virtual bool ok()
	{
		return ( dbOK );
	}
	virtual QString err()
	{
		return ( dbErr );
	}

protected:
	virtual void portOldDatabases( float version ) = 0;
	virtual QStringList backupCommand() const = 0;
	virtual QStringList restoreCommand() const = 0;

	//Use these with caution: SQL for one backend might not work on another!
	void execSQL( QTextStream &stream );
	virtual void execSQL( const QString & ) = 0;

	QString buildSearchQuery( const QStringList &titleKeywords, bool requireAllTitleWords,
		const QStringList &instructionsKeywords, bool requireAllInstructionsWords,
		const QStringList &ingsOr,
		const QStringList &catsOr,
		const QStringList &authorsOr,
		const QTime &time, int prep_param,
		int servings, int servings_param ) const;

	double latestDBVersion() const;
	QString krecipes_version() const;

private:
	QTextStream *dumpStream;
	bool haltOperation;

private slots:
	void processDumpOutput( KProcess *, char *buffer, int buflen );
};




#endif
