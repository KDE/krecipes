/***************************************************************************
*   Copyright © 2004 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2004 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2004, 2006 Jason Kivlighn <jkivlighn@gmail.com>           *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/


#ifndef QSQLRECIPEDB_H
#define QSQLRECIPEDB_H

#include "backends/recipedb.h"

#include <qglobal.h>
#include <qsqldatabase.h>
#include <QString>
#include <QPixmap>
#include <QSqlQuery>
#include <QSqlDriver>

#include "datablocks/recipe.h"
#include "datablocks/elementlist.h"
#include "datablocks/ingredientpropertylist.h"
#include "datablocks/unitratiolist.h"

/**
@author Unai Garro, Jason Kivlighn
*/
class QSqlRecipeDB : public RecipeDB
{

	Q_OBJECT

protected:
	virtual QString qsqlDriverPlugin() const { return QString(); }
	
	//If this function returns 0, it means we are using a builtin driver.
	virtual QSqlDriver *qsqlDriver() const { return 0; }

	//Unlike qsqlDriver() this function does NOT return 0 if we are using a
	// builtin driver, it always retuns the current driver used by the
	// database.
	virtual QSqlDriver *currentDriver() const { return database->driver(); }

	virtual void createDB( void ) = 0;

	virtual void initializeData( void );

	virtual void storePhoto( int recipeID, const QByteArray &data );
	virtual void loadPhoto( int recipeID, QPixmap &photo );
	void loadRecipeMetadata( Recipe *recipe );

	void search( RecipeList *list, int items, const RecipeSearchParameters & );

	/** Return the next id for the given table and column.
	  * If the database supports getting this afterwards,
	  * leave the default implementation which returns -1.
	  *
	  * Note: Only call when an insert is actually going to take place.
	  *       This function will increment the sequence counter.
	  */
	virtual int getNextInsertID( const QString & /*table*/, const QString & /*column*/ )
	{
		return -1;
	}

	QSqlDatabase * database;
	QSqlQuery * m_query;
	QString DBuser;
	QString DBpass;
	QString DBhost;
	int DBport;

public:
	explicit QSqlRecipeDB( const QString &host, const QString &user = QString(), const QString &pass = QString(), const QString &DBName = DEFAULT_DB_NAME, int port = 0 );
	~QSqlRecipeDB( void );

	RecipeDB::Error connect( bool create_db, bool create_tables );

	virtual void transaction();
	virtual void commit();

	void addIngredientWeight( const Weight & );
	RecipeDB::IdType addProperty( const QString &name, const QString &units );
	void addPropertyToIngredient( int ingredientID, int propertyID, double amount, int perUnitsID );
	void addUnitToIngredient( int ingredientID, int unitID );

	void categorizeRecipe( int recipeID, const ElementList &categoryList );
	void changePropertyAmountToIngredient( int ingredientID, int propertyID, double amount, int per_units );

	RecipeDB::IdType createNewAuthor( const QString &authorName );
	RecipeDB::IdType createNewCategory( const QString &categoryName, int parent_id = -1 );
	RecipeDB::IdType createNewIngGroup( const QString &name );
	RecipeDB::IdType createNewIngredient( const QString &ingredientName );
	RecipeDB::IdType createNewPrepMethod( const QString &prepMethodName );
	RecipeDB::IdType createNewRating( const QString &name );
	RecipeDB::IdType createNewUnit( const Unit &unit );
	RecipeDB::IdType createNewYieldType( const QString &type );

	void emptyData( void );
	void empty( void );

	int findExistingAuthorByName( const QString& name );
	int findExistingCategoryByName( const QString& name );
	int findExistingIngredientGroupByName( const QString& name );
	int findExistingIngredientByName( const QString& name );
	int findExistingPrepByName( const QString& name );
	int findExistingRecipeByName( const QString& name );
	int findExistingRatingByName( const QString& name );
	int findExistingUnitByName( const QString& name );
	int findExistingPropertyByName( const QString& name );
	int findExistingYieldTypeByName( const QString& name );
	void findIngredientUnitDependancies( int ingredientID, int unitID, ElementList *recipes, ElementList *ingredientInfo );
	void findIngredientDependancies( int ingredientID, ElementList *recipes );
	void findPrepMethodDependancies( int prepMethodID, ElementList *recipes );
	void findUnitDependancies( int unitID, ElementList *properties, ElementList *recipes, ElementList *weights );
	void findUseOfIngGroupInRecipes( ElementList *results, int groupID );
	void findUseOfCategoryInRecipes( ElementList *results, int catID );
	void findUseOfAuthorInRecipes( ElementList *results, int authorID );

	QString getUniqueRecipeTitle( const QString &recipe_title );

