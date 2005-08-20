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

#include "backends/recipedb.h"

#include <qglobal.h>
#include <qobject.h>
#include <qsqldatabase.h>
#include <qimage.h>
#include <qfileinfo.h>
#include <qregexp.h>
#include <qstring.h>

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
	virtual QString qsqlDriver() const = 0;
	virtual void createDB( void ) = 0;

	virtual void portOldDatabases( float version );
	virtual void storePhoto( int recipeID, const QByteArray &data );
	virtual void loadPhoto( int recipeID, QPixmap &photo );

	void search( RecipeList *list, int items,
			const QStringList &titleKeywords, bool requireAllTitleWords,
			const QStringList &instructionsKeywords, bool requireAllInstructionsWords,
			const QStringList &ingsOr,
			const QStringList &catsOr,
			const QStringList &authorsOr,
			const QTime &time, int prep_param,
			int servings, int servings_param );

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

	QSqlDatabase *database;
	QSqlQuery m_query;
	QString DBuser;
	QString DBpass;
	QString DBhost;

public:
	QSqlRecipeDB( const QString &host, const QString &user = QString::null, const QString &pass = QString::null, const QString &DBName = DEFAULT_DB_NAME );
	~QSqlRecipeDB( void );

	void connect( bool create_db, bool create_tables );

	void addProperty( const QString &name, const QString &units );
	void addPropertyToIngredient( int ingredientID, int propertyID, double amount, int perUnitsID );
	void addUnitToIngredient( int ingredientID, int unitID );

	void changePropertyAmountToIngredient( int ingredientID, int propertyID, double amount, int per_units );

	void createNewAuthor( const QString &authorName );
	void createNewCategory( const QString &categoryName, int parent_id = -1 );
	void createNewIngGroup( const QString &name );
	void createNewIngredient( const QString &ingredientName );
	void createNewPrepMethod( const QString &prepMethodName );
	void createNewUnit( const QString &unitName, const QString &unitPlural );

	void emptyData( void );
	void empty( void );

	int findExistingAuthorByName( const QString& name );
	int findExistingCategoryByName( const QString& name );
	int findExistingIngredientByName( const QString& name );
	int findExistingPrepByName( const QString& name );
	int findExistingRecipeByName( const QString& name );
	int findExistingUnitByName( const QString& name );
	int findExistingPropertyByName( const QString& name );
	int findExistingUnitsByName( const QString& name, int ingredientID = -1, ElementList *list = 0 );
	void findIngredientUnitDependancies( int ingredientID, int unitID, ElementList *recipes, ElementList *ingredientInfo );
	void findIngredientDependancies( int ingredientID, ElementList *recipes );
	void findPrepMethodDependancies( int prepMethodID, ElementList *recipes );
	void findUnitDependancies( int unitID, ElementList *properties, ElementList *recipes );
	void findUseOf_Ing_Unit_InRecipes( ElementList *results, int ingredientID, int unitID );
	void findUseOfIngInRecipes( ElementList *results, int ingredientID );
	void findUseOf_Unit_InRecipes( ElementList *results, int unitID );
	void findUseOf_Unit_InProperties( ElementList *results, int unitID );

	QString getUniqueRecipeTitle( const QString &recipe_title );

	bool ingredientContainsProperty( int ingredientID, int propertyID, int perUnitsID );
	bool ingredientContainsUnit( int ingredientID, int unitID );

	void loadAuthors( ElementList *list, int limit = -1, int offset = 0 );
	void loadCategories( CategoryTree *list, int limit = -1, int offset = 0, int parent_id = -1, bool recurse = true );
	void loadCategories( ElementList *list, int limit = -1, int offset = 0 );
	void loadIngredientGroups( ElementList *list );
	void loadIngredients( ElementList *list, int limit = -1, int offset = 0 );
	void loadPossibleUnits( int ingredientID, UnitList *list );
	void loadPrepMethods( ElementList *list, int limit = -1, int offset = 0 );
	void loadProperties( IngredientPropertyList *list, int ingredientID = -2 ); // Loads the list of possible properties by default, all the ingredient properties with -1, and the ingredients of given property if id>=0
	void loadRecipes( RecipeList *, int items = All, QValueList<int> ids = QValueList<int>() );
	void loadRecipeList( ElementList *list, int categoryID = -1, bool recursive = false );
	void loadUncategorizedRecipes( ElementList *list );
	void loadUnits( UnitList *list, int limit = -1, int offset = 0 );
	void loadUnitRatios( UnitRatioList *ratioList );

	void mergeAuthors( int id1, int id2 );
	void mergeCategories( int id1, int id2 );
	void mergeIngredients( int id1, int id2 );
	void mergeUnits( int id1, int id2 );
	void mergePrepMethods( int id1, int id2 );
	void mergeProperties( int id1, int id2 );

	/**
	* set newLabel for ingredientID
	*/
	void modIngredient( int ingredientID, const QString &newLabel );
	/**
	* set newLabel for unitID
	*/
	void modUnit( int unitID, const QString &newName, const QString &newPlural );
	/**
	* set newLabel for categoryID
	*/
	void modCategory( int categoryID, const QString &newLabel );
	void modCategory( int categoryID, int new_parent_id );
	/**
	* set newLabel for authorID
	*/
	void modAuthor( int authorID, const QString &newLabel );

	void modPrepMethod( int prepMethodID, const QString &newLabel );

	void modProperty( int propertyID, const QString &newLabel );

	QString recipeTitle( int recipeID );

	void removeAuthor( int categoryID );
	void removeCategory( int categoryID );
	void removeIngredient( int ingredientID );
	void removePrepMethod( int prepMethodID );
	void removeProperty( int propertyID );
	void removePropertyFromIngredient( int ingredientID, int propertyID, int perUnitID );
	void removeRecipe( int id );
	void removeRecipeFromCategory( int ingredientID, int categoryID );
	void removeUnit( int unitID );
	void removeUnitFromIngredient( int ingredientID, int unitID );

	void saveRecipe( Recipe *recipe );
	void saveUnitRatio( const UnitRatio *ratio );

	double unitRatio( int unitID1, int unitID2 );

	QCString escapeAndEncode( const QString &s ) const;
	QString unescapeAndDecode( const QString &s ) const;

	QString categoryName( int ID );
	IngredientProperty propertyName( int ID );
	Unit unitName( int ID );

	int getCount( const QString &table_name );
	int categoryTopLevelCount();

	bool checkIntegrity( void );

	void splitCommands( QString& s, QStringList& sl );

	float databaseVersion( void );

protected:
	void execSQL( QTextStream &stream );

private:
	void loadElementList( ElementList *elList, QSqlQuery *query );
	void loadPropertyElementList( ElementList *elList, QSqlQuery *query );
	QString getNextInsertIDStr( const QString &table, const QString &column );

	QString DBname;
	const QString connectionName;

	static int m_refCount;
};




#endif
