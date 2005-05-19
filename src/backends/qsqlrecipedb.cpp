/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include <stdlib.h>

#include "qsqlrecipedb.h"
#include "datablocks/categorytree.h"

#include <qbuffer.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <ktempfile.h>
#include <klocale.h>
#include <kmessagebox.h>

int QSqlRecipeDB::m_refCount = 0;

QSqlRecipeDB::QSqlRecipeDB( const QString &host, const QString &user, const QString &pass, const QString &name ) : RecipeDB(),
	connectionName("connection" + QString::number( m_refCount+1 ))
{
	DBuser = user;
	DBpass = pass;
	DBhost = host;
	DBname = name;

	dbOK = false; //it isn't ok until we've connect()'ed
	++m_refCount;
}

QSqlRecipeDB::~QSqlRecipeDB()
{
	if ( dbOK ) {
		database->close();
	}

	QSqlDatabase::removeDatabase( connectionName );
	--m_refCount;
}

void QSqlRecipeDB::connect( bool create )
{
	kdDebug() << i18n( "QSqlRecipeDB: Opening Database..." ) << endl;
	kdDebug() << "Parameters: \n\thost: " << DBhost << "\n\tuser: " << DBuser << "\n\ttable: " << DBname << endl;

	QStringList drivers = QSqlDatabase::drivers();
	bool driver_found = false;
	for ( QStringList::const_iterator it = drivers.begin(); it != drivers.end(); ++it ) {
		if ( ( *it ) == qsqlDriver() ) {
			driver_found = true;
			break;
		}
	}

	if ( !driver_found ) {
		dbErr = QString( i18n( "The Qt database plug-in (%1) is not installed.  This plug-in is required for using this database backend." ) ).arg( qsqlDriver() );
		return ;
	}

	//we need to have a unique connection name for each QSqlRecipeDB class as multiple db's may be open at once (db to db transfer)
	database = QSqlDatabase::addDatabase( qsqlDriver(), connectionName );

	database->setDatabaseName( DBname );
	if ( !( DBuser.isNull() ) )
		database->setUserName( DBuser );
	if ( !( DBpass.isNull() ) )
		database->setPassword( DBpass );
	database->setHostName( DBhost );

	kdDebug() << i18n( "Parameters set. Calling db->open()" ) << endl;

	if ( !database->open() ) {
		//Try to create the database
		if ( create ) {
			kdDebug() << i18n( "Failing to open database. Trying to create it" ) << endl;
			createDB();
		}
		else {
			// Handle the error (passively)
			dbErr = QString( i18n( "Krecipes could not open the database using the driver '%2' (with username: \"%1\"). You may not have the necessary permissions, or the server may be down." ) ).arg( DBuser ).arg( qsqlDriver() );
		}

		//Now Reopen the Database and signal & exit if it fails
		if ( !database->open() ) {
			QString error = i18n( "Database message: %1" ).arg( database->lastError().databaseText() );
			kdDebug() << i18n( "Failing to open database. Exiting\n" ).latin1();

			// Handle the error (passively)
			dbErr = QString( i18n( "Krecipes could not open the database using the driver '%2' (with username: \"%1\"). You may not have the necessary permissions, or the server may be down." ) ).arg( DBuser ).arg( qsqlDriver() );
			return ;
		}
	}

	if ( int( qRound( databaseVersion() * 1e5 ) ) > int( qRound( latestDBVersion() * 1e5 ) ) ) { //correct for float's imprecision
		dbErr = i18n( "This database was created with a newer version of Krecipes and cannot be opened." );
		return ;
	}

	// Check integrity of the database (tables). If not possible, exit
	if ( !checkIntegrity() ) {
		kdError() << i18n( "Failed to fix database structure. Exiting.\n" ).latin1();
		kapp->exit( 1 );
	}

	// Database was opened correctly
	m_query = QSqlQuery( QString::null, database );
	m_query.setForwardOnly(true);
	dbOK = true;
}

void QSqlRecipeDB::initializeData( void )
{
	// Populate with data

	QString commands;
	// Read the commands form the data file
	QFile datafile( KGlobal::dirs() ->findResource( "appdata", "data/data.sql" ) );
	if ( datafile.open( IO_ReadOnly ) ) {
		QTextStream stream( &datafile );
		commands = stream.read();
		datafile.close();
	}

	// Split commands
	QStringList commandList;
	splitCommands( commands, commandList );

	// Execute commands
	for ( QStringList::Iterator it = commandList.begin(); it != commandList.end(); ++it ) {
		database->exec( ( *it ) + QString( ";" ) ); //Split removes the semicolons
	}
}

void QSqlRecipeDB::loadRecipes( RecipeList *rlist, int items, QValueList<int> ids )
{
	// Empty the recipe first
	rlist->empty();

	QMap <int, RecipeList::Iterator> recipeIterators; // Stores the iterator of each recipe in the list;

	QString command;

	// Read title, author, instructions, and prep time
	QStringList ids_str;
	for ( QValueList<int>::const_iterator it = ids.begin(); it != ids.end(); ++it ) {
		ids_str << QString::number(*it);
	}

	command = "SELECT id,title,instructions,persons,prep_time FROM recipes"+(ids_str.count()!=0?" WHERE id IN ("+ids_str.join(",")+")":"");

	m_query.exec( command );
	if ( m_query.isActive() ) {
		while ( m_query.next() ) {
			Recipe recipe;
			recipe.recipeID = m_query.value( 0 ).toInt();
			if ( items & RecipeDB::Title ) recipe.title = unescapeAndDecode( m_query.value( 1 ).toString() );
			if ( items & RecipeDB::Instructions ) recipe.instructions = unescapeAndDecode( m_query.value( 2 ).toString() );
			if ( items & RecipeDB::Servings ) recipe.persons = m_query.value( 3 ).toInt();
			if ( items & RecipeDB::PrepTime ) recipe.prepTime = m_query.value( 4 ).toTime();
			

			recipeIterators[ recipe.recipeID ] = rlist->append( recipe );
		}
	}

	// Read the ingredients
	if ( items & RecipeDB::Ingredients ) {
		for ( RecipeList::iterator recipe_it = rlist->begin(); recipe_it != rlist->end(); ++recipe_it ) {
			command = QString( "SELECT il.ingredient_id,i.name,il.amount,u.id,u.name,u.plural,il.prep_method_id,il.group_id FROM ingredients i, ingredient_list il, units u WHERE il.recipe_id=%1 AND i.id=il.ingredient_id AND u.id=il.unit_id ORDER BY il.order_index;" ).arg( (*recipe_it).recipeID );
		
			m_query.exec( command );
			if ( m_query.isActive() ) {
				while ( m_query.next() ) {
					Ingredient ing;
					ing.ingredientID = m_query.value( 0 ).toInt();
					ing.name = unescapeAndDecode( m_query.value( 1 ).toString() );
					ing.amount = m_query.value( 2 ).toDouble();
					ing.unitID = m_query.value( 3 ).toInt();
					ing.units.name = unescapeAndDecode( m_query.value( 4 ).toString() );
					ing.units.plural = unescapeAndDecode( m_query.value( 5 ).toString() );
		
					//if we don't have both name and plural, use what we have as both
					if ( ing.units.name.isEmpty() )
						ing.units.name = ing.units.plural;
					else if ( ing.units.plural.isEmpty() )
						ing.units.plural = ing.units.name;
		
					ing.prepMethodID = m_query.value( 6 ).toInt();
					if ( ing.prepMethodID != -1 ) {
						QSqlQuery prepMethodToLoad( QString( "SELECT name FROM prep_methods WHERE id=%1" ).arg( ing.prepMethodID ), database );
						if ( prepMethodToLoad.isActive() && prepMethodToLoad.first() )
							ing.prepMethod = unescapeAndDecode( prepMethodToLoad.value( 0 ).toString() );
					}
		
					ing.groupID = m_query.value( 7 ).toInt();
					if ( ing.groupID != -1 ) {
						QSqlQuery toLoad( QString( "SELECT name FROM ingredient_groups WHERE id=%1" ).arg( ing.groupID ), database );
						if ( toLoad.isActive() && toLoad.first() )
							ing.group = unescapeAndDecode( toLoad.value( 0 ).toString() );
					}
		
					(*recipeIterators[ (*recipe_it).recipeID ]).ingList.append( ing );
				}
			}
		}
	}

	//Load the Image
	if ( items & RecipeDB::Photo ) {
		for ( RecipeList::iterator recipe_it = rlist->begin(); recipe_it != rlist->end(); ++recipe_it ) {
			RecipeList::iterator it = recipeIterators[ (*recipe_it).recipeID ];
			loadPhoto( (*it).recipeID, (*it).photo );
		}
	}

	//Load the category list
	if ( items & RecipeDB::Categories ) {
		for ( RecipeList::iterator recipe_it = rlist->begin(); recipe_it != rlist->end(); ++recipe_it ) {
			RecipeList::iterator it = recipeIterators[ (*recipe_it).recipeID ];
			
			command = QString( "SELECT cl.category_id,c.name FROM category_list cl, categories c WHERE recipe_id=%1 AND cl.category_id=c.id;" ).arg( (*it).recipeID );
		
			m_query.exec( command );
			if ( m_query.isActive() ) {
				while ( m_query.next() ) {
					Element el;
					el.id = m_query.value( 0 ).toInt();
					el.name = unescapeAndDecode( m_query.value( 1 ).toString() );
					if ( el.id != -1 )
						(*it).categoryList.append( el ); // add to list except for default category (-1)
				}
			}
		}
	}

	//Load the author list
	if ( items & RecipeDB::Authors ) {
		for ( RecipeList::iterator recipe_it = rlist->begin(); recipe_it != rlist->end(); ++recipe_it ) {
			RecipeList::iterator it = recipeIterators[ (*recipe_it).recipeID ];

			command = QString( "SELECT al.author_id,a.name FROM author_list al, authors a WHERE recipe_id=%1 AND al.author_id=a.id;" ).arg( (*it).recipeID );
		
			m_query.exec( command );
			if ( m_query.isActive() ) {
				while ( m_query.next() ) {
					Element el;
					el.id = m_query.value( 0 ).toInt();
					el.name = unescapeAndDecode( m_query.value( 1 ).toString() );
					(*it).authorList.append( el );
				}
			}
		}
	}
}

