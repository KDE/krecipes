/***************************************************************************
*   Copyright (C) 2003-2004 by                                            *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef LITERECIPEDB_H
#define LITERECIPEDB_H

#include <qglobal.h>
#include <qimage.h>
#include <qfileinfo.h>
#include <qobject.h>
#include <qregexp.h>
#include <qstring.h>
#include <iostream>
#ifdef Q_OS_LINUX
#include <asm/unistd.h>
#endif
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
class LiteRecipeDB: public RecipeDB
{

	Q_OBJECT

private:
	QSQLiteDB *database;
	void createDB( void );

public:
	LiteRecipeDB( const QString &dbFile = QString::null );
	~LiteRecipeDB( void );

	void connect( bool create );

	void addAuthorToRecipe( int recipeID, int categoryID );
	void addCategoryToRecipe( int recipeID, int categoryID );


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

	int findExistingAuthorByName( const QString& name );
	int findExistingCategoryByName( const QString& name );
	int findExistingIngredientByName( const QString& name );
	int findExistingPrepByName( const QString& name );
	int findExistingPropertyByName( const QString& name );
	int findExistingRecipeByName( const QString& name );
	int findExistingUnitByName( const QString& name );
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
	void givePermissions( const QString &dbName, const QString &username, const QString &password = QString::null, const QString &clientHost = "localhost" );


	bool ingredientContainsProperty( int ingredientID, int propertyID, int perUnitsID );
	bool ingredientContainsUnit( int ingredientID, int unitID );

	void initializeData( void );

	int lastInsertID();

	void loadAllRecipeIngredients( RecipeIngredientList *list, bool withNames = true );
	void loadAuthors( ElementList *list, int limit = -1, int offset = 0 );
	void loadCategories( CategoryTree *list, int limit = -1, int offset = 0, int parent_id = -1 );
	void loadCategories( ElementList *list, int limit = -1, int offset = 0 );
	void loadIngredientGroups( ElementList *list );
	void loadIngredients( ElementList *list, int limit = -1, int offset = 0 );
	void loadPossibleUnits( int ingredientID, UnitList *list );
	void loadPrepMethods( ElementList *list, int limit = -1, int offset = 0 );
	void loadProperties( IngredientPropertyList *list, int ingredientID = -2 ); // Loads the list of possible properties by default, all the ingredient properties with -1, and the ingredients of given property if id>=0
	void loadRecipe( Recipe *recipe, int recipeID = 0 );
	void loadRecipeAuthors( int recipeID, ElementList *list );
	void loadRecipeCategories( int recipeID, ElementList *list );
	void loadRecipeDetails( RecipeList *rlist, bool loadIngredients = false, bool loadCategories = false, bool loadIngredientNames = false, bool loadAuthors = false ); // Read only the recipe details (no instructions, no photo,...) and when loading ingredients and categories, no names by default, just IDs)
	void loadRecipeList( ElementList *list, int categoryID = 0, QPtrList <int>*recipeCategoryList = 0, int limit = -1, int offset = 0 );
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

	QString escape( const QString &s );
	QCString escapeAndEncode( const QString &s );
	QString unescapeAndDecode( const QString &s );

	QString categoryName( int ID );
	IngredientProperty propertyName( int ID );
	Unit unitName( int ID );

	int getCount( const QString &table_name );
	int categoryTopLevelCount();

	bool checkIntegrity( void );

	void createTable( const QString &tableName );
	void splitCommands( QString& s, QStringList& sl );

	float databaseVersion( void );

private:
	QString dbFile;

	void loadElementList( ElementList *elList, QSQLiteResult *query );
	void loadPropertyElementList( ElementList *elList, QSQLiteResult *query );
	void portOldDatabases( float version );
	int sqlite_encode_binary( const unsigned char *in, int n, unsigned char *out );
	int sqlite_decode_binary( const unsigned char *in, unsigned char *out );
};




#endif
