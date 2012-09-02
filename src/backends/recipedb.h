/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003-2006 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef RECIPEDB_H
#define RECIPEDB_H

#include <limits.h> /* needed for INT_MAX */

#include <QObject>
#include <QString>
#include <QList>

class QEventLoop;
class QTimer;

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

#include <KProcess>
class QTextStream;

class CategoryTree;
class RecipeSearchParameters;
class Weight;
typedef QList<Weight> WeightList;


class RecipeDB: public QObject
{
	Q_OBJECT

public:
	enum Error {NoError = 0, NoDriverFound, RefusedByServer, NewerDbVersion, FixDbFailed};

	RecipeDB();
	virtual ~RecipeDB();

	virtual RecipeDB::Error connect( bool create_db = true, bool create_tables = true ) = 0;

	void importSamples();

	bool backupdb( const QString &file ){ return backup(file,0); }
	bool backup( const QString &file, QString *errMsg = 0 );
	bool restore( const QString &file, QString *errMsg = 0 );

	// Error handling (passive)
	bool dbOK;
	QString dbErr;

	typedef int IdType;
	static const IdType InvalidId = -1;

	enum RecipeItems {
		None = 0,
		NamesOnly = 256,
		Noatime = 1024,
		Photo = 1,
		Instructions = 2,
		Ingredients = 4,
		Authors = 8,
		Categories = 16,
		PrepTime = 32,
		Yield = 64,
		Title = 128,
		Meta = 512,
		Ratings = 2048,
		Properties = 4096,
		IngredientAmounts = 8192,
		All = 0xFFFF ^ NamesOnly ^ Noatime
	};

	enum ConversionStatus {
		Success,
		MissingUnitConversion,
		MissingIngredientWeight,
		MissingIngredient,
		InvalidTypes,
		MismatchedPrepMethod,
		MismatchedPrepMethodUsingApprox
	};

public slots:
	void cancelOperation();

signals:
	void progressBegin(int,const QString &c=QString(),const QString &t=QString(),int rate=1);
	void progressDone();
	void progress();

	void authorCreated( const Element & );
	void authorRemoved( int id );

	void categoryCreated( const Element &, int parent_id );
	void categoryRemoved( int id );
	void categoryModified( const Element & );
	void categoryModified( int id, int parent_id );
	void categoriesMerged( int id1, int id2 );

	void ingGroupCreated( const Element & );
	void ingGroupRemoved( int id );

	void ingredientCreated( const Element & );
	void ingredientRemoved( int id );

	void prepMethodCreated( const Element & );
	void prepMethodRemoved( int id );

	void propertyCreated( const IngredientProperty & );
	void propertyRemoved( int id );

	void unitCreated( const Unit & );
	void unitRemoved( int id );