	virtual void importUSDADatabase();

	bool ingredientContainsProperty( int ingredientID, int propertyID, int perUnitsID );
	bool ingredientContainsUnit( int ingredientID, int unitID );

	int loadAuthors( ElementList *list, int limit = -1, int offset = 0 );
	void loadCategories( CategoryTree *list, int limit = -1, int offset = 0, int parent_id = -1, bool recurse = true );
	void loadCategories( ElementList *list, int limit = -1, int offset = 0 );
	void loadIngredientGroups( ElementList *list );
	void loadIngredients( ElementList *list, int limit = -1, int offset = 0 );
	void loadPossibleUnits( int ingredientID, UnitList *list );
	void loadPrepMethods( ElementList *list, int limit = -1, int offset = 0 );
	void loadProperties( IngredientPropertyList *list, int ingredientID = -2 ); // Loads the list of possible properties by default, all the ingredient properties with -1, and the ingredients of given property if id>=0
	void loadRatingCriterion( ElementList *list, int limit = -1, int offset = 0 );
	void loadRecipes( RecipeList *, int items = All, QList<int> ids = QList<int>() );
	void loadRecipeList( ElementList *list, int categoryID = -1, bool recursive = false );
	void loadUncategorizedRecipes( ElementList *list );
	void loadUnits( UnitList *list, Unit::Type = Unit::All, int limit = -1, int offset = 0 );
	void loadUnitRatios( UnitRatioList *ratioList, Unit::Type );
	void loadYieldTypes( ElementList *list, int limit, int offset );

	void mergeAuthors( int id1, int id2 );
	void mergeCategories( int id1, int id2 );
	void mergeIngredientGroups( int id1, int id2 );
	void mergeIngredients( int id1, int id2 );
	void mergeUnits( int id1, int id2 );
	void mergePrepMethods( int id1, int id2 );
	void mergeProperties( int id1, int id2 );

	void modIngredientGroup( int ingredientID, const QString &newLabel );
	/**
	* set newLabel for ingredientID
	*/
	void modIngredient( int ingredientID, const QString &newLabel );
	/**
	* set newLabel for unitID
	*/
	void modUnit( const Unit &unit );
	/**
	* set newLabel for categoryID
	*/
	void modCategory( int categoryID, const QString &newLabel );
	void modCategory( int categoryID, int new_parent_id, bool emit_cat_modifed = true );
	/**
	* set newLabel for authorID
	*/
	void modAuthor( int authorID, const QString &newLabel );

	void modPrepMethod( int prepMethodID, const QString &newLabel );

	void modProperty( int propertyID, const QString &newLabel, const QString &unit = QString() );

	QString recipeTitle( int recipeID );

	void removeAuthor( int categoryID );
	void removeCategory( int categoryID );
	void removeIngredientGroup( int groupID );
	void removeIngredient( int ingredientID );
	void removeIngredientWeight( int id );
	void removePrepMethod( int prepMethodID );
	void removeProperty( int propertyID );
	void removePropertyFromIngredient( int ingredientID, int propertyID, int perUnitID );
	void removeRecipe( int id );
	void removeRecipeFromCategory( int ingredientID, int categoryID );
	void removeUnit( int unitID );
	void removeUnitFromIngredient( int ingredientID, int unitID );
	void removeUnitRatio( int unitID1, int unitID2 );

	void saveRecipe( Recipe *recipe );
	void saveUnitRatio( const UnitRatio *ratio );

	double unitRatio( int unitID1, int unitID2 );
	double ingredientWeight( const Ingredient &ing, bool *wasApproximated = 0 );
	WeightList ingredientWeightUnits( int ingID );

	QString escapeAndEncode( const QString &s ) const;
	QString unescapeAndDecode( const QByteArray &s ) const;

	QString categoryName( int ID );
	QString prepMethodName( int ID );
	QString ingredientName( int ID );
	IngredientProperty propertyName( int ID );
	Unit unitName( int ID );

	int getCount( const QString &table_name );
	int categoryTopLevelCount();

	bool checkIntegrity( void );

	virtual void wipeDatabase();

	void splitCommands( QString& s, QStringList& sl );

	virtual float databaseVersion( void );

protected:
	void execSQL( const QString &command );
	virtual RecipeDB::IdType lastInsertId( const QSqlQuery &query );

private:
	void loadElementList( ElementList *elList, QSqlQuery *query );
	void loadPropertyElementList( ElementList *elList, QSqlQuery *query );
	QString getNextInsertIDStr( const QString &table, const QString &column );

	QString DBname;
	const QString connectionName;
	QString m_command;

	static int m_refCount;
};




#endif