void QSqlRecipeDB::loadIngredientGroups( ElementList *list )
{
	list->clear();

	QString command = "SELECT id,name FROM ingredient_groups ORDER BY name;";
	m_query.exec( command );

	if ( m_query.isActive() ) {
		while ( m_query.next() ) {
			Element group;
			group.id = m_query.value( 0 ).toInt();
			group.name = unescapeAndDecode( m_query.value( 1 ).toString() );
			list->append( group );
		}
	}
}

void QSqlRecipeDB::loadIngredients( ElementList *list, int limit, int offset )
{
	list->clear();

	QString command = "SELECT id,name FROM ingredients ORDER BY name"
	  +((limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset));
	m_query.exec( command );

	if ( m_query.isActive() ) {
		while ( m_query.next() ) {
			Element ing;
			ing.id = m_query.value( 0 ).toInt();
			ing.name = unescapeAndDecode( m_query.value( 1 ).toString() );
			list->append( ing );
		}
	}
}

void QSqlRecipeDB::loadPrepMethods( ElementList *list, int limit, int offset )
{
	list->clear();

	QString command = "SELECT id,name FROM prep_methods ORDER BY name"
	  +((limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset));
	m_query.exec( command );

	if ( m_query.isActive() ) {
		while ( m_query.next() ) {
			Element prep_method;
			prep_method.id = m_query.value( 0 ).toInt();
			prep_method.name = unescapeAndDecode( m_query.value( 1 ).toString() );
			list->append( prep_method );
		}
	}
}

void QSqlRecipeDB::createNewPrepMethod( const QString &prepMethodName )
{
	QString command;
	QString real_name = prepMethodName.left( maxPrepMethodNameLength() );

	command = QString( "INSERT INTO prep_methods VALUES(%2,'%1');" ).arg( escapeAndEncode( real_name ) ).arg( getNextInsertIDStr( "prep_methods", "id" ) );
	QSqlQuery prepMethodToCreate( command, database );

	emit prepMethodCreated( Element( real_name, lastInsertID() ) );
}

void QSqlRecipeDB::modPrepMethod( int prepMethodID, const QString &newLabel )
{
	QString command;

	command = QString( "UPDATE prep_methods SET name='%1' WHERE id=%2;" ).arg( escapeAndEncode( newLabel ) ).arg( prepMethodID );
	QSqlQuery prepMethodToCreate( command, database );

	emit prepMethodRemoved( prepMethodID );
	emit prepMethodCreated( Element( newLabel, prepMethodID ) );
}

void QSqlRecipeDB::modProperty( int propertyID, const QString &newLabel )
{
	QString command;

	command = QString( "UPDATE ingredient_properties SET name='%1' WHERE id=%2;" ).arg( escapeAndEncode( newLabel ) ).arg( propertyID );
	QSqlQuery createQuery( command, database );

	emit propertyRemoved( propertyID );
	emit propertyCreated( propertyName( propertyID ) );
}

void QSqlRecipeDB::loadPossibleUnits( int ingredientID, UnitList *list )
{
	list->clear();

	QString command;

	command = QString( "SELECT u.id,u.name,u.plural FROM unit_list ul, units u WHERE ul.ingredient_id=%1 AND ul.unit_id=u.id;" ).arg( ingredientID );

	QSqlQuery unitToLoad( command, database );

	if ( unitToLoad.isActive() ) {
		while ( unitToLoad.next() ) {
			Unit unit;
			unit.id = unitToLoad.value( 0 ).toInt();
			unit.name = unescapeAndDecode( unitToLoad.value( 1 ).toString() );
			unit.plural = unescapeAndDecode( unitToLoad.value( 2 ).toString() );
			list->append( unit );
		}
	}


}

void QSqlRecipeDB::storePhoto( int recipeID, const QByteArray &data )
{
	QSqlQuery query( QString::null, database );

	query.prepare( "UPDATE recipes SET photo=? WHERE id=" + QString::number( recipeID ) );
	query.addBindValue( data ); //this handles the binary encoding
	query.exec();
}

void QSqlRecipeDB::loadPhoto( int recipeID, QPixmap &photo )
{
	QString command = QString( "SELECT photo FROM recipes WHERE id=%1" ).arg( recipeID ); //Be careful: No semicolon here at the end
	QSqlQuery query( command, database );
	if ( query.isActive() && query.first() ) {
		photo.loadFromData( query.value( 0 ).toByteArray() );
	}
}

void QSqlRecipeDB::saveRecipe( Recipe *recipe )
{
	// Check if it's a new recipe or it exists (supossedly) already.

	bool newRecipe;
	newRecipe = ( recipe->recipeID == -1 );
	// First check if the recipe ID is set, if so, update (not create)
	// Be carefull, first check if the recipe hasn't been deleted while changing.

	QSqlQuery recipeToSave( QString::null, database );

	QString command;

	if ( newRecipe ) {
		command = QString( "INSERT INTO recipes VALUES (%5,'%1',%2,'%3',NULL,'%4');" )  // Id is autoincremented
		          .arg( escapeAndEncode( recipe->title ) )
		          .arg( recipe->persons )
		          .arg( escapeAndEncode( recipe->instructions ) )
		          .arg( recipe->prepTime.toString( "hh:mm:ss" ) )
		          .arg( getNextInsertIDStr( "recipes", "id" ) );
	}
	else	{
		command = QString( "UPDATE recipes SET title='%1',persons=%2,instructions='%3',prep_time='%4' WHERE id=%5;" )
		          .arg( escapeAndEncode( recipe->title ) )
		          .arg( recipe->persons )
		          .arg( escapeAndEncode( recipe->instructions ) )
		          .arg( recipe->prepTime.toString( "hh:mm:ss" ) )
		          .arg( recipe->recipeID );
	}
	recipeToSave.exec( command );

	// If it's a new recipe, identify the ID that was given to the recipe and store in the Recipe itself
	int recipeID;
	if ( newRecipe ) {
		recipeID = lastInsertID();
		recipe->recipeID = recipeID;
	}
	recipeID = recipe->recipeID;

	// Let's begin storing the Image!
	if ( !recipe->photo.isNull() ) {
		QByteArray ba;
		QBuffer buffer( ba );
		buffer.open( IO_WriteOnly );
		QImageIO iio( &buffer, "JPEG" );
		iio.setImage( recipe->photo.convertToImage() );
		iio.write();
		//recipe->photo.save( &buffer, "JPEG" ); don't need QImageIO in QT 3.2

		storePhoto( recipeID, ba );
	}
	else {
		recipeToSave.exec( "UPDATE recipes SET photo=NULL WHERE id=" + QString::number( recipeID ) );
	}

	// Save the ingredient list (first delete if we are updating)

	command = QString( "DELETE FROM ingredient_list WHERE recipe_id=%1;" )
	          .arg( recipeID );
	recipeToSave.exec( command );

	int order_index = 0;
	for ( IngredientList::const_iterator ing_it = recipe->ingList.begin(); ing_it != recipe->ingList.end(); ++ing_it ) {
		order_index++;
		command = QString( "INSERT INTO ingredient_list VALUES (%1,%2,%3,%4,%5,%6,%7);" )
		          .arg( recipeID )
		          .arg( ( *ing_it ).ingredientID )
		          .arg( ( *ing_it ).amount )
		          .arg( ( *ing_it ).unitID )
		          .arg( ( *ing_it ).prepMethodID )
		          .arg( order_index )
		          .arg( ( *ing_it ).groupID );
		recipeToSave.exec( command );
	}

	// Save the category list for the recipe (first delete, in case we are updating)
	command = QString( "DELETE FROM category_list WHERE recipe_id=%1;" )
	          .arg( recipeID );
	recipeToSave.exec( command );

	ElementList::const_iterator cat_it = recipe->categoryList.end(); // Start from last, mysql seems to work in lifo format... so it's read first the latest inserted one (newest)
	--cat_it;
	for ( unsigned int i = 0; i < recipe->categoryList.count(); i++ ) {
		command = QString( "INSERT INTO category_list VALUES (%1,%2);" )
		          .arg( recipeID )
		          .arg( ( *cat_it ).id );
		recipeToSave.exec( command );

		--cat_it;
	}

	//Add the default category -1 to ease and speed up searches

	command = QString( "INSERT INTO category_list VALUES (%1,-1);" )
	          .arg( recipeID );
	recipeToSave.exec( command );


	// Save the author list for the recipe (first delete, in case we are updating)
	command = QString( "DELETE FROM author_list WHERE recipe_id=%1;" )
	          .arg( recipeID );
	recipeToSave.exec( command );

	ElementList::const_iterator author_it = recipe->authorList.end(); // Start from last, mysql seems to work in lifo format... so it's read first the latest inserted one (newest)
	--author_it;
	for ( unsigned int i = 0; i < recipe->authorList.count(); i++ ) {
		command = QString( "INSERT INTO author_list VALUES (%1,%2);" )
		          .arg( recipeID )
		          .arg( ( *author_it ).id );
		recipeToSave.exec( command );

		--author_it;
	}

	if ( newRecipe )
		emit recipeCreated( Element( recipe->title.left( maxRecipeTitleLength() ), recipeID ), recipe->categoryList );
	else
		emit recipeModified( Element( recipe->title.left( maxRecipeTitleLength() ), recipeID ), recipe->categoryList );
}