	void ratingCriteriaCreated( const Element & );

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
	                                 int port,
	                                 const QString &file = QString() );

	/** Convenience method.  Calls the above with arguments from KConfig. */
	static RecipeDB* createDatabase( const QString &dbType, const QString &file = QString() );
	/** Convenience method.  Calls the above with arguments from KConfig. */
	static RecipeDB* createDatabase();

	virtual void transaction() = 0;
	virtual void commit() = 0;

	virtual void addIngredientWeight( const Weight & ) = 0;
	virtual RecipeDB::IdType addProperty( const QString &name, const QString &units ) = 0;
	virtual void addPropertyToIngredient( int ingredientID, int propertyID, double amount, int perUnitsID ) = 0;
	virtual void addUnitToIngredient( int ingredientID, int unitID ) = 0;

	virtual void categorizeRecipe( int recipeID, const ElementList &categoryList ) = 0;
	virtual void changePropertyAmountToIngredient( int ingredientID, int propertyID, double amount, int per_units ) = 0;

	virtual RecipeDB::IdType createNewAuthor( const QString &authorName ) = 0;
	virtual RecipeDB::IdType createNewCategory( const QString &categoryName, int parent_id = -1 ) = 0;
	virtual RecipeDB::IdType createNewIngGroup( const QString &name ) = 0;
	virtual RecipeDB::IdType createNewIngredient( const QString &ingredientName ) = 0;
	virtual RecipeDB::IdType createNewPrepMethod( const QString &prepMethodName ) = 0;
	virtual RecipeDB::IdType createNewRating( const QString &name ) = 0;
	virtual RecipeDB::IdType createNewUnit( const Unit &unit ) = 0;
	virtual RecipeDB::IdType createNewYieldType( const QString &type ) = 0;

	virtual void emptyData( void ) = 0;
	virtual void empty( void ) = 0;

	virtual int findExistingAuthorByName( const QString& name ) = 0;
	virtual int findExistingCategoryByName( const QString& name ) = 0;
	virtual int findExistingIngredientGroupByName( const QString& name ) = 0;
	virtual int findExistingIngredientByName( const QString& name ) = 0;
	virtual int findExistingPrepByName( const QString& name ) = 0;
	virtual int findExistingPropertyByName( const QString& name ) = 0;
	virtual int findExistingRatingByName( const QString& name ) = 0;
	virtual int findExistingRecipeByName( const QString& name ) = 0;
	virtual int findExistingUnitByName( const QString& name ) = 0;
	virtual int findExistingYieldTypeByName( const QString& name ) = 0;
	virtual void findIngredientUnitDependancies( int ingredientID, int unitID, ElementList *recipes, ElementList *ingredientInfo ) = 0;
	virtual void findIngredientDependancies( int ingredientID, ElementList *recipes ) = 0;
	virtual void findPrepMethodDependancies( int prepMethodID, ElementList *recipes ) = 0;
	virtual void findUnitDependancies( int unitID, ElementList *properties, ElementList *recipes, ElementList *weights ) = 0;
	virtual void findUseOfIngGroupInRecipes( ElementList *results, int groupID ) = 0;
	virtual void findUseOfCategoryInRecipes( ElementList *results, int catID ) = 0;
	virtual void findUseOfAuthorInRecipes( ElementList *results, int authorID ) = 0;

	void getIDList( const CategoryTree *categoryTree, QStringList &ids );
	virtual QString getUniqueRecipeTitle( const QString &recipe_title ) = 0;
	virtual void givePermissions( const QString &dbName, const QString &username, const QString &password = QString(), const QString &clientHost = "localhost" ) = 0;

	virtual void importUSDADatabase();

	virtual bool ingredientContainsProperty( int ingredientID, int propertyID, int perUnitsID ) = 0;
	virtual bool ingredientContainsUnit( int ingredientID, int unitID ) = 0;

	virtual void initializeData( void );

	virtual int loadAuthors( ElementList *list, int limit = -1, int offset = 0 ) = 0;
	virtual void loadCategories( CategoryTree *list, int limit = -1, int offset = 0, int parent_id = -1, bool recurse = true ) = 0;
	void loadCachedCategories( CategoryTree **list, int limit, int offset, int parent_id, bool recurse );
	virtual void loadCategories( ElementList *list, int limit = -1, int offset = 0 ) = 0;
	virtual void loadIngredientGroups( ElementList *list ) = 0;
	virtual void loadIngredients( ElementList *list, int limit = -1, int offset = 0 ) = 0;
	virtual void loadPossibleUnits( int ingredientID, UnitList *list ) = 0;
	virtual void loadPrepMethods( ElementList *list, int limit = -1, int offset = 0 ) = 0;
	virtual void loadProperties( IngredientPropertyList *list, int ingredientID = -2 ) = 0; // Loads the list of possible properties by default, all the ingredient properties with -1, and the ingredients of given property if id>=0
	void loadRecipe( Recipe *recipe, int items, int id );

	virtual void loadRatingCriterion( ElementList *list, int limit = -1, int offset = 0 ) = 0;
	/** Load all recipes with the ids in @param ids into the @ref RecipeList @param recipes */
	virtual void loadRecipes( RecipeList *, int items = All, QList<int> ids = QList<int>()/*, KProgressDialog *progress_dlg = 0*/ ) = 0;
	virtual void loadRecipeList( ElementList *list, int categoryID = -1, bool recursive = false ) = 0;
	virtual void loadUncategorizedRecipes( ElementList *list ) = 0;
	virtual void loadUnits( UnitList *list, Unit::Type = Unit::All, int limit = -1, int offset = 0 ) = 0;
	virtual void loadUnitRatios( UnitRatioList *ratioList, Unit::Type ) = 0;
	virtual void loadYieldTypes( ElementList *list, int limit = -1, int offset = 0 ) = 0;

	/** Change all instances of authors with id @param id2 to @param id1 */
	virtual void mergeAuthors( int id1, int id2 ) = 0;

	/** Change all instances of categories with id @param id2 to @param id1 */
	virtual void mergeCategories( int id1, int id2 ) = 0;

	virtual void mergeIngredientGroups( int id1, int id2 ) = 0;

	/** Change all instances of ingredients with id @param id2 to @param id1 */
	virtual void mergeIngredients( int id1, int id2 ) = 0;

	/** Change all instances of units with id @param id2 to @param id1 */
	virtual void mergeUnits( int id1, int id2 ) = 0;

	/** Change all instances of prep methods with id @param id2 to @param id1 */
	virtual void mergePrepMethods( int id1, int id2 ) = 0;

	virtual void mergeProperties( int id1, int id2 ) = 0;


	virtual void modIngredientGroup( int ingredientID, const QString &newLabel ) = 0;
	/**
	* set newLabel for ingredientID
	*/
	virtual void modIngredient( int ingredientID, const QString &newLabel ) = 0;
	/**
	* set newLabel for unitID
	*/
	virtual void modUnit( const Unit &unit ) = 0;
	/**
	* set newLabel for categoryID
	*/
	virtual void modCategory( int categoryID, const QString &newLabel ) = 0;
	virtual void modCategory( int categoryID, int new_parent_id, bool emit_cat_modified = true ) = 0;
	/**
	* set newLabel for authorID
	*/
	virtual void modAuthor( int authorID, const QString &newLabel ) = 0;

	virtual void modPrepMethod( int prepMethodID, const QString &newLabel ) = 0;

	virtual void modProperty( int propertyID, const QString &newLabel, const QString &unit = QString() ) = 0;

	virtual QString recipeTitle( int recipeID ) = 0;

	virtual void removeAuthor( int categoryID ) = 0;
	virtual void removeCategory( int categoryID ) = 0;
	virtual void removeIngredientGroup( int ingredientID ) = 0;
	virtual void removeIngredient( int ingredientID ) = 0;
	virtual void removeIngredientWeight( int id ) = 0;
	virtual void removePrepMethod( int prepMethodID ) = 0;
	virtual void removeProperty( int propertyID ) = 0;
	virtual void removePropertyFromIngredient( int ingredientID, int propertyID, int perUnitID ) = 0;
	virtual void removeRecipe( int id ) = 0;
	virtual void removeRecipeFromCategory( int ingredientID, int categoryID ) = 0;
	virtual void removeUnit( int unitID ) = 0;
	virtual void removeUnitFromIngredient( int ingredientID, int unitID ) = 0;
	virtual void removeUnitRatio( int unitID1, int unitID2 ) = 0;

	virtual void saveRecipe( Recipe *recipe ) = 0;
	virtual void saveUnitRatio( const UnitRatio *ratio ) = 0;
	virtual void search( RecipeList *list, int items, const RecipeSearchParameters &parameters ) = 0;

	/** @returns true on success, false otherwise */
	ConversionStatus convertIngredientUnits( const Ingredient &from, const Unit &to, Ingredient &result );
	virtual double unitRatio( int unitID1, int unitID2 ) = 0;

	/** @returns the number of grams in the given amount of the ingredient, or -1 on failure */
	virtual double ingredientWeight( const Ingredient &ing, bool *wasApproximated = 0 ) = 0;
	virtual WeightList ingredientWeightUnits( int ingID ) = 0;

	virtual QString escapeAndEncode( const QString &s ) const = 0;
	virtual QString unescapeAndDecode( const QByteArray &s ) const = 0;

	virtual QString categoryName( int ID ) = 0;
	virtual QString ingredientName( int ID ) = 0;
	virtual QString prepMethodName( int ID ) = 0;
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

	virtual void wipeDatabase() = 0;

	virtual void createTable( const QString &tableName ) = 0;
	virtual void splitCommands( QString& s, QStringList& sl ) = 0;

	virtual float databaseVersion( void ) = 0;

	// Functions to retrieve the maximum length of string fields,
	// these functions must be reimplemented in RecipeDB subclasses,
        // return the UnlimitedLength const if the field is unlimited.
	static const int UnlimitedLength = INT_MAX;
	virtual int maxAuthorNameLength() const = 0;
	virtual int maxCategoryNameLength() const = 0;
	virtual int maxIngredientNameLength() const = 0;
	virtual int maxIngGroupNameLength() const = 0;
	virtual int maxRecipeTitleLength() const = 0;
	virtual int maxUnitNameLength() const = 0;
	virtual int maxPrepMethodNameLength() const = 0;
	virtual int maxPropertyNameLength() const = 0;
	virtual int maxYieldTypeLength() const = 0;

	virtual bool ok()
	{
		return ( dbOK );
	}
	virtual QString err()
	{
		return ( dbErr );
	}

	void updateCategoryCache( int limit );
	void clearCategoryCache();

protected:
	virtual void portOldDatabases( float version ) = 0;
	virtual QStringList backupCommand() const = 0;
	virtual QStringList restoreCommand() const = 0;

	/** Fix property units as imported from the USDA prior to Krecipes 1.0.  This will be called once
	 * when updating from database version 0.95 to 0.96
	 */
	void fixUSDAPropertyUnits();

	//Use these with caution: SQL for one backend might not work on another!
	void execSQL( QTextStream &stream );
	virtual void execSQL( const QString & ) = 0;

	QString buildSearchQuery( const RecipeSearchParameters &parameters ) const;

	double latestDBVersion() const;
	QString krecipes_version() const;

	CategoryTree *m_categoryCache;

private:
	QIODevice * m_dumpFile;
	bool haltOperation;
	bool m_operationHalted;
	KProcess * process;
	bool m_processStarted;
	bool m_processFinished;
	bool m_processError;
	int m_exitCode;
	QProcess::ExitStatus m_exitStatus;
	QEventLoop * m_localEventLoop;
	QTimer * m_timer;

private slots:
	void processDumpOutput();
	void cancelWatcher();
	void processReadDump();
	void processStarted();
	void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
	void processError(QProcess::ProcessError);

};

#endif