void QSqlRecipeDB::loadRecipeList( ElementList *list, int categoryID, QValueList <int>*recipeCategoryList, int limit, int offset )
{
	list->clear();

	QString command;

	if ( !categoryID )  // load just the list
	{
		if ( !recipeCategoryList )
			command = "SELECT id,title FROM recipes;";
		else {
			CategoryTree tree; loadCategories(&tree,limit,offset);
			QStringList ids; getIDList(&tree,ids);
			command = "SELECT r.id,r.title,cl.category_id FROM recipes r,category_list cl WHERE r.id=cl.recipe_id AND cl.category_id IN ("+ids.join(",")+");";

			if ( offset == 0 ) {
				QString uncategorized_command = "SELECT r.id,r.title FROM recipes r,category_list cl WHERE r.id=cl.recipe_id GROUP BY id HAVING COUNT(*)=1";
				m_query.exec( uncategorized_command );
				if ( m_query.isActive() ) {
					while ( m_query.next() ) {
						Element recipe;
						recipe.id = m_query.value( 0 ).toInt();
						recipe.name = unescapeAndDecode( m_query.value( 1 ).toString() );
						list->append( recipe );
			
						recipeCategoryList->append ( -1 );
					}
				}
			}
		}

	}
	else  // load the list of those in the specified category
	{

		if ( !recipeCategoryList )
			command = QString( "SELECT r.id,r.title FROM recipes r,category_list cl WHERE r.id=cl.recipe_id AND cl.category_id=%1;" ).arg( categoryID );
		else
			command = QString( "SELECT r.id,r.title,cl.category_id FROM recipes r,category_list cl WHERE r.id=cl.recipe_id AND cl.category_id=%1;" ).arg( categoryID );
	}


	QSqlQuery recipeToLoad( command, database );

	if ( recipeToLoad.isActive() ) {
		while ( recipeToLoad.next() ) {
			Element recipe;
			recipe.id = recipeToLoad.value( 0 ).toInt();
			recipe.name = unescapeAndDecode( recipeToLoad.value( 1 ).toString() );
			list->append( recipe );

			if ( recipeCategoryList ) {
				recipeCategoryList->append ( recipeToLoad.value( 2 ).toInt() );
			}
		}
	}
}


void QSqlRecipeDB::removeRecipe( int id )
{
	emit recipeRemoved( id );

	QString command;

	command = QString( "DELETE FROM recipes WHERE id=%1;" ).arg( id );
	QSqlQuery recipeToRemove( command, database );
	command = QString( "DELETE FROM ingredient_list WHERE recipe_id=%1;" ).arg( id );
	recipeToRemove.exec( command );
	command = QString( "DELETE FROM category_list WHERE recipe_id=%1;" ).arg( id );
	recipeToRemove.exec( command );

	// Clean up ingredient_groups which have no recipe that they belong to
	// MySQL doesn't support subqueries until 4.1, so we'll do this the long way
	// (Easy way: DELETE FROM ingredient_groups WHERE id NOT IN ( SELECT DISTINCT(group_id) FROM ingredient_list );)
	QStringList ids;
	command = QString( "SELECT DISTINCT(group_id) FROM ingredient_list;" );
	recipeToRemove.exec( command );
	if ( recipeToRemove.isActive() ) {
		while ( recipeToRemove.next() ) {
			if ( recipeToRemove.value( 0 ).toInt() != -1 )
				ids << QString::number( recipeToRemove.value( 0 ).toInt() );
		}
	}
	command = QString( "DELETE FROM ingredient_groups WHERE id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	recipeToRemove.exec( command );
}

void QSqlRecipeDB::removeRecipeFromCategory( int recipeID, int categoryID )
{
	QString command;
	command = QString( "DELETE FROM category_list WHERE recipe_id=%1 AND category_id=%2;" ).arg( recipeID ).arg( categoryID );
	QSqlQuery recipeToRemove( command, database );

	emit recipeRemoved( recipeID, categoryID );
}

void QSqlRecipeDB::createNewIngGroup( const QString &name )
{
	QString command;
	QString real_name = name.left( maxIngGroupNameLength() );

	command = QString( "INSERT INTO ingredient_groups VALUES(%2,'%1');" ).arg( escapeAndEncode( real_name ) ).arg( getNextInsertIDStr( "ingredient_groups", "id" ) );
	QSqlQuery query( command, database );
}

void QSqlRecipeDB::createNewIngredient( const QString &ingredientName )
{
	QString command;
	QString real_name = ingredientName.left( maxIngredientNameLength() );

	command = QString( "INSERT INTO ingredients VALUES(%2,'%1');" ).arg( escapeAndEncode( real_name ) ).arg( getNextInsertIDStr( "ingredients", "id" ) );
	QSqlQuery ingredientToCreate( command, database );

	emit ingredientCreated( Element( real_name, lastInsertID() ) );
}

void QSqlRecipeDB::modIngredient( int ingredientID, const QString &newLabel )
{
	QString command;

	command = QString( "UPDATE ingredients SET name='%1' WHERE id=%2;" ).arg( escapeAndEncode( newLabel ) ).arg( ingredientID );
	QSqlQuery ingredientToCreate( command, database );

	emit ingredientRemoved( ingredientID );
	emit ingredientCreated( Element( newLabel, ingredientID ) );
}

void QSqlRecipeDB::addUnitToIngredient( int ingredientID, int unitID )
{
	QString command;

	command = QString( "INSERT INTO unit_list VALUES(%1,%2);" ).arg( ingredientID ).arg( unitID );
	QSqlQuery ingredientToCreate( command, database );
}

void QSqlRecipeDB::loadUnits( UnitList *list, int limit, int offset )
{
	list->clear();

	QString command;

	command = "SELECT id,name,plural FROM units ORDER BY name"
	  +((limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset));

	QSqlQuery unitToLoad( command, database );

	if ( unitToLoad.isActive() ) {
		while ( unitToLoad.next() ) {
			Unit unit;
			unit.id = unitToLoad.value( 0 ).toInt();
			unit.name = unescapeAndDecode( unitToLoad.value( 1 ).toString() );
			unit.plural = unescapeAndDecode( unitToLoad.value( 2 ).toString() );
			list->append( unit );
		}
	}
}

void QSqlRecipeDB::removeUnitFromIngredient( int ingredientID, int unitID )
{
	QString command;

	command = QString( "DELETE FROM unit_list WHERE ingredient_id=%1 AND unit_id=%2;" ).arg( ingredientID ).arg( unitID );
	QSqlQuery unitToRemove( command, database );

	// Remove any recipe using that combination of ingredients also (user must have been warned before calling this function!)

	command = QString( "SELECT r.id FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1 AND il.unit_id=%2;" ).arg( ingredientID ).arg( unitID );
	unitToRemove.exec( command );
	if ( unitToRemove.isActive() ) {
		while ( unitToRemove.next() ) {
			emit recipeRemoved( unitToRemove.value( 0 ).toInt() );
			database->exec( QString( "DELETE FROM recipes WHERE id=%1;" ).arg( unitToRemove.value( 0 ).toInt() ) );
		}
	}

	// Remove any ingredient in ingredient_list which has references to this unit and ingredient
	command = QString( "DELETE FROM ingredient_list WHERE ingredient_id=%1 AND unit_id=%2;" ).arg( ingredientID ).arg( unitID );
	unitToRemove.exec( command );

	// Remove any ingredient properties from ingredient_info where the this ingredient+unit is being used (user must have been warned before calling this function!)
	command = QString( "DELETE FROM ingredient_info ii WHERE ii.ingredient_id=%1 AND ii.per_units=%2;" ).arg( ingredientID ).arg( unitID );
	unitToRemove.exec( command );

	// Clean up ingredient_list which have no recipe that they belong to
	// MySQL doesn't support subqueries until 4.1, so we'll do this the long way
	// (Easy way: DELETE FROM ingredient_list WHERE recipe_id NOT IN ( SELECT id FROM recipes );)
	QStringList ids;
	command = QString( "SELECT id FROM recipes;" );
	unitToRemove.exec( command );
	if ( unitToRemove.isActive() ) {
		while ( unitToRemove.next() ) {
			ids << QString::number( unitToRemove.value( 0 ).toInt() );
		}
	}
	command = QString( "DELETE FROM ingredient_list WHERE recipe_id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	unitToRemove.exec( command );

	// Clean up category_list which have no recipe that they belong to
	command = QString( "DELETE FROM category_list WHERE recipe_id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	unitToRemove.exec( command );

	// Clean up ingredient_groups which have no recipe that they belong to
	// MySQL doesn't support subqueries until 4.1, so we'll do this the long way
	// (Easy way: DELETE FROM ingredient_groups WHERE id NOT IN ( SELECT DISTINCT(group_id) FROM ingredient_list );)
	ids.clear();
	command = QString( "SELECT DISTINCT(group_id) FROM ingredient_list;" );
	unitToRemove.exec( command );
	if ( unitToRemove.isActive() ) {
		while ( unitToRemove.next() ) {
			if ( unitToRemove.value( 0 ).toInt() != -1 )
				ids << QString::number( unitToRemove.value( 0 ).toInt() );
		}
	}
	command = QString( "DELETE FROM ingredient_groups WHERE id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	unitToRemove.exec( command );
}


void QSqlRecipeDB::findUseOf_Ing_Unit_InRecipes( ElementList *results, int ingredientID, int unitID )
{
	QString command = QString( "SELECT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1 AND il.unit_id=%2" ).arg( ingredientID ).arg( unitID );
	QSqlQuery recipeFound( command, database ); // Find the entries

	// Populate data in the ElementList*
	if ( recipeFound.isActive() ) {
		while ( recipeFound.next() ) {
			Element recipe;
			recipe.id = recipeFound.value( 0 ).toInt();
			recipe.name = unescapeAndDecode( recipeFound.value( 1 ).toString() );
			results->append( recipe );
		}
	}
}

void QSqlRecipeDB::findUseOfIngInRecipes( ElementList *results, int ingredientID )
{
	QString command = QString( "SELECT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1" ).arg( ingredientID );
	QSqlQuery recipeFound( command, database ); // Find the entries

	// Populate data in the ElementList*
	if ( recipeFound.isActive() ) {
		while ( recipeFound.next() ) {
			Element recipe;
			recipe.id = recipeFound.value( 0 ).toInt();
			recipe.name = unescapeAndDecode( recipeFound.value( 1 ).toString() );
			results->append( recipe );
		}
	}
}

void QSqlRecipeDB::removeIngredient( int ingredientID )
{
	QString command;

	// First remove the ingredient

	command = QString( "DELETE FROM ingredients WHERE id=%1;" ).arg( ingredientID );
	QSqlQuery ingredientToDelete( command, database );

	// Remove all the unit entries for this ingredient

	command = QString( "DELETE FROM unit_list WHERE ingredient_id=%1;" ).arg( ingredientID );
	ingredientToDelete.exec( command );

	// Remove any recipe using that ingredient

	command = QString( "SELECT r.id FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1;" ).arg( ingredientID );
	ingredientToDelete.exec( command );
	if ( ingredientToDelete.isActive() ) {
		while ( ingredientToDelete.next() ) {
			emit recipeRemoved( ingredientToDelete.value( 0 ).toInt() );
			database->exec( QString( "DELETE FROM recipes WHERE id=%1;" ).arg( ingredientToDelete.value( 0 ).toInt() ) );
		}
	}

	// Remove any ingredient in ingredient_list which has references to this ingredient
	command = QString( "DELETE FROM ingredient_list WHERE ingredient_id=%1;" ).arg( ingredientID );
	ingredientToDelete.exec( command );

	// Clean up ingredient_list which have no recipe that they belong to
	// MySQL doesn't support subqueries until 4.1, so we'll do this the long way
	// (Easy way: DELETE FROM ingredient_list WHERE recipe_id NOT IN ( SELECT id FROM recipes );)
	QStringList ids;
	command = QString( "SELECT id FROM recipes;" );
	ingredientToDelete.exec( command );
	if ( ingredientToDelete.isActive() ) {
		while ( ingredientToDelete.next() ) {
			ids << QString::number( ingredientToDelete.value( 0 ).toInt() );
		}
	}
	command = QString( "DELETE FROM ingredient_list WHERE recipe_id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	ingredientToDelete.exec( command );

	// Clean up category_list which have no recipe that they belong to. Same method as above
	command = QString( "DELETE FROM category_list WHERE recipe_id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	ingredientToDelete.exec( command );

	// Clean up ingredient_groups which have no recipe that they belong to
	// MySQL doesn't support subqueries until 4.1, so we'll do this the long way
	// (Easy way: DELETE FROM ingredient_groups WHERE id NOT IN ( SELECT DISTINCT(group_id) FROM ingredient_list );)
	ids.clear();
	command = QString( "SELECT DISTINCT(group_id) FROM ingredient_list;" );
	ingredientToDelete.exec( command );
	if ( ingredientToDelete.isActive() ) {
		while ( ingredientToDelete.next() ) {
			if ( ingredientToDelete.value( 0 ).toInt() != -1 )
				ids << QString::number( ingredientToDelete.value( 0 ).toInt() );
		}
	}
	command = QString( "DELETE FROM ingredient_groups WHERE id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	ingredientToDelete.exec( command );

	// Remove property list of this ingredient
	command = QString( "DELETE FROM ingredient_info WHERE ingredient_id=%1;" ).arg( ingredientID );
	ingredientToDelete.exec( command );

	emit ingredientRemoved( ingredientID );
}

void QSqlRecipeDB::addProperty( const QString &name, const QString &units )
{
	QString command;
	QString real_name = name.left( maxPropertyNameLength() );

	command = QString( "INSERT INTO ingredient_properties VALUES(%3,'%1','%2');" )
	          .arg( escapeAndEncode( real_name ) )
	          .arg( escapeAndEncode( units ) )
	          .arg( getNextInsertIDStr( "ingredient_properties", "id" ) );
	QSqlQuery propertyToAdd( command, database );

	emit propertyCreated( IngredientProperty( real_name, units, lastInsertID() ) );
}

void QSqlRecipeDB::loadProperties( IngredientPropertyList *list, int ingredientID )
{
	list->clear();
	QString command;
	bool usePerUnit;
	if ( ingredientID >= 0 )  // Load properties of this ingredient
	{
		usePerUnit = true;
		command = QString( "SELECT ip.id,ip.name,ip.units,ii.per_units,u.name,ii.amount,ii.ingredient_id  FROM ingredient_properties ip, ingredient_info ii, units u WHERE ii.ingredient_id=%1 AND ii.property_id=ip.id AND ii.per_units=u.id;" ).arg( ingredientID );
	}
	else if ( ingredientID == -1 )  // Load the properties of all the ingredients
	{
		usePerUnit = true;
		command = QString( "SELECT ip.id,ip.name,ip.units,ii.per_units,u.name,ii.amount,ii.ingredient_id FROM ingredient_properties ip, ingredient_info ii, units u WHERE ii.property_id=ip.id AND ii.per_units=u.id;" );
	}
	else // Load the whole property list (just the list of possible properties, not the ingredient properties)
	{
		usePerUnit = false;
		command = QString( "SELECT  id,name,units FROM ingredient_properties;" );
	}

	QSqlQuery propertiesToLoad ( command, database );
	// Load the results into the list
	if ( propertiesToLoad.isActive() ) {
		while ( propertiesToLoad.next() ) {
			IngredientProperty prop;
			prop.id = propertiesToLoad.value( 0 ).toInt();
			prop.name = unescapeAndDecode( propertiesToLoad.value( 1 ).toString() );
			prop.units = unescapeAndDecode( propertiesToLoad.value( 2 ).toString() );
			if ( usePerUnit ) {
				prop.perUnit.id = propertiesToLoad.value( 3 ).toInt();
				prop.perUnit.name = unescapeAndDecode( propertiesToLoad.value( 4 ).toString() );
			}
			else {
				prop.perUnit.id = -1;
				prop.perUnit.name = QString::null;
			}

			if ( ingredientID >= -1 )
				prop.amount = propertiesToLoad.value( 5 ).toDouble();
			else
				prop.amount = -1; // Property is generic, not attached to an ingredient

			if ( ingredientID >= -1 )
				prop.ingredientID = propertiesToLoad.value( 6 ).toInt();

			list->add( prop );
		}
	}
}

void QSqlRecipeDB::changePropertyAmountToIngredient( int ingredientID, int propertyID, double amount, int per_units )
{
	QString command;
	command = QString( "UPDATE ingredient_info SET amount=%1 WHERE ingredient_id=%2 AND property_id=%3 AND per_units=%4;" ).arg( amount ).arg( ingredientID ).arg( propertyID ).arg( per_units );
	QSqlQuery infoToChange( command, database );
}

void QSqlRecipeDB::addPropertyToIngredient( int ingredientID, int propertyID, double amount, int perUnitsID )
{
	QString command;

	command = QString( "INSERT INTO ingredient_info VALUES(%1,%2,%3,%4);" ).arg( ingredientID ).arg( propertyID ).arg( amount ).arg( perUnitsID );
	QSqlQuery propertyToAdd( command, database );
}


void QSqlRecipeDB::removePropertyFromIngredient( int ingredientID, int propertyID, int perUnitID )
{
	QString command;
	// remove property from ingredient info. Note that there could be duplicates with different units (per_units). Remove just the one especified.
	command = QString( "DELETE FROM ingredient_info WHERE ingredient_id=%1 AND property_id=%2 AND per_units=%3;" ).arg( ingredientID ).arg( propertyID ).arg( perUnitID );
	QSqlQuery propertyToRemove( command, database );
}

void QSqlRecipeDB::removeProperty( int propertyID )
{
	QString command;

	// Remove property from the ingredient_properties
	command = QString( "DELETE FROM ingredient_properties WHERE id=%1;" ).arg( propertyID );
	QSqlQuery propertyToRemove( command, database );

	// Remove any ingredient info that uses this property
	command = QString( "DELETE FROM ingredient_info WHERE property_id=%1;" ).arg( propertyID );
	propertyToRemove.exec( command );

	emit propertyRemoved( propertyID );
}

void QSqlRecipeDB::removeUnit( int unitID )
{
	QString command;
	// Remove the unit first
	command = QString( "DELETE FROM units WHERE id=%1;" ).arg( unitID );
	QSqlQuery unitToRemove( command, database );

	//Remove the unit from ingredients using it

	command = QString( "DELETE FROM unit_list WHERE unit_id=%1;" ).arg( unitID );
	unitToRemove.exec( command );


	// Remove any recipe using that unit in the ingredient list (user must have been warned before calling this function!)

	command = QString( "SELECT r.id FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.unit_id=%1;" ).arg( unitID );
	unitToRemove.exec( command );
	if ( unitToRemove.isActive() ) {
		while ( unitToRemove.next() ) {
			emit recipeRemoved( unitToRemove.value( 0 ).toInt() );
			database->exec( QString( "DELETE FROM recipes WHERE id=%1;" ).arg( unitToRemove.value( 0 ).toInt() ) );
		}
	}

	// Remove any ingredient in ingredient_list which has references to this unit
	command = QString( "DELETE FROM ingredient_list WHERE unit_id=%1;" ).arg( unitID );
	unitToRemove.exec( command );

	// Clean up ingredient_list which have no recipe that they belong to
	// MySQL doesn't support subqueries until 4.1, so we'll do this the long way
	// (Easy way: DELETE FROM ingredient_list WHERE recipe_id NOT IN ( SELECT id FROM recipes );)
	QStringList ids;
	command = QString( "SELECT id FROM recipes;" );
	unitToRemove.exec( command );
	if ( unitToRemove.isActive() ) {
		while ( unitToRemove.next() ) {
			ids << QString::number( unitToRemove.value( 0 ).toInt() );
		}
	}
	command = QString( "DELETE FROM ingredient_list WHERE recipe_id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	unitToRemove.exec( command );

	// Clean up category_list which have no recipe that they belong to. Same method as above
	command = QString( "DELETE FROM category_list WHERE recipe_id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	unitToRemove.exec( command );

	// Clean up ingredient_groups which have no recipe that they belong to
	// MySQL doesn't support subqueries until 4.1, so we'll do this the long way
	// (Easy way: DELETE FROM ingredient_groups WHERE id NOT IN ( SELECT DISTINCT(group_id) FROM ingredient_list );)
	ids.clear();
	command = QString( "SELECT DISTINCT(group_id) FROM ingredient_list;" );
	unitToRemove.exec( command );
	if ( unitToRemove.isActive() ) {
		while ( unitToRemove.next() ) {
			if ( unitToRemove.value( 0 ).toInt() != -1 )
				ids << QString::number( unitToRemove.value( 0 ).toInt() );
		}
	}
	command = QString( "DELETE FROM ingredient_groups WHERE id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	unitToRemove.exec( command );

	// Remove the ingredient properties using this unit (user must be warned before calling this function)
	command = QString( "DELETE FROM ingredient_info WHERE per_units=%1;" ).arg( unitID );
	unitToRemove.exec( command );

	// Remove the unit conversion ratios with this unit
	command = QString( "DELETE FROM units_conversion WHERE unit1_id=%1 OR unit2_id=%2;" ).arg( unitID ).arg( unitID );
	unitToRemove.exec( command );

	emit unitRemoved( unitID );
}

void QSqlRecipeDB::removePrepMethod( int prepMethodID )
{
	QString command;
	// Remove the prep method first
	command = QString( "DELETE FROM prep_methods WHERE id=%1;" ).arg( prepMethodID );
	QSqlQuery prepMethodToRemove( command, database );

	// Remove any recipe using that prep method in the ingredient list (user must have been warned before calling this function!)

	command = QString( "SELECT r.id FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.prep_method_id=%1;" ).arg( prepMethodID );
	prepMethodToRemove.exec( command );
	if ( prepMethodToRemove.isActive() ) {
		while ( prepMethodToRemove.next() ) {
			emit recipeRemoved( prepMethodToRemove.value( 0 ).toInt() );
			database->exec( QString( "DELETE FROM recipes WHERE id=%1;" ).arg( prepMethodToRemove.value( 0 ).toInt() ) );
		}
	}

	// Remove any ingredient in ingredient_list which has references to this prep method
	command = QString( "DELETE FROM ingredient_list WHERE prep_method_id=%1;" ).arg( prepMethodID );
	prepMethodToRemove.exec( command );

	// Clean up ingredient_list which have no recipe that they belong to
	// MySQL doesn't support subqueries until 4.1, so we'll do this the long way
	// (Easy way: DELETE FROM ingredient_list WHERE recipe_id NOT IN ( SELECT id FROM recipes );)
	QStringList ids;
	command = QString( "SELECT id FROM recipes;" );
	prepMethodToRemove.exec( command );
	if ( prepMethodToRemove.isActive() ) {
		while ( prepMethodToRemove.next() ) {
			ids << QString::number( prepMethodToRemove.value( 0 ).toInt() );
		}
	}

	command = QString( "DELETE FROM ingredient_list WHERE recipe_id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	prepMethodToRemove.exec( command );

	// Clean up category_list which have no recipe that they belong to. Same method as above
	command = QString( "DELETE FROM category_list WHERE recipe_id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	prepMethodToRemove.exec( command );

	// Clean up ingredient_groups which have no recipe that they belong to
	// MySQL doesn't support subqueries until 4.1, so we'll do this the long way
	// (Easy way: DELETE FROM ingredient_groups WHERE id NOT IN ( SELECT DISTINCT(group_id) FROM ingredient_list );)
	ids.clear();
	command = QString( "SELECT DISTINCT(group_id) FROM ingredient_list;" );
	prepMethodToRemove.exec( command );
	if ( prepMethodToRemove.isActive() ) {
		while ( prepMethodToRemove.next() ) {
			if ( prepMethodToRemove.value( 0 ).toInt() != -1 )
				ids << QString::number( prepMethodToRemove.value( 0 ).toInt() );
		}
	}
	command = QString( "DELETE FROM ingredient_groups WHERE id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	prepMethodToRemove.exec( command );

	emit prepMethodRemoved( prepMethodID );
}


void QSqlRecipeDB::createNewUnit( const QString &unitName, const QString &unitPlural )
{
	QString command;
	QString real_name = unitName.left( maxUnitNameLength() ).stripWhiteSpace();
	QString real_plural = unitPlural.left( maxUnitNameLength() ).stripWhiteSpace();

	if ( real_name.isEmpty() )
		real_name = real_plural;
	else if ( real_plural.isEmpty() )
		real_plural = real_name;

	command = "INSERT INTO units VALUES(" + getNextInsertIDStr( "units", "id" ) + ",'" + QString(escapeAndEncode( real_name )) + "','" + QString(escapeAndEncode( real_plural )) + "');";
	QSqlQuery unitToCreate( command, database );

	emit unitCreated( Unit( real_name, real_plural, lastInsertID() ) );
}


void QSqlRecipeDB::modUnit( int unitID, const QString &newName, const QString &newPlural )
{
	QSqlQuery unitQuery( QString::null, database );

	unitQuery.exec( "UPDATE units SET name='" + QString( escapeAndEncode( newName ) ) + "' WHERE id='" + QString::number( unitID ) + "';" );
	unitQuery.exec( "UPDATE units SET plural='" + QString( escapeAndEncode( newPlural ) ) + "' WHERE id='" + QString::number( unitID ) + "';" );

	emit unitRemoved( unitID );
	emit unitCreated( Unit( newName, newPlural, unitID ) );
}

void QSqlRecipeDB::findUseOf_Unit_InRecipes( ElementList *results, int unitID )
{
	QString command = QString( "SELECT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.unit_id=%1;" ).arg( unitID );
	QSqlQuery recipeFound( command, database ); // Find the entries

	// Populate data in the ElementList*
	if ( recipeFound.isActive() ) {
		while ( recipeFound.next() ) {
			Element recipe;
			recipe.id = recipeFound.value( 0 ).toInt();
			recipe.name = unescapeAndDecode( recipeFound.value( 1 ).toString() );
			results->append( recipe );
		}
	}
}

void QSqlRecipeDB::findUseOf_Unit_InProperties( ElementList *results, int unitID )
{
	QString command = QString( "SELECT ip.id,ip.name FROM ingredient_info ii, ingredient_properties ip WHERE ii.per_units=%1 AND ip.id=ii.property_id;" ).arg( unitID );
	QSqlQuery recipeFound( command, database ); // Find the entries

	// Populate data in the ElementList*
	if ( recipeFound.isActive() ) {
		while ( recipeFound.next() ) {
			Element recipe;
			recipe.id = recipeFound.value( 0 ).toInt();
			recipe.name = recipeFound.value( 1 ).toString();
			results->append( recipe );
		}
	}
}

void QSqlRecipeDB::loadUnitRatios( UnitRatioList *ratioList )
{
	ratioList->clear();

	QString command;
	command = "SELECT unit1_id,unit2_id,ratio FROM units_conversion;";
	QSqlQuery ratiosToLoad( command, database );

	if ( ratiosToLoad.isActive() ) {
		while ( ratiosToLoad.next() ) {
			UnitRatio ratio;
			ratio.uID1 = ratiosToLoad.value( 0 ).toInt();
			ratio.uID2 = ratiosToLoad.value( 1 ).toInt();
			ratio.ratio = ratiosToLoad.value( 2 ).toDouble();
			ratioList->add
			( ratio );
		}
	}
}

void QSqlRecipeDB::saveUnitRatio( const UnitRatio *ratio )
{
	QString command;

	// Check if it's a new ratio or it exists already.
	command = QString( "SELECT * FROM units_conversion WHERE unit1_id=%1 AND unit2_id=%2;" ).arg( ratio->uID1 ).arg( ratio->uID2 ); // Find ratio between units

	QSqlQuery ratioFound( command, database ); // Find the entries
	bool newRatio = ( ratioFound.size() == 0 );

	if ( newRatio )
		command = QString( "INSERT INTO units_conversion VALUES(%1,%2,%3);" ).arg( ratio->uID1 ).arg( ratio->uID2 ).arg( ratio->ratio );
	else
		command = QString( "UPDATE units_conversion SET ratio=%3 WHERE unit1_id=%1 AND unit2_id=%2" ).arg( ratio->uID1 ).arg( ratio->uID2 ).arg( ratio->ratio );

	ratioFound.exec( command ); // Enter the new ratio
}

double QSqlRecipeDB::unitRatio( int unitID1, int unitID2 )
{

	if ( unitID1 == unitID2 )
		return ( 1.0 );
	QString command;

	command = QString( "SELECT ratio FROM units_conversion WHERE unit1_id=%1 AND unit2_id=%2;" ).arg( unitID1 ).arg( unitID2 );
	QSqlQuery ratioToLoad( command, database );

	if ( ratioToLoad.isActive() ) {
		if ( ratioToLoad.next() )
			return ( ratioToLoad.value( 0 ).toDouble() );
		else
			return ( -1 ); // There is no ratio defined between the units
	}

	else {
		return ( -1 );
	}
}

//Finds data dependant on this Ingredient/Unit combination
void QSqlRecipeDB::findIngredientUnitDependancies( int ingredientID, int unitID, ElementList *recipes, ElementList *ingredientInfo )
{

	// Recipes using that combination

	QString command = QString( "SELECT DISTINCT r.id,r.title  FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1 AND il.unit_id=%2;" ).arg( ingredientID ).arg( unitID );
	QSqlQuery unitToRemove( command, database );
	loadElementList( recipes, &unitToRemove );
	// Ingredient info using that combination
	command = QString( "SELECT i.name,ip.name,ip.units,u.name FROM ingredients i, ingredient_info ii, ingredient_properties ip, units u WHERE i.id=ii.ingredient_id AND ii.ingredient_id=%1 AND ii.per_units=%2 AND ii.property_id=ip.id AND ii.per_units=u.id;" ).arg( ingredientID ).arg( unitID );

	unitToRemove.exec( command );
	loadPropertyElementList( ingredientInfo, &unitToRemove );
}

void QSqlRecipeDB::findIngredientDependancies( int ingredientID, ElementList *recipes )
{
	QString command = QString( "SELECT DISTINCT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1" ).arg( ingredientID );

	QSqlQuery ingredientToRemove( command, database );
	loadElementList( recipes, &ingredientToRemove );
}



//Finds data dependant on the removal of this Unit
void QSqlRecipeDB::findUnitDependancies( int unitID, ElementList *properties, ElementList *recipes )
{

	// Ingredient-Info (ingredient->property) using this Unit

	QString command = QString( "SELECT i.name,ip.name,ip.units,u.name  FROM ingredients i, ingredient_info ii, ingredient_properties ip, units u WHERE i.id=ii.ingredient_id AND ii.per_units=%1 AND ii.property_id=ip.id  AND ii.per_units=u.id;" ).arg( unitID );
	QSqlQuery unitToRemove( command, database );
	loadPropertyElementList( properties, &unitToRemove );

	// Recipes using this Unit
	command = QString( "SELECT DISTINCT r.id,r.title  FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.unit_id=%1;" ).arg( unitID ); // Without "DISTINCT" we get duplicates since ingredient_list has no unique recipe_id's
	unitToRemove.exec( command );
	loadElementList( recipes, &unitToRemove );

}

void QSqlRecipeDB::findPrepMethodDependancies( int prepMethodID, ElementList *recipes )
{
	QString command = QString( "SELECT DISTINCT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.prep_method_id=%1" ).arg( prepMethodID );

	QSqlQuery prepMethodToRemove( command, database );
	loadElementList( recipes, &prepMethodToRemove );
}


void QSqlRecipeDB::loadElementList( ElementList *elList, QSqlQuery *query )
{
	if ( query->isActive() ) {
		while ( query->next() ) {
			Element el;
			el.id = query->value( 0 ).toInt();
			el.name = unescapeAndDecode( query->value( 1 ).toString() );
			elList->append( el );
		}
	}
}
// See function "findUnitDependancies" for use
void QSqlRecipeDB::loadPropertyElementList( ElementList *elList, QSqlQuery *query )
{
	if ( query->isActive() ) {
		while ( query->next() ) {
			Element el;
			el.id = -1; // There's no ID for the ingredient-property combination
			QString ingName = query->value( 0 ).toString();
			QString propName = unescapeAndDecode( query->value( 1 ).toString() );
			QString propUnits = unescapeAndDecode( query->value( 2 ).toString() );
			QString propPerUnits = unescapeAndDecode( query->value( 3 ).toString() );

			el.name = QString( "In ingredient %1: property \"%2\" [%3/%4]" ).arg( ingName ).arg( propName ).arg( propUnits ).arg( propPerUnits );
			elList->append( el );
		}
	}
}

QCString QSqlRecipeDB::escapeAndEncode( const QString &s ) const
{
	QString s_escaped = s;

	s_escaped.replace ( "'", "\\'" );
	s_escaped.replace ( ";", "\";@" ); // Small trick for only for parsing later on

	return ( s_escaped.utf8() );
}

QString QSqlRecipeDB::unescapeAndDecode( const QString &s ) const
{
	QString s_escaped = QString::fromUtf8( s.latin1() );
	s_escaped.replace( "\";@", ";" );
	return ( s_escaped ); // Use unicode encoding
}

bool QSqlRecipeDB::ingredientContainsUnit( int ingredientID, int unitID )
{
	QString command = QString( "SELECT *  FROM unit_list WHERE ingredient_id= %1 AND unit_id=%2;" ).arg( ingredientID ).arg( unitID );
	QSqlQuery recipeToLoad( command, database );
	if ( recipeToLoad.isActive() ) {
		return ( recipeToLoad.size() > 0 );
	}
	return false;
}

bool QSqlRecipeDB::ingredientContainsProperty( int ingredientID, int propertyID, int perUnitsID )
{
	QString command = QString( "SELECT *  FROM ingredient_info WHERE ingredient_id=%1 AND property_id=%2 AND per_units=%3;" ).arg( ingredientID ).arg( propertyID ).arg( perUnitsID );
	QSqlQuery recipeToLoad( command, database );
	if ( recipeToLoad.isActive() ) {
		return ( recipeToLoad.size() > 0 );
	}
	return false;
}

QString QSqlRecipeDB::categoryName( int ID )
{
	QString command = QString( "SELECT name FROM categories WHERE id=%1;" ).arg( ID );
	QSqlQuery toLoad( command, database );
	if ( toLoad.isActive() && toLoad.next() )  // Go to the first record (there should be only one anyway.
		return ( unescapeAndDecode( toLoad.value( 0 ).toString() ) );

	return ( QString::null );
}

IngredientProperty QSqlRecipeDB::propertyName( int ID )
{
	QString command = QString( "SELECT name,units FROM ingredient_properties WHERE id=%1;" ).arg( ID );
	QSqlQuery toLoad( command, database );
	if ( toLoad.isActive() && toLoad.next() ) { // Go to the first record (there should be only one anyway.
		return ( IngredientProperty( unescapeAndDecode( toLoad.value( 0 ).toString() ), unescapeAndDecode( toLoad.value( 1 ).toString() ), ID ) );
	}

	return ( IngredientProperty( QString::null, QString::null ) );
}

Unit QSqlRecipeDB::unitName( int ID )
{
	QString command = QString( "SELECT name,plural FROM units WHERE id=%1;" ).arg( ID );
	QSqlQuery toLoad( command, database );
	if ( toLoad.isActive() && toLoad.next() ) { // Go to the first record (there should be only one anyway.
		Unit unit( unescapeAndDecode( toLoad.value( 0 ).toString() ), unescapeAndDecode( toLoad.value( 1 ).toString() ) );

		//if we don't have both name and plural, use what we have as both
		if ( unit.name.isEmpty() )
			unit.name = unit.plural;
		else if ( unit.plural.isEmpty() )
			unit.plural = unit.name;

		return unit;
	}

	return Unit();
}

int QSqlRecipeDB::getCount( const QString &table_name )
{
	QSqlQuery count( "SELECT COUNT(1) FROM "+table_name, database );
	if ( count.isActive() && count.next() ) { // Go to the first record (there should be only one anyway.
		return count.value( 0 ).toInt();
	}

	return -1;
}

int QSqlRecipeDB::categoryTopLevelCount()
{
	QSqlQuery count( "SELECT COUNT(1) FROM categories WHERE parent_id='-1'", database );
	if ( count.isActive() && count.next() ) { // Go to the first record (there should be only one anyway.
		return count.value( 0 ).toInt();
	}

	return -1;
}

bool QSqlRecipeDB::checkIntegrity( void )
{


	// Check existence of the necessary tables (the database may be created, but empty)
	QStringList tables;
	tables << "ingredient_info" << "ingredient_list" << "ingredient_properties" << "ingredients" << "recipes" << "unit_list" << "units" << "units_conversion" << "categories" << "category_list" << "authors" << "author_list" << "db_info" << "prep_methods" << "ingredient_groups";

	QStringList existingTableList = database->tables();
	for ( QStringList::Iterator it = tables.begin(); it != tables.end(); ++it ) {
		bool found = false;

		for ( QStringList::Iterator ex_it = existingTableList.begin(); ( ( ex_it != existingTableList.end() ) && ( !found ) ); ++ex_it ) {
			found = ( *ex_it == *it );
		}

		if ( !found ) {
			kdDebug() << "Recreating missing table: " << *it << "\n";
			createTable( *it );
		}
	}


	// Check for older versions, and port

	kdDebug() << "Checking database version...\n";
	float version = databaseVersion();
	kdDebug() << "version found... " << version << " \n";
	kdDebug() << "latest version... " << latestDBVersion() << endl;
	if ( int( qRound( databaseVersion() * 1e5 ) ) < int( qRound( latestDBVersion() * 1e5 ) ) ) { //correct for float's imprecision
		switch ( KMessageBox::questionYesNo( 0, i18n( "<!doc>The database was created with a previous version of Krecipes.  Would you like Krecipes to update this database to work with this version of Krecipes?<br><br><b>Warning: After updating, this database will no longer be compatible with previous versions of Krecipes.</b>" ) ) ) {
		case KMessageBox::Yes:
			portOldDatabases( version );
			break;
		case KMessageBox::No:
			return false;
		}
	}

	return true;
}

void QSqlRecipeDB::splitCommands( QString& s, QStringList& sl )
{
	sl = QStringList::split( QRegExp( ";{1}(?!@)" ), s );
}

void QSqlRecipeDB::portOldDatabases( float /* version */ )
{}

float QSqlRecipeDB::databaseVersion( void )
{

	QString command = "SELECT ver FROM db_info";
	QSqlQuery dbVersion( command, database );

	if ( dbVersion.isActive() && dbVersion.next() )
		return ( dbVersion.value( 0 ).toDouble() ); // There should be only one (or none for old DB) element, so go to first
	else
		return ( 0.2 ); // if table is empty, assume oldest (0.2), and port
}

void QSqlRecipeDB::loadCategories( ElementList *list, int limit, int offset )
{
	list->clear();

	QString command = "SELECT id,name FROM categories ORDER BY name"
	  +((limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset));
	QSqlQuery categoryToLoad( command, database );
	if ( categoryToLoad.isActive() ) {
		while ( categoryToLoad.next() ) {
			Element el;
			el.id = categoryToLoad.value( 0 ).toInt();
			el.name = unescapeAndDecode( categoryToLoad.value( 1 ).toString() );
			list->append( el );
		}
	}
}

void QSqlRecipeDB::loadCategories( CategoryTree *list, int limit, int offset, int parent_id )
{
	QString limit_str;
	if ( parent_id == -1 ) {
		list->clear();

		//only limit the number of top-level categories
		limit_str = (limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset);
	}

	QString command = "SELECT id,name,parent_id FROM categories WHERE parent_id='"+QString::number(parent_id)+"' ORDER BY name "+limit_str;

	QSqlQuery categoryToLoad( QString::null, database );
	categoryToLoad.setForwardOnly(true);
	categoryToLoad.exec(command);

	if ( categoryToLoad.isActive() ) {
		while ( categoryToLoad.next() ) {
			int id = categoryToLoad.value( 0 ).toInt();
			Element el;
			el.id = id;
			el.name = unescapeAndDecode( categoryToLoad.value( 1 ).toString() );
			CategoryTree *list_child = list->add( el );

			//QTime dbg_timer; dbg_timer.start(); kdDebug()<<"   calling QSqlRecipeDB::loadCategories"<<endl;
			loadCategories( list_child, -1, -1, id ); //limit and offset won't be used
			// kdDebug()<<"   done in "<<dbg_timer.elapsed()<<" ms"<<endl;
		}
	}
}

void QSqlRecipeDB::createNewCategory( const QString &categoryName, int parent_id )
{
	QString command;
	QString real_name = categoryName.left( maxCategoryNameLength() );

	command = QString( "INSERT INTO categories VALUES(%3,'%1',%2);" )
	          .arg( escapeAndEncode( real_name ) )
	          .arg( parent_id )
	          .arg( getNextInsertIDStr( "categories", "id" ) );
	QSqlQuery categoryToCreate( command, database );

	emit categoryCreated( Element( real_name, lastInsertID() ), parent_id );
}

void QSqlRecipeDB::modCategory( int categoryID, const QString &newLabel )
{
	QString command = QString( "UPDATE categories SET name='%1' WHERE id=%2;" ).arg( escapeAndEncode( newLabel ) ).arg( categoryID );
	QSqlQuery categoryToUpdate( command, database );

	emit categoryModified( Element( newLabel, categoryID ) );
}

void QSqlRecipeDB::modCategory( int categoryID, int new_parent_id )
{
	QString command = QString( "UPDATE categories SET parent_id=%1 WHERE id=%2;" ).arg( new_parent_id ).arg( categoryID );
	QSqlQuery categoryToUpdate( command, database );

	emit categoryModified( categoryID, new_parent_id );
}

void QSqlRecipeDB::removeCategory( int categoryID )
{
	QString command;

	command = QString( "DELETE FROM categories WHERE id=%1;" ).arg( categoryID );
	QSqlQuery categoryToRemove( command, database );

	command = QString( "DELETE FROM category_list WHERE category_id=%1;" ).arg( categoryID );
	categoryToRemove.exec( command );

	//recursively delete subcategories
	command = QString( "SELECT id FROM categories WHERE parent_id=%1;" ).arg( categoryID );
	categoryToRemove.exec( command );
	if ( categoryToRemove.isActive() ) {
		while ( categoryToRemove.next() ) {
			removeCategory( categoryToRemove.value( 0 ).toInt() );
		}
	}

	emit categoryRemoved( categoryID );
}


void QSqlRecipeDB::loadAuthors( ElementList *list, int limit, int offset )
{
	list->clear();
	QString command = "SELECT id,name FROM authors ORDER BY name"
	  +((limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset));
	QSqlQuery authorToLoad( command, database );
	if ( authorToLoad.isActive() ) {
		while ( authorToLoad.next() ) {
			Element el;
			el.id = authorToLoad.value( 0 ).toInt();
			el.name = unescapeAndDecode( authorToLoad.value( 1 ).toString() );
			list->append( el );
		}
	}
}

void QSqlRecipeDB::createNewAuthor( const QString &authorName )
{
	QString command;
	QString real_name = authorName.left( maxAuthorNameLength() );

	command = QString( "INSERT INTO authors VALUES(%2,'%1');" ).arg( escapeAndEncode( real_name ) ).arg( getNextInsertIDStr( "authors", "id" ) );
	QSqlQuery authorToCreate( command, database );

	emit authorCreated( Element( real_name, lastInsertID() ) );
}

void QSqlRecipeDB::modAuthor( int authorID, const QString &newLabel )
{
	QString command;

	command = QString( "UPDATE authors SET name='%1' WHERE id=%2;" ).arg( escapeAndEncode( newLabel ) ).arg( authorID );
	QSqlQuery authorToCreate( command, database );

	emit authorRemoved( authorID );
	emit authorCreated( Element( newLabel, authorID ) );
}

void QSqlRecipeDB::removeAuthor( int authorID )
{
	QString command;

	command = QString( "DELETE FROM authors WHERE id=%1;" ).arg( authorID );
	QSqlQuery authorToRemove( command, database );

	emit authorRemoved( authorID );
}

int QSqlRecipeDB::findExistingUnitsByName( const QString& name, int ingredientID, ElementList *list )
{
	QString search_str = escapeAndEncode( name.left( maxUnitNameLength() ) ); //truncate to the maximum size db holds
	QString command;
	if ( ingredientID < 0 )  // We're looking for units with that name all through the table, no specific ingredient
	{
		command = "SELECT id,name FROM units WHERE name='" + search_str + "' OR plural='" + search_str + "';";
	}
	else // Look for units  with that name for the specified ingredient
	{
		command = "SELECT u.id,u.name FROM units u, unit_list ul WHERE u.id=ul.unit_id AND ul.ingredient_id=" + QString::number( ingredientID ) + " AND ( u.name='" + search_str + "' OR u.plural='" + search_str + "' );";
	}

	QSqlQuery unitsToLoad( command, database ); // Run the query

	if ( list )  //If the pointer exists, then load all the values found into it
	{
		if ( unitsToLoad.isActive() )
		{
			while ( unitsToLoad.next() ) {
				Element el;
				el.id = unitsToLoad.value( 0 ).toInt();
				el.name = unescapeAndDecode( unitsToLoad.value( 1 ).toString() );
				list->append( el );
			}
		}
	}

	return ( unitsToLoad.size() );
}

int QSqlRecipeDB::findExistingAuthorByName( const QString& name )
{
	QCString search_str = escapeAndEncode( name.left( maxAuthorNameLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM authors WHERE name='%1';" ).arg( search_str );
	QSqlQuery elementToLoad( command, database ); // Run the query
	int id = -1;

	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

int QSqlRecipeDB::findExistingCategoryByName( const QString& name )
{
	QCString search_str = escapeAndEncode( name.left( maxCategoryNameLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM categories WHERE name='%1';" ).arg( search_str );
	QSqlQuery elementToLoad( command, database ); // Run the query
	int id = -1;

	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

int QSqlRecipeDB::findExistingIngredientByName( const QString& name )
{
	QCString search_str = escapeAndEncode( name.left( maxIngredientNameLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM ingredients WHERE name='%1';" ).arg( search_str );
	QSqlQuery elementToLoad( command, database ); // Run the query
	int id = -1;

	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

int QSqlRecipeDB::findExistingPrepByName( const QString& name )
{
	QCString search_str = escapeAndEncode( name.left( maxPrepMethodNameLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM prep_methods WHERE name='%1';" ).arg( search_str );
	QSqlQuery elementToLoad( command, database ); // Run the query
	int id = -1;

	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

int QSqlRecipeDB::findExistingPropertyByName( const QString& name )
{
	QCString search_str = escapeAndEncode( name.left( maxPropertyNameLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM ingredient_properties WHERE name='%1';" ).arg( search_str );
	QSqlQuery elementToLoad( command, database ); // Run the query
	int id = -1;

	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

int QSqlRecipeDB::findExistingUnitByName( const QString& name )
{
	QCString search_str = escapeAndEncode( name.left( maxUnitNameLength() ) ); //truncate to the maximum size db holds

	QString command = "SELECT id FROM units WHERE name='" + search_str + "' OR plural='" + search_str + "';";
	QSqlQuery elementToLoad( command, database ); // Run the query
	int id = -1;

	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

int QSqlRecipeDB::findExistingRecipeByName( const QString& name )
{
	QCString search_str = escapeAndEncode( name.left( maxRecipeTitleLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM recipes WHERE title='%1';" ).arg( search_str );
	QSqlQuery elementToLoad( command, database ); // Run the query

	int id = -1;
	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

void QSqlRecipeDB::mergeAuthors( int id1, int id2 )
{
	QSqlQuery update( QString::null, database );

	//change all instances of 'id2' to 'id1'
	QString command = QString( "UPDATE author_list SET author_id=%1 WHERE author_id=%2" )
	                  .arg( id1 )
	                  .arg( id2 );
	update.exec( command );

	//and ensure no duplicates were created in this process
	command = QString( "SELECT recipe_id FROM author_list WHERE author_id=%1 ORDER BY recipe_id" )
	          .arg( id1 );
	update.exec( command );
	int last_id = -1;
	if ( update.isActive() ) {
		while ( update.next() ) {
			int current_id = update.value( 0 ).toInt();
			if ( last_id == current_id ) {
				command = QString( "DELETE FROM author_list WHERE author_id=%1 AND recipe_id=%2 LIMIT 1" )
				          .arg( id1 )
				          .arg( last_id );
				QSqlQuery remove
					( command, database );

			}
			last_id = current_id;
		}
	}

	//remove author with id 'id2'
	command = QString( "DELETE FROM authors WHERE id=%1" ).arg( id2 );
	update.exec( command );
	emit authorRemoved( id2 );
}

void QSqlRecipeDB::mergeCategories( int id1, int id2 )
{
	QSqlQuery update( QString::null, database );

	//change all instances of 'id2' to 'id1'
	QString command = QString( "UPDATE category_list SET category_id=%1 WHERE category_id=%2" )
	                  .arg( id1 )
	                  .arg( id2 );
	update.exec( command );

	//and ensure no duplicates were created in this process
	command = QString( "SELECT recipe_id FROM category_list WHERE category_id=%1 ORDER BY recipe_id" )
	          .arg( id1 );
	update.exec( command );
	int last_id = -1;
	if ( update.isActive() ) {
		while ( update.next() ) {
			int current_id = update.value( 0 ).toInt();
			if ( last_id == current_id ) {
				command = QString( "DELETE FROM category_list WHERE category_id=%1 AND recipe_id=%2 LIMIT 1" )
				          .arg( id1 )
				          .arg( last_id );
				QSqlQuery remove
					( command, database );

			}
			last_id = current_id;
		}
	}

	command = QString( "UPDATE categories SET parent_id=%1 WHERE parent_id=%2" )
	          .arg( id1 )
	          .arg( id2 );
	update.exec( command );

	//we don't want to have a category be its own parent...
	command = QString( "UPDATE categories SET parent_id=-1 WHERE parent_id=id" );
	update.exec( command );

	//remove category with id 'id2'
	command = QString( "DELETE FROM categories WHERE id=%1" ).arg( id2 );
	update.exec( command );

	emit categoriesMerged( id1, id2 );
}

void QSqlRecipeDB::mergeIngredients( int id1, int id2 )
{
	QSqlQuery update( QString::null, database );

	//change all instances of 'id2' to 'id1'
	QString command = QString( "UPDATE ingredient_list SET ingredient_id=%1 WHERE ingredient_id=%2" )
	                  .arg( id1 )
	                  .arg( id2 );
	update.exec( command );

	//delete nutrient info associated with ingredient with id 'id2'
	command = QString( "DELETE FROM ingredient_info WHERE ingredient_id=%1" )
	          .arg( id2 );
	update.exec( command );

	//update the unit_list
	command = QString( "UPDATE unit_list SET ingredient_id=%1 WHERE ingredient_id=%2" )
	          .arg( id1 )
	          .arg( id2 );
	update.exec( command );

	//and ensure no duplicates were created in this process
	command = QString( "SELECT unit_id FROM unit_list WHERE ingredient_id=%1 ORDER BY unit_id" )
	          .arg( id1 );
	update.exec( command );
	int last_id = -1;
	if ( update.isActive() ) {
		while ( update.next() ) {
			int current_id = update.value( 0 ).toInt();
			if ( last_id == current_id ) {
				command = QString( "DELETE FROM unit_list WHERE ingredient_id=%1 AND unit_id=%2 LIMIT 1" )
				          .arg( id1 )
				          .arg( last_id );
				QSqlQuery remove
					( command, database );

			}
			last_id = current_id;
		}
	}

	//update ingredient info
	command = QString( "UPDATE ingredient_info SET ingredient_id=%1 WHERE ingredient_id=%2" )
	          .arg( id1 )
	          .arg( id2 );
	update.exec( command );

	//and ensure no duplicates were created in this process
	//info associated with one ingredient will be lost... they should be the same ingredient and thus info anyways
	command = QString( "SELECT property_id FROM ingredient_info WHERE ingredient_id=%1 ORDER BY property_id" )
	          .arg( id1 );
	update.exec( command );
	last_id = -1;
	if ( update.isActive() ) {
		while ( update.next() ) {
			int current_id = update.value( 0 ).toInt();
			if ( last_id == current_id ) {
				command = QString( "DELETE FROM ingredient_info WHERE ingredient_id=%1 AND property_id=%2 LIMIT 1" )
				          .arg( id1 )
				          .arg( last_id );
				QSqlQuery remove
					( command, database );

			}
			last_id = current_id;
		}
	}

	//remove ingredient with id 'id2'
	command = QString( "DELETE FROM ingredients WHERE id=%1" ).arg( id2 );
	update.exec( command );
	emit ingredientRemoved( id2 );
}

void QSqlRecipeDB::mergePrepMethods( int id1, int id2 )
{
	QSqlQuery update( QString::null, database );

	//change all instances of 'id2' to 'id1' in ingredient list
	QString command = QString( "UPDATE ingredient_list SET prep_method_id=%1 WHERE prep_method_id=%2" )
	                  .arg( id1 )
	                  .arg( id2 );
	update.exec( command );

	//remove prep method with id 'id2'
	command = QString( "DELETE FROM prep_methods WHERE id=%1" ).arg( id2 );
	update.exec( command );
	emit prepMethodRemoved( id2 );
}

void QSqlRecipeDB::mergeProperties( int id1, int id2 )
{
	QSqlQuery update( QString::null, database );

	//change all instances of 'id2' to 'id1'
	QString command = QString( "UPDATE ingredient_properties SET id=%1 WHERE id=%2" )
	                  .arg( id1 )
	                  .arg( id2 );
	update.exec( command );

	command = QString( "UPDATE ingredient_info SET property_id=%1 WHERE property_id=%2" )
	          .arg( id1 )
	          .arg( id2 );
	update.exec( command );

	//remove prep method with id 'id2'
	command = QString( "DELETE FROM ingredient_properties WHERE id=%1" ).arg( id2 );
	update.exec( command );
	emit propertyRemoved( id2 );
}

void QSqlRecipeDB::mergeUnits( int id1, int id2 )
{
	QSqlQuery update( QString::null, database );

	//change all instances of 'id2' to 'id1' in ingredient list
	QString command = QString( "UPDATE unit_list SET unit_id=%1 WHERE unit_id=%2" )
	                  .arg( id1 )
	                  .arg( id2 );
	update.exec( command );

	//change all instances of 'id2' to 'id1' in ingredient list
	command = QString( "UPDATE ingredient_list SET unit_id=%1 WHERE unit_id=%2" )
	          .arg( id1 )
	          .arg( id2 );
	update.exec( command );

	//and ensure no duplicates were created in this process
	command = QString( "SELECT ingredient_id FROM unit_list WHERE unit_id=%1 ORDER BY ingredient_id" )
	          .arg( id1 );
	update.exec( command );
	int last_id = -1;
	if ( update.isActive() ) {
		while ( update.next() ) {
			int current_id = update.value( 0 ).toInt();
			if ( last_id == current_id ) {
				command = QString( "DELETE FROM unit_list WHERE unit_id=%1 AND ingredient_id=%2 LIMIT 1" )
				          .arg( id1 )
				          .arg( last_id );
				QSqlQuery remove
					( command, database );

			}
			last_id = current_id;
		}
	}

	//update ingredient info
	command = QString( "UPDATE ingredient_info SET per_units=%1 WHERE per_units=%2" )
	          .arg( id1 )
	          .arg( id2 );
	update.exec( command );

	//change all instances of 'id2' to 'id1' in unit_conversion
	command = QString( "UPDATE units_conversion SET unit1_id=%1 WHERE unit1_id=%2" )
	          .arg( id1 )
	          .arg( id2 );
	update.exec( command );
	command = QString( "UPDATE units_conversion SET unit2_id=%1 WHERE unit2_id=%2" )
	          .arg( id1 )
	          .arg( id2 );
	update.exec( command );

	//and ensure that the one to one ratio wasn't created
	command = QString( "DELETE FROM units_conversion WHERE unit1_id=unit2_id" );
	update.exec( command );

	//remove units with id 'id2'
	command = QString( "DELETE FROM units WHERE id=%1" ).arg( id2 );
	update.exec( command );
	emit unitRemoved( id2 );
}

QString QSqlRecipeDB::getUniqueRecipeTitle( const QString &recipe_title )
{
	//already is unique
	if ( findExistingRecipeByName( recipe_title ) == -1 )
		return recipe_title;

	QString return_title = recipe_title; //If any error is produced, just go for default value (always return something)

	QString command = QString( "SELECT COUNT(DISTINCT title) FROM recipes WHERE title LIKE '%1 (%)';" ).arg( escapeAndEncode( recipe_title ) );

	QSqlQuery alikeRecipes( command, database );
	if ( alikeRecipes.isActive() && alikeRecipes.first() )
		;
	{
		int count = alikeRecipes.value( 0 ).toInt();
		return_title = QString( "%1 (%2)" ).arg( recipe_title ).arg( count + 2 );

		//make sure this newly created title is unique (just in case)
		while ( findExistingRecipeByName( return_title ) != -1 ) {
			count--; //go down to find the skipped recipe(s)
			return_title = QString( "%1 (%2)" ).arg( recipe_title ).arg( count + 2 );
		}
	}

	return return_title;
}

QString QSqlRecipeDB::recipeTitle( int recipeID )
{
	QString command = QString( "SELECT title FROM recipes WHERE id=%1;" ).arg( recipeID );
	QSqlQuery recipeToLoad( command, database );
	if ( recipeToLoad.isActive() && recipeToLoad.next() )  // Go to the first record (there should be only one anyway.
		return ( recipeToLoad.value( 1 ).toString() );

	return ( QString::null );
}

void QSqlRecipeDB::emptyData( void )
{
	QStringList tables;
	tables << "ingredient_info" << "ingredient_list" << "ingredient_properties" << "ingredients" << "recipes" << "unit_list" << "units" << "units_conversion" << "categories" << "category_list" << "authors" << "author_list" << "prep_methods" << "ingredient_groups";
	QSqlQuery tablesToEmpty( QString::null, database );
	for ( QStringList::Iterator it = tables.begin(); it != tables.end(); ++it ) {
		QString command = QString( "DELETE FROM %1;" ).arg( *it );
		tablesToEmpty.exec( command );
	}
}

QString QSqlRecipeDB::getNextInsertIDStr( const QString &table, const QString &column )
{
	int next_id = getNextInsertID( table, column );

	QString id_str;
	if ( next_id == -1 )
		id_str = "NULL";
	else
		id_str = QString::number( next_id );

	return id_str;
}

void QSqlRecipeDB::search( RecipeList *list, int items,
	const QStringList &titleKeywords, bool requireAllTitleWords,
	const QStringList &instructionsKeywords, bool requireAllInstructionsWords,
	const QStringList &ingsOr,
	const QStringList &catsOr,
	const QStringList &authorsOr,
	const QTime &time, int prep_param,
	int servings, int servings_param )
{
	QString query = buildSearchQuery(titleKeywords, requireAllTitleWords,
		instructionsKeywords, requireAllInstructionsWords,
		ingsOr,catsOr,authorsOr,
		time,prep_param,
		servings,servings_param);

	QValueList<int> ids;
	QSqlQuery recipeToLoad( query, database );
	if ( recipeToLoad.isActive() ) {
		while ( recipeToLoad.next() ) {
			ids << recipeToLoad.value( 0 ).toInt();
		}
	}

	if ( ids.count() > 0 )
		loadRecipes( list, items, ids );
}

#include "qsqlrecipedb.moc"
