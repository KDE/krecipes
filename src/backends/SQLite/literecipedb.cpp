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

#include "literecipedb.h"

#include <kdebug.h>
#include <kstandarddirs.h>
#include <ktempfile.h>
#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <kmessagebox.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "datablocks/categorytree.h"

#define DB_FILENAME "krecipes.krecdb"

LiteRecipeDB::LiteRecipeDB( const QString &_dbFile ) : RecipeDB(), dbFile( _dbFile )
{
	KConfig * config = KGlobal::config();
	config->setGroup( "Server" );

	if ( dbFile.isNull() )
		dbFile = config->readEntry( "DBFile", locateLocal ( "appdata", DB_FILENAME ) );
}

LiteRecipeDB::~LiteRecipeDB()
{
	database->close();
	delete database;
}

void LiteRecipeDB::connect( bool create_db, bool create_tables )
{
	kdDebug() << "Connecting to the SQLite database\n";

	database = new QSQLiteDB();

	if ( !create_db && !QFile::exists( dbFile ) ) {
		dbErr = i18n( "Krecipes could not open the SQLite database. You may not have the necessary permissions.\n" );
		return ;
	}

	//if the file didn't exist before, then we need to do this to initialize the database
	if ( !QFile::exists( dbFile ) || !database->open( dbFile ) ) { //check that the file didn't exist before trying to open the db (opening it creates the file)
		//Try to create the database
		if ( !create_db ) {
			dbErr = i18n( "Krecipes could not open the SQLite database. You may not have the necessary permissions.\n" );
			return ;
		}

		//Now Reopen the Database and exit if it fails
		if ( !database->open( dbFile ) ) {
			kdError() << QString( i18n( "Could not open DB. You may not have permissions. Exiting.\n" ) ).latin1();
			dbErr = i18n( "Krecipes could not open the SQLite database. You may not have the necessary permissions.\n" );
			return ;
		}
	}

	if ( int( qRound( databaseVersion() * 1e5 ) ) > int( qRound( latestDBVersion() * 1e5 ) ) ) { //correct for float's imprecision
		dbErr = i18n( "This database was created with a newer version of Krecipes and cannot be opened." );
		return ;
	}

	// Check integrity of the database (tables). If not possible, exit
	kdDebug() << "I'll check the DB integrity now\n";
	if ( create_tables && !checkIntegrity() ) {
		kdError() << i18n( "Failed to fix database structure. Exiting.\n" ).latin1();
		dbErr = i18n( "Krecipes failed to fix the SQLite database structure. You may not have the necessary permissions, or the database structure may be too corrupted.\n" );
		return ;
	}
	dbOK = true;
}

QStringList LiteRecipeDB::backupCommand() const
{
	#if HAVE_SQLITE
	QString binary = "sqlite";
	#elif HAVE_SQLITE3
	QString binary = "sqlite3";
	#endif

	KConfig * config = KGlobal::config();
	config->setGroup( "Server" );
	binary = config->readEntry( "SQLitePath", binary );

	QStringList command;
	command<<binary<<dbFile<<".dump";
	return command;
}

QStringList LiteRecipeDB::restoreCommand() const
{
	#if HAVE_SQLITE
	QString binary = "sqlite";
	#elif HAVE_SQLITE3
	QString binary = "sqlite3";
	#endif

	KConfig * config = KGlobal::config();
	config->setGroup( "Server" );
	binary = config->readEntry( "SQLitePath", binary );

	QStringList command;
	command<<binary<<dbFile;
	return command;
}

void LiteRecipeDB::createDB()
{
	//The file is created by SQLite automatically
}

void LiteRecipeDB::execSQL( const QString &command )
{
	database->executeQuery( command );
}

void LiteRecipeDB::loadRecipes( RecipeList *rlist, int items, QValueList<int> ids )
{
	// Empty the recipe first
	rlist->empty();

	QMap <int, RecipeList::Iterator> recipeIterators; // Stores the iterator of each recipe in the list;

	QString command;

	QString current_timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);

	QStringList ids_str;
	for ( QValueList<int>::const_iterator it = ids.begin(); it != ids.end(); ++it ) {
		QString number_str = QString::number(*it);
		ids_str << number_str;

		if ( !(items & RecipeDB::Noatime) )
 			database->executeQuery( "UPDATE recipes SET atime='"+current_timestamp+"' WHERE id="+number_str );
	}

	// Read title, author, yield, and instructions as specified
	command = "SELECT id";
	if ( items & RecipeDB::Title ) command += ",title";
	if ( items & RecipeDB::Instructions ) command += ",instructions";
	if ( items & RecipeDB::PrepTime ) command += ",prep_time";
	if ( items & RecipeDB::Yield ) command += ",yield_amount,yield_amount_offset,yield_type_id";
	if ( items & RecipeDB::Meta ) command += ",ctime,mtime,atime";
	command += " FROM recipes"+(ids_str.count()!=0?" WHERE id IN ("+ids_str.join(",")+")":"");

	QSQLiteResult recipeToLoad = database->executeQuery( command );

	if ( recipeToLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = recipeToLoad.first();
		while ( !recipeToLoad.atEnd() ) {
			Recipe recipe;
			recipe.recipeID = row.data( "id" ).toInt();

			if ( items & RecipeDB::Title )
				recipe.title = unescapeAndDecode( row.data( "title" ) );

			if ( items & RecipeDB::Instructions )
				recipe.instructions = unescapeAndDecode( row.data( "instructions" ) );

			if ( items & RecipeDB::Yield ) {
				recipe.yield.amount = row.data( "yield_amount" ).toDouble();
				recipe.yield.amount_offset = row.data( "yield_amount_offset" ).toDouble();
				recipe.yield.type_id = row.data( "yield_type_id" ).toInt();
				if ( recipe.yield.type_id != -1 ) {
					QString y_command = QString("SELECT name FROM yield_types WHERE id=%1;").arg(recipe.yield.type_id);
					QSQLiteResult yield_query = database->executeQuery(y_command);
					if ( yield_query.getStatus() != QSQLiteResult::Failure ) {
						QSQLiteResultRow row = yield_query.first();
						recipe.yield.type = unescapeAndDecode(row.data( 0 ));
					}
				}
			}

			if ( items & RecipeDB::PrepTime ) {
				recipe.prepTime = QTime::fromString( row.data( "prep_time" ) );
			}

			if ( items & RecipeDB::Meta ) {
				recipe.ctime = QDateTime::fromString( row.data( "ctime" ), Qt::ISODate );
				recipe.mtime = QDateTime::fromString( row.data( "mtime" ), Qt::ISODate );
				recipe.atime = QDateTime::fromString( row.data( "atime" ), Qt::ISODate );
			}

			recipeIterators[ recipe.recipeID ] = rlist->append( recipe );
			row = recipeToLoad.next();
		}
	}
	else {
		kdDebug() << recipeToLoad.getError();
		return ; // There were problems while loading the recipe
	}

	// Read the ingredients
	if ( items & RecipeDB::Ingredients ) {
		for ( RecipeList::iterator recipe_it = rlist->begin(); recipe_it != rlist->end(); ++recipe_it ) {
			RecipeList::iterator it = recipeIterators[ (*recipe_it).recipeID ];
			if ( !(items & RecipeDB::NamesOnly) )
				command = QString( "SELECT il.ingredient_id,i.name,il.amount,il.amount_offset,u.id,u.name,u.plural,il.group_id,il.id FROM ingredient_list il LEFT JOIN ingredients i ON (i.id=il.ingredient_id) LEFT JOIN units u  ON (u.id=il.unit_id) WHERE il.recipe_id=%1 ORDER BY il.order_index;" ).arg( (*it).recipeID );
			else
				command = QString( "SELECT il.ingredient_id,i.name FROM ingredient_list il LEFT JOIN ingredients i ON (i.id=il.ingredient_id) WHERE il.recipe_id=%1 ORDER BY il.order_index;" ).arg( (*it).recipeID );
		
			recipeToLoad = database->executeQuery( command );
			if ( recipeToLoad.getStatus() != QSQLiteResult::Failure ) {
				QSQLiteResultRow row = recipeToLoad.first();
				while ( !recipeToLoad.atEnd() ) {
					Ingredient ing;
					ing.ingredientID = row.data( 0 ).toInt();
					ing.name = unescapeAndDecode( row.data( 1 ) );

					if ( !(items & RecipeDB::NamesOnly) ) {
						ing.amount = row.data( 2 ).toDouble();
						ing.amount_offset = row.data( 3 ).toDouble();
						ing.unitID = row.data( 4 ).toInt();
						ing.units.name = unescapeAndDecode( row.data( 5 ) );
						ing.units.plural = unescapeAndDecode( row.data( 6 ) );
			
						//if we don't have both name and plural, use what we have as both
						if ( ing.units.name.isEmpty() )
							ing.units.name = ing.units.plural;
						else if ( ing.units.plural.isEmpty() )
							ing.units.plural = ing.units.name;
			
						ing.groupID = row.data( 7 ).toInt();
			
						command = QString("SELECT pl.prep_method_id,p.name FROM prep_methods p, prep_method_list pl WHERE pl.ingredient_list_id=%1 AND p.id=pl.prep_method_id ORDER BY pl.order_index;").arg(row.data( 8 ).toInt());
						QSQLiteResult toLoad = database->executeQuery( command );
						if ( toLoad.getStatus() != QSQLiteResult::Failure ) {
							QSQLiteResultRow prep_row = toLoad.first();
							while ( !toLoad.atEnd() ) {
								ing.prepMethodList.append( Element(prep_row.data(1),prep_row.data(0).toInt()) );
								prep_row = toLoad.next();
							}
						}
			
						if ( ing.groupID != -1 ) {
							QSQLiteResult toLoad = database->executeQuery( QString( "SELECT name FROM ingredient_groups WHERE id=%1" ).arg( ing.groupID ) );
							if ( toLoad.getStatus() != QSQLiteResult::Failure ) {
								QSQLiteResultRow row = toLoad.first();
								if ( !toLoad.atEnd() )
									ing.group = unescapeAndDecode( row.data( 0 ) );
							}
						}
					}
		
					(*it).ingList.append( ing );
					row = recipeToLoad.next();
				}
			}
		}
	}

	//Load the Image
	if ( items & RecipeDB::Photo ) {
		for ( RecipeList::iterator recipe_it = rlist->begin(); recipe_it != rlist->end(); ++recipe_it ) {
			RecipeList::iterator it = recipeIterators[ (*recipe_it).recipeID ];
			command = QString( "SELECT photo FROM recipes WHERE id=%1;" ).arg( (*it).recipeID );
		
			recipeToLoad = database->executeQuery( command );
		
			if ( recipeToLoad.getStatus() != QSQLiteResult::Failure ) {
				QSQLiteResultRow row = recipeToLoad.first();
				if ( !recipeToLoad.atEnd() ) {
					bool ok;
					QString photoString = row.data( 0, &ok );
		
					// Decode the photo
					uchar *photoArray = new uchar [ photoString.length() + 1 ];
					memcpy( photoArray, photoString.latin1(), photoString.length() * sizeof( char ) );
					sqlite_decode_binary( ( uchar* ) photoArray, ( uchar* ) photoArray );
	
					if ( ok )
						(*it).photo.loadFromData( photoArray, photoString.length() );
					// picture will now have a ready-to-use image
					delete[] photoArray;
				}
			}
		}
	}

	//Load the category list
	if ( items & RecipeDB::Categories ) {
		for ( RecipeList::iterator recipe_it = rlist->begin(); recipe_it != rlist->end(); ++recipe_it ) {
			RecipeList::iterator it = recipeIterators[ (*recipe_it).recipeID ];
			command = QString( "SELECT cl.category_id,c.name FROM category_list cl, categories c WHERE recipe_id=%1 AND cl.category_id=c.id;" ).arg( (*it).recipeID );
		
			recipeToLoad = database->executeQuery( command );
			if ( recipeToLoad.getStatus() != QSQLiteResult::Failure ) {
				QSQLiteResultRow row = recipeToLoad.first();
				while ( !recipeToLoad.atEnd() ) {
					Element el;
					el.id = row.data( 0 ).toInt();
					el.name = unescapeAndDecode( row.data( 1 ) );
					(*it).categoryList.append( el );
					row = recipeToLoad.next();
				}
			}
		}
	}

	//Load the author list
	if ( items & RecipeDB::Authors ) {
		for ( RecipeList::iterator recipe_it = rlist->begin(); recipe_it != rlist->end(); ++recipe_it ) {
			RecipeList::iterator it = recipeIterators[ (*recipe_it).recipeID ];

			command = QString( "SELECT al.author_id,a.name FROM author_list al, authors a WHERE recipe_id=%1 AND al.author_id=a.id;" ).arg( (*it).recipeID );
		
			recipeToLoad = database->executeQuery( command );
			if ( recipeToLoad.getStatus() != QSQLiteResult::Failure ) {
				QSQLiteResultRow row = recipeToLoad.first();
				while ( !recipeToLoad.atEnd() ) {
					Element el;
					el.id = row.data( 0 ).toInt();
					el.name = unescapeAndDecode( row.data( 1 ) );
					(*it).authorList.append( el );
					row = recipeToLoad.next();
				}
			}
		}
	}
}

void LiteRecipeDB::loadIngredientGroups( ElementList *list )
{
	list->clear();

	QString command;
	command = "SELECT DISTINCT name FROM ingredient_groups ORDER BY name;";
	QSQLiteResult toLoad = database->executeQuery( command );

	if ( toLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = toLoad.first();
		while ( !toLoad.atEnd() ) {
			Element group;
			group.name = unescapeAndDecode( row.data( 0 ) );
			list->append( group );
			row = toLoad.next();
		}
	}
}

void LiteRecipeDB::loadIngredients( ElementList *list, int limit, int offset )
{
	list->clear();

	QString command;
	command = "SELECT id,name FROM ingredients ORDER BY name"
	  +((limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset));

	QSQLiteResult ingredientToLoad = database->executeQuery( command );

	if ( ingredientToLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = ingredientToLoad.first();
		while ( !ingredientToLoad.atEnd() ) {
			Element ing;
			ing.id = row.data( 0 ).toInt();
			ing.name = unescapeAndDecode( row.data( 1 ) );
			list->append( ing );
			row = ingredientToLoad.next();
		}
	}
}

void LiteRecipeDB::loadPrepMethods( ElementList *list, int limit, int offset )
{
	list->clear();

	QString command = "SELECT id,name FROM prep_methods ORDER BY name"
	  +((limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset));
	QSQLiteResult prepMethodsToLoad = database->executeQuery( command );

	if ( prepMethodsToLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = prepMethodsToLoad.first();
		while ( !prepMethodsToLoad.atEnd() ) {
			Element prep_method;
			prep_method.id = row.data( 0 ).toInt();
			prep_method.name = unescapeAndDecode( row.data( 1 ) );
			list->append( prep_method );
			row = prepMethodsToLoad.next();
		}
	}
}

void LiteRecipeDB::loadYieldTypes( ElementList *list, int limit, int offset )
{
	list->clear();

	QString command = "SELECT id,name FROM yield_types ORDER BY name"
	  +((limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset));
	QSQLiteResult toLoad = database->executeQuery( command );

	if ( toLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = toLoad.first();
		while ( !toLoad.atEnd() ) {
			Element el;
			el.id = row.data( 0 ).toInt();
			el.name = unescapeAndDecode( row.data( 1 ) );
			list->append( el );
			row = toLoad.next();
		}
	}
}

void LiteRecipeDB::loadPossibleUnits( int ingredientID, UnitList *list )
{
	list->clear();

	QString command;

	command = QString( "SELECT u.id,u.name,u.plural FROM unit_list ul, units u WHERE ul.ingredient_id=%1 AND ul.unit_id=u.id;" ).arg( ingredientID );

	QSQLiteResult unitToLoad = database->executeQuery( command );

	if ( unitToLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = unitToLoad.first();
		if ( !unitToLoad.atEnd() ) {

			while ( !unitToLoad.atEnd() ) {
				Unit unit;
				unit.id = row.data( 0 ).toInt();
				unit.name = unescapeAndDecode( row.data( 1 ) );
				unit.plural = unescapeAndDecode( row.data( 2 ) );
				list->append( unit );
				row = unitToLoad.next();
			}
		}
	}

}

void LiteRecipeDB::saveRecipe( Recipe *recipe )
{
	// Check if it's a new recipe or it exists (supossedly) already.

	bool newRecipe;
	newRecipe = ( recipe->recipeID == -1 );
	// First check if the recipe ID is set, if so, update (not create)
	// Be carefull, first check if the recipe hasn't been deleted while changing.

	QString command;

	QDateTime current_datetime = QDateTime::currentDateTime();
	QString current_timestamp = current_datetime.toString(Qt::ISODate);
	if ( newRecipe ) {
		command = QString( "INSERT INTO recipes VALUES (NULL,'%1',%2,'%3','%4','%5',NULL,'%6','%7','%8','%9');" )  // Id is autoincremented
		          .arg( escapeAndEncode( recipe->title ) )
		          .arg( recipe->yield.amount )
		          .arg( recipe->yield.amount_offset )
		          .arg( recipe->yield.type_id )
		          .arg( escapeAndEncode( recipe->instructions ) )
		          .arg( recipe->prepTime.toString( "hh:mm:ss" ) )
		          .arg( current_timestamp )
		          .arg( current_timestamp )
		          .arg( current_timestamp );
		recipe->mtime = recipe->ctime = recipe->atime = current_datetime;
	}
	else {
		command = QString( "UPDATE recipes SET title='%1',yield_amount='%2',yield_amount_offset='%3',yield_type_id='%4',instructions='%5',prep_time='%6',mtime='%8' WHERE id=%7;" )
		          .arg( escapeAndEncode( recipe->title ) )
		          .arg( recipe->yield.amount )
		          .arg( recipe->yield.amount_offset )
		          .arg( recipe->yield.type_id )
		          .arg( escapeAndEncode( recipe->instructions ) )
		          .arg( recipe->prepTime.toString( "hh:mm:ss" ) )
		          .arg( recipe->recipeID )
		          .arg( current_timestamp );
		recipe->mtime = current_datetime;
	}

	int lastID = -1;
	QSQLiteResult insertedRecipe = database->executeQuery( command, &lastID );
	// If it's a new recipe, identify the ID that was given to the recipe and store in the Recipe itself


	if ( newRecipe ) {
		recipe->recipeID = lastID; // store the new ID in the recipe
	}

	int recipeID = recipe->recipeID; // Set the recipeID (either new or old)


	// Let's begin storing the Image
	if ( !recipe->photo.isNull() ) {
		KTempFile * fn = new KTempFile ( locateLocal( "tmp", "kre" ), ".jpg", 0600 );
		fn->setAutoDelete( true );
		recipe->photo.save( fn->name(), "JPEG" );

		QFileInfo fi( fn->name() );

		//load to photoArray
		char * photoArray;
		long unsigned int n = fi.size();
		photoArray = new char[ n ];
		if ( fn ) {
			fn->file() ->readBlock( photoArray, n );
			fn->close();
		}
		delete fn;

		char *photoEncodedArray = new char[ 2 + ( 257 * n ) / 254 + 1 ]; //Just in case, add+1
		sqlite_encode_binary( ( uchar* ) photoArray, fi.size(), ( uchar* ) photoEncodedArray );
		command = QString( "UPDATE recipes SET photo='%2' WHERE id=%1;" ).arg( recipeID ).arg( photoEncodedArray ); //insert binary data LAST, otherwise if the data contains a '%2', it will be replaced
		database->executeQuery( command );
	}
	else {
		database->executeQuery( "UPDATE recipes SET photo=NULL WHERE id=" + QString::number( recipeID ) );
	}

	// Save the ingredient list (first delete in case we are updating)
	command = QString( "DELETE FROM prep_method_list WHERE ingredient_list_id IN (SELECT id FROM ingredient_list WHERE recipe_id=%1)" ).arg(recipeID);
	database->executeQuery( command );

	command = QString( "DELETE FROM ingredient_list WHERE recipe_id=%1;" )
	          .arg( recipeID );
	database->executeQuery( command );

	int order_index = 0;
	for ( IngredientList::const_iterator ing_it = recipe->ingList.begin(); ing_it != recipe->ingList.end(); ++ing_it ) {
		order_index++;
		command = QString( "INSERT INTO ingredient_list VALUES (NULL,%1,%2,%3,%4,%5,%6,%7);" )
		          .arg( recipeID )
		          .arg( ( *ing_it ).ingredientID )
		          .arg( ( *ing_it ).amount )
		          .arg( ( *ing_it ).amount_offset )
		          .arg( ( *ing_it ).unitID )
		          .arg( order_index )
		          .arg( ( *ing_it ).groupID );
		database->executeQuery( command );

		int ing_list_id = lastInsertID();
		int prep_order_index = 0;
		for ( ElementList::const_iterator prep_it = (*ing_it).prepMethodList.begin(); prep_it != (*ing_it).prepMethodList.end(); ++prep_it ) {
			prep_order_index++;
			command = QString( "INSERT INTO prep_method_list VALUES (%1,%2,%3);" )
				.arg( ing_list_id )
				.arg( ( *prep_it ).id )
				.arg( prep_order_index );
			database->executeQuery( command );
		}
	}

	// Save the category list for the recipe (first delete, in case we are updating)
	command = QString( "DELETE FROM category_list WHERE recipe_id=%1;" )
	          .arg( recipeID );
	database->executeQuery( command );

	ElementList::const_iterator cat_it = recipe->categoryList.end(); // Start from last, mysql seems to work in lifo format... so it's read first the latest inserted one (newest)
	--cat_it;
	for ( unsigned int i = 0; i < recipe->categoryList.count(); i++ ) {
		command = QString( "INSERT INTO category_list VALUES (%1,%2);" )
		          .arg( recipeID )
		          .arg( ( *cat_it ).id );
		database->executeQuery( command );

		--cat_it;
	}

	// Add the default category -1 to ease and speed up searches

	command = QString( "INSERT INTO category_list VALUES (%1,-1);" )
	          .arg( recipeID );
	database->executeQuery( command );

	// Save the author list for the recipe (first delete, in case we are updating)
	command = QString( "DELETE FROM author_list WHERE recipe_id=%1;" )
	          .arg( recipeID );
	database->executeQuery( command );

	ElementList::const_iterator author_it = recipe->authorList.end(); // Start from last, mysql seems to work in lifo format... so it's read first the latest inserted one (newest)
	--author_it;
	for ( unsigned int i = 0; i < recipe->authorList.count(); i++ ) {
		command = QString( "INSERT INTO author_list VALUES (%1,%2);" )
		          .arg( recipeID )
		          .arg( ( *author_it ).id );
		database->executeQuery( command );

		--author_it;
	}

	if ( newRecipe )
		emit recipeCreated( Element( recipe->title.left( maxRecipeTitleLength() ), recipeID ), recipe->categoryList );
	else
		emit recipeModified( Element( recipe->title.left( maxRecipeTitleLength() ), recipeID ), recipe->categoryList );
}

void LiteRecipeDB::loadRecipeList( ElementList *list, int categoryID, bool recursive )
{
	QString command;
	QString outputData;

	if ( recursive ) {
		QSQLiteResult subcategories = database->executeQuery( QString("SELECT id FROM categories WHERE parent_id='%1'").arg(categoryID) );
		if ( subcategories.getStatus() != QSQLiteResult::Failure ) {
			QSQLiteResultRow row = subcategories.first();
			while ( !subcategories.atEnd() ) {
				loadRecipeList(list,row.data( 0 ).toInt(),true);

				row = subcategories.next();
			}
		}
	}

	// Load the recipe list
	if ( categoryID == -1 )  // load just the list
		command = "SELECT id,title FROM recipes;";
	else  // load the list of those in the specified category
	{
			command = QString( "SELECT r.id,r.title FROM recipes r,category_list cl WHERE r.id=cl.recipe_id AND cl.category_id=%1 ORDER BY r.title;" ).arg( categoryID );
	}

	QSQLiteResult recipeToLoad = database->executeQuery( command );
	if ( recipeToLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = recipeToLoad.first();
		while ( !recipeToLoad.atEnd() ) {
			Element recipe;
			recipe.id = row.data( 0 ).toInt();
			recipe.name = unescapeAndDecode( row.data( 1 ) );
			list->append( recipe );

			row = recipeToLoad.next();
		}
	}

}

void LiteRecipeDB::loadUncategorizedRecipes( ElementList *list )
{
	list->clear();

	QString command;

	// Load the recipe list

	command = "SELECT r.id,r.title FROM recipes r,category_list cl WHERE r.id=cl.recipe_id GROUP BY id HAVING COUNT(*)=1 ORDER BY r.title DESC";
	QSQLiteResult recipeToLoad = database->executeQuery( command );
	if ( recipeToLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = recipeToLoad.first();
		while ( !recipeToLoad.atEnd() ) {

			Element recipe;
			recipe.id = row.data( 0 ).toInt();
			recipe.name = unescapeAndDecode( row.data( 1 ) );
			list->append( recipe );

			row = recipeToLoad.next();

		}
	}
}

void LiteRecipeDB::removeRecipe( int id )
{
	emit recipeRemoved( id );

	QString command;

	command = QString( "DELETE FROM recipes WHERE id=%1;" ).arg( id );
	database->executeQuery( command );
	command = QString( "DELETE FROM ingredient_list WHERE recipe_id=%1;" ).arg( id );
	database->executeQuery( command );
	command = QString( "DELETE FROM category_list WHERE recipe_id=%1;" ).arg( id );
	database->executeQuery( command );

	// Clean up ingredient_groups which have no recipe that they belong to
	command = "DELETE FROM ingredient_groups WHERE id NOT IN ( SELECT DISTINCT(group_id) FROM ingredient_list );";
	database->executeQuery( command );
}

void LiteRecipeDB::removeRecipeFromCategory( int recipeID, int categoryID )
{
	QString command;
	command = QString( "DELETE FROM category_list WHERE recipe_id=%1 AND category_id=%2;" ).arg( recipeID ).arg( categoryID );
	database->executeQuery( command );

	emit recipeRemoved( recipeID, categoryID );
}

void LiteRecipeDB::createNewIngGroup( const QString &name )
{
	QString command;
	QString real_name = name.left( maxIngGroupNameLength() );

	command = QString( "INSERT INTO ingredient_groups VALUES(NULL,'%1');" ).arg( escapeAndEncode( real_name ) );
	database->executeQuery( command );
}

void LiteRecipeDB::createNewIngredient( const QString &ingredientName )
{
	QString command;
	QString real_name = ingredientName.left( maxIngredientNameLength() );

	command = QString( "INSERT INTO ingredients VALUES(NULL,'%1');" ).arg( escapeAndEncode( real_name ) );
	database->executeQuery( command );

	emit ingredientCreated( Element( real_name, lastInsertID() ) );
}

void LiteRecipeDB::createNewYieldType( const QString &name )
{
	QString command;
	QString real_name = name.left( maxYieldTypeLength() );

	command = QString( "INSERT INTO yield_types VALUES(NULL,'%1');" ).arg( escapeAndEncode( real_name ) );
	database->executeQuery( command );

	//emit yieldCreated( Element( real_name, lastInsertID() ) );
}

void LiteRecipeDB::modIngredient( int ingredientID, const QString &newLabel )
{
	QString command;

	command = QString( "UPDATE ingredients SET name='%1' WHERE id=%2;" ).arg( escapeAndEncode( newLabel ) ).arg( ingredientID );
	database->executeQuery( command );

	emit ingredientRemoved( ingredientID );
	emit ingredientCreated( Element( newLabel, ingredientID ) );
}

void LiteRecipeDB::addUnitToIngredient( int ingredientID, int unitID )
{
	QString command;

	command = QString( "INSERT INTO unit_list VALUES(%1,%2);" ).arg( ingredientID ).arg( unitID );
	database->executeQuery( command );
}

void LiteRecipeDB::loadUnits( UnitList *list, int limit, int offset )
{
	list->clear();

	QString command;

	command = "SELECT id,name,plural FROM units ORDER BY name"
	  +((limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset));

	QSQLiteResult unitToLoad = database->executeQuery( command );
	if ( unitToLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = unitToLoad.first();
		while ( !unitToLoad.atEnd() ) {
			Unit unit;
			unit.id = row.data( 0 ).toInt();
			unit.name = unescapeAndDecode( row.data( 1 ) );
			unit.plural = unescapeAndDecode( row.data( 2 ) );
			list->append( unit );
			row = unitToLoad.next();
		}
	}
}

void LiteRecipeDB::removeUnitFromIngredient( int ingredientID, int unitID )
{
	QString command;

	command = QString( "DELETE FROM unit_list WHERE ingredient_id=%1 AND unit_id=%2;" ).arg( ingredientID ).arg( unitID );
	database->executeQuery( command );

	// Remove any recipe using that combination of ingredients also (user must have been warned before calling this function!)

	command = QString( "SELECT r.id FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1 AND il.unit_id=%2;" ).arg( ingredientID ).arg( unitID );
	QSQLiteResult query = database->executeQuery( command );
	if ( query.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = query.first();
		while ( !query.atEnd() ) {
			emit recipeRemoved( row.data( 0 ).toInt() );
			database->executeQuery( QString( "DELETE FROM recipes WHERE id=%1;" ).arg( row.data( 0 ).toInt() ) );
			row = query.next();
		}
	}

	// Remove any ingredient in ingredient_list which has references to this unit and ingredient
	database->executeQuery( QString( "DELETE FROM ingredient_list WHERE ingredient_id=%1 AND unit_id=%2;" ).arg( ingredientID ).arg( unitID ) );

	// Remove any ingredient properties from ingredient_info where the this ingredient+unit is being used (user must have been warned before calling this function!)
	command = QString( "DELETE FROM ingredient_info WHERE ingredient_id=%1 AND per_units=%2;" ).arg( ingredientID ).arg( unitID );
	database->executeQuery( command );
	// Clean up ingredient_list which have no recipe that they belong to.
	command = QString( "DELETE FROM ingredient_list WHERE recipe_id NOT IN ( SELECT id FROM recipes );" );
	database->executeQuery( command );

	// Clean up ingredient_groups which have no recipe that they belong to
	command = "DELETE FROM ingredient_groups WHERE id NOT IN ( SELECT DISTINCT(group_id) FROM ingredient_list );";
	database->executeQuery( command );

	// Clean up category_list which have no recipe that they belong to. Same method as above
	command = QString( "DELETE FROM category_list WHERE recipe_id NOT IN ( SELECT id FROM recipes );" );
	database->executeQuery( command );
}


void LiteRecipeDB::findUseOf_Ing_Unit_InRecipes( ElementList *results, int ingredientID, int unitID )
{
	QString command = QString( "SELECT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1 AND il.unit_id=%2" ).arg( ingredientID ).arg( unitID );
	QSQLiteResult recipeFound = database->executeQuery( command ); // Find the entries

	// Populate data in the ElementList*
	if ( recipeFound.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = recipeFound.first();
		while ( !recipeFound.atEnd() ) {
			Element recipe;
			recipe.id = row.data( 0 ).toInt();
			recipe.name = unescapeAndDecode( row.data( 1 ) );
			results->append( recipe );
			row = recipeFound.next();
		}
	}
}

void LiteRecipeDB::findUseOfIngInRecipes( ElementList *results, int ingredientID )
{
	QString command = QString( "SELECT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1" ).arg( ingredientID );
	QSQLiteResult recipeFound = database->executeQuery( command ); // Find the entries

	// Populate data in the ElementList*
	if ( recipeFound.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = recipeFound.first();
		while ( !recipeFound.atEnd() ) {
			Element recipe;
			recipe.id = row.data( 0 ).toInt();
			recipe.name = unescapeAndDecode( row.data( 1 ) );
			results->append( recipe );
			row = recipeFound.next();
		}
	}
}

void LiteRecipeDB::removeIngredient( int ingredientID )
{
	QString command;

	// First remove the ingredient

	command = QString( "DELETE FROM ingredients WHERE id=%1;" ).arg( ingredientID );
	database->executeQuery( command );

	// Remove all the unit entries for this ingredient

	command = QString( "DELETE FROM unit_list WHERE ingredient_id=%1;" ).arg( ingredientID );
	database->executeQuery( command );

	// Remove any recipe using that ingredient

	command = QString( "SELECT r.id FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1;" ).arg( ingredientID );
	QSQLiteResult query = database->executeQuery( command );
	if ( query.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = query.first();
		while ( !query.atEnd() ) {
			emit recipeRemoved( row.data( 0 ).toInt() );
			database->executeQuery( QString( "DELETE FROM recipes WHERE id=%1;" ).arg( row.data( 0 ).toInt() ) );
			row = query.next();
		}
	}

	// Remove any ingredient in ingredient_list which has references to this ingredient
	database->executeQuery( QString( "DELETE FROM ingredient_list WHERE ingredient_id=%1;" ).arg( ingredientID ) );

	// Remove any ingredient in ingredient_list whis has references to inexisting recipes.
	command = QString( "DELETE FROM ingredient_list WHERE recipe_id NOT IN ( SELECT id FROM recipes );" );
	database->executeQuery( command );

	// Clean up ingredient_groups which have no recipe that they belong to
	command = "DELETE FROM ingredient_groups WHERE id NOT IN ( SELECT DISTINCT(group_id) FROM ingredient_list );";
	database->executeQuery( command );

	// Clean up category_list which have no recipe that they belong to. Same method as above
	command = QString( "DELETE FROM category_list WHERE recipe_id NOT IN ( SELECT id FROM recipes );" );
	database->executeQuery( command );

	// Remove property list of this ingredient
	command = QString( "DELETE FROM ingredient_info WHERE ingredient_id=%1;" ).arg( ingredientID );
	database->executeQuery( command );

	emit ingredientRemoved( ingredientID );
}

void LiteRecipeDB::addProperty( const QString &name, const QString &units )
{
	QString command;
	QString real_name = name.left( maxPropertyNameLength() );

	command = QString( "INSERT INTO ingredient_properties VALUES(NULL,'%1','%2');" ).arg( escapeAndEncode( real_name ) ).arg( escapeAndEncode( units ) );
	database->executeQuery( command );

	emit propertyCreated( IngredientProperty( real_name, units, lastInsertID() ) );
}

void LiteRecipeDB::loadProperties( IngredientPropertyList *list, int ingredientID )
{
	list->clear();
	QString command;
	bool usePerUnit;
	if ( ingredientID >= 0 )  // Load properties of this ingredient
	{
		usePerUnit = true;
		command = QString( "SELECT ip.id,ip.name,ip.units,ii.per_units,u.name,ii.amount,ii.ingredient_id FROM ingredient_properties ip, ingredient_info ii, units u WHERE ii.ingredient_id=%1 AND ii.property_id=ip.id AND ii.per_units=u.id;" ).arg( ingredientID );
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

	QSQLiteResult propertiesToLoad = database->executeQuery( command );
	// Load the results into the list
	if ( propertiesToLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = propertiesToLoad.first();
		while ( !propertiesToLoad.atEnd() ) {
			IngredientProperty prop;
			prop.id = row.data( 0 ).toInt();
			prop.name = unescapeAndDecode( row.data( 1 ) );
			prop.units = unescapeAndDecode( row.data( 2 ) );
			if ( usePerUnit ) {
				prop.perUnit.id = row.data( 3 ).toInt();
				prop.perUnit.name = unescapeAndDecode( row.data( 4 ) );
			}
			else {
				prop.perUnit.id = -1;
				prop.perUnit.name = QString::null;
			}

			if ( ingredientID >= -1 )
				prop.amount = row.data( 5 ).toDouble();
			else
				prop.amount = -1; // Property is generic, not attached to an ingredient

			if ( ingredientID >= -1 )
				prop.ingredientID = row.data( 6 ).toInt();

			list->add( prop );




			row = propertiesToLoad.next();
		}

	}
}

void LiteRecipeDB::changePropertyAmountToIngredient( int ingredientID, int propertyID, double amount, int per_units )
{
	QString command;
	command = QString( "UPDATE ingredient_info SET amount=%1 WHERE ingredient_id=%2 AND property_id=%3 AND per_units=%4;" ).arg( amount ).arg( ingredientID ).arg( propertyID ).arg( per_units );
	database->executeQuery( command );
}

void LiteRecipeDB::addPropertyToIngredient( int ingredientID, int propertyID, double amount, int perUnitsID )
{
	QString command;

	command = QString( "INSERT INTO ingredient_info VALUES(%1,%2,%3,%4);" ).arg( ingredientID ).arg( propertyID ).arg( amount ).arg( perUnitsID );
	database->executeQuery( command );
}


void LiteRecipeDB::removePropertyFromIngredient( int ingredientID, int propertyID, int perUnitID )
{
	QString command;
	// remove property from ingredient info. Note that there could be duplicates with different units (per_units). Remove just the one especified.
	command = QString( "DELETE FROM ingredient_info WHERE ingredient_id=%1 AND property_id=%2 AND per_units=%3;" ).arg( ingredientID ).arg( propertyID ).arg( perUnitID );
	database->executeQuery( command );
}

void LiteRecipeDB::removeProperty( int propertyID )
{
	QString command;

	// Remove property from the ingredient_properties
	command = QString( "DELETE FROM ingredient_properties WHERE id=%1;" ).arg( propertyID );
	database->executeQuery( command );

	// Remove any ingredient info that uses this property
	command = QString( "DELETE FROM ingredient_info WHERE property_id=%1;" ).arg( propertyID );
	database->executeQuery( command );

	emit propertyRemoved( propertyID );
}

void LiteRecipeDB::removePrepMethod( int prepMethodID )
{
	QString command;
	// Remove the unit first
	command = QString( "DELETE FROM prep_methods WHERE id=%1;" ).arg( prepMethodID );
	database->executeQuery( command );

	// Remove any recipe using that unit in the ingredient list (user must have been warned before calling this function!)

	command = QString( "SELECT r.id FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.prep_method_id=%1;" ).arg( prepMethodID );
	QSQLiteResult query = database->executeQuery( command );
	if ( query.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = query.first();
		while ( !query.atEnd() ) {
			emit recipeRemoved( row.data( 0 ).toInt() );
			database->executeQuery( QString( "DELETE FROM recipes WHERE id=%1;" ).arg( row.data( 0 ).toInt() ) );
			row = query.next();
		}
	}

	// Remove any ingredient in ingredient_list which has references to this prep method
	database->executeQuery( QString( "DELETE FROM ingredient_list WHERE prep_method_id=%1;" ).arg( prepMethodID ) );

	// Remove any ingredient in ingredient_list whis has references to inexisting recipes
	command = QString( "DELETE FROM ingredient_list WHERE recipe_id NOT IN ( SELECT id FROM recipes );" );
	database->executeQuery( command );

	// Clean up ingredient_groups which have no recipe that they belong to
	command = "DELETE FROM ingredient_groups WHERE id NOT IN ( SELECT DISTINCT(group_id) FROM ingredient_list );";
	database->executeQuery( command );

	// Clean up category_list which have no recipe that they belong to. Same method as above
	command = QString( "DELETE FROM category_list WHERE recipe_id NOT IN ( SELECT id FROM recipes );" );
	database->executeQuery( command );

	emit prepMethodRemoved( prepMethodID );
}

void LiteRecipeDB::modPrepMethod( int prepMethodID, const QString &newLabel )
{
	QString command;

	command = QString( "UPDATE prep_methods SET name='%1' WHERE id=%2;" ).arg( escapeAndEncode( newLabel ) ).arg( prepMethodID );
	database->executeQuery( command );

	emit prepMethodRemoved( prepMethodID );
	emit prepMethodCreated( Element( newLabel, prepMethodID ) );
}

void LiteRecipeDB::modProperty( int propertyID, const QString &newLabel )
{
	QString command;

	command = QString( "UPDATE ingredient_properties SET name='%1' WHERE id=%2;" ).arg( escapeAndEncode( newLabel ) ).arg( propertyID );
	database->executeQuery( command );

	emit propertyRemoved( propertyID );
	emit propertyCreated( propertyName( propertyID ) );
}

void LiteRecipeDB::createNewPrepMethod( const QString &prepMethodName )
{
	QString command;
	QString real_name = prepMethodName.left( maxPrepMethodNameLength() );

	command = QString( "INSERT INTO prep_methods VALUES(NULL,'%1');" ).arg( escapeAndEncode( real_name ) );
	database->executeQuery( command );

	emit prepMethodCreated( Element( real_name, lastInsertID() ) );
}

void LiteRecipeDB::removeUnit( int unitID )
{
	QString command;
	// Remove the unit first
	command = QString( "DELETE FROM units WHERE id=%1;" ).arg( unitID );
	database->executeQuery( command );

	//Remove the unit from ingredients using it

	command = QString( "DELETE FROM unit_list WHERE unit_id=%1;" ).arg( unitID );
	database->executeQuery( command );


	// Remove any recipe using that unit in the ingredient list (user must have been warned before calling this function!)

	command = QString( "SELECT r.id FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.unit_id=%1;" ).arg( unitID );
	QSQLiteResult query = database->executeQuery( command );
	if ( query.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = query.first();
		while ( !query.atEnd() ) {
			emit recipeRemoved( row.data( 0 ).toInt() );
			database->executeQuery( QString( "DELETE FROM recipes WHERE id=%1;" ).arg( row.data( 0 ).toInt() ) );
			row = query.next();
		}
	}

	// Remove any ingredient in ingredient_list which has references to this unit
	database->executeQuery( QString( "DELETE FROM ingredient_list WHERE unit_id=%1;" ).arg( unitID ) );

	// Remove any ingredient in ingredient_list whis has references to inexisting recipes.
	command = QString( "DELETE FROM ingredient_list WHERE recipe_id NOT IN ( SELECT id FROM recipes );" );
	database->executeQuery( command );

	// Clean up ingredient_groups which have no recipe that they belong to
	command = "DELETE FROM ingredient_groups WHERE id NOT IN ( SELECT DISTINCT(group_id) FROM ingredient_list );";
	database->executeQuery( command );

	// Clean up category_list which have no recipe that they belong to. Same method as above
	command = QString( "DELETE FROM category_list WHERE recipe_id NOT IN ( SELECT id FROM recipes );" );
	database->executeQuery( command );

	// Remove the ingredient properties using this unit (user must be warned before calling this function)
	command = QString( "DELETE FROM ingredient_info WHERE per_units=%1;" ).arg( unitID );
	database->executeQuery( command );

	// Remove the unit conversion ratios with this unit
	command = QString( "DELETE FROM units_conversion WHERE unit1_id=%1 OR unit2_id=%2;" ).arg( unitID ).arg( unitID );
	database->executeQuery( command );

	emit unitRemoved( unitID );
}

void LiteRecipeDB::createNewUnit( const QString &unitName, const QString &unitPlural )
{
	QString command;
	QString real_name = unitName.left( maxUnitNameLength() ).stripWhiteSpace();
	QString real_plural = unitPlural.left( maxUnitNameLength() ).stripWhiteSpace();

	if ( real_name.isEmpty() )
		real_name = real_plural;
	else if ( real_plural.isEmpty() )
		real_plural = real_name;

	command = QString( "INSERT INTO units VALUES(NULL,'" + escapeAndEncode( real_name ) + "','" + escapeAndEncode( real_plural ) + "');" );
	database->executeQuery( command );

	emit unitCreated( Unit( real_name, real_plural, lastInsertID() ) );
}


void LiteRecipeDB::modUnit( int unitID, const QString &newName, const QString &newPlural )
{
	QString command;

	database->executeQuery( "UPDATE units SET name='" + QString( escapeAndEncode( newName ) ) + "' WHERE id='" + QString::number( unitID ) + "';" );
	database->executeQuery( "UPDATE units SET plural='" + QString( escapeAndEncode( newPlural ) ) + "' WHERE id='" + QString::number( unitID ) + "';" );

	emit unitRemoved( unitID );
	emit unitCreated( Unit( newName, newPlural, unitID ) );
}

void LiteRecipeDB::findUseOf_Unit_InRecipes( ElementList *results, int unitID )
{
	QString command = QString( "SELECT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.unit_id=%1;" ).arg( unitID );
	QSQLiteResult recipeFound = database->executeQuery( command ); // Find the entries

	// Populate data in the ElementList*
	if ( recipeFound.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = recipeFound.first();
		while ( !recipeFound.atEnd() ) {
			Element recipe;
			recipe.id = row.data( 0 ).toInt();
			recipe.name = unescapeAndDecode( row.data( 1 ) );
			results->append( recipe );
			row = recipeFound.next();
		}
	}
}

void LiteRecipeDB::findUseOf_Unit_InProperties( ElementList *results, int unitID )
{
	QString command = QString( "SELECT ip.id,ip.name FROM ingredient_info ii, ingredient_properties ip WHERE ii.per_units=%1 AND ip.id=ii.property_id;" ).arg( unitID );
	QSQLiteResult recipeFound = database->executeQuery( command ); // Find the entries

	// Populate data in the ElementList*
	if ( recipeFound.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = recipeFound.first();
		while ( !recipeFound.atEnd() ) {
			Element recipe;
			recipe.id = row.data( 0 ).toInt();
			recipe.name = row.data( 1 );
			results->append( recipe );
			row = recipeFound.next();
		}
	}


}

void LiteRecipeDB::loadUnitRatios( UnitRatioList *ratioList )
{
	ratioList->clear();

	QString command;
	command = "SELECT unit1_id,unit2_id,ratio FROM units_conversion;";
	QSQLiteResult ratiosToLoad = database->executeQuery( command );

	if ( ratiosToLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = ratiosToLoad.first();
		while ( !ratiosToLoad.atEnd() ) {
			UnitRatio ratio;
			ratio.uID1 = row.data( 0 ).toInt();
			ratio.uID2 = row.data( 1 ).toInt();
			ratio.ratio = row.data( 2 ).toDouble();
			ratioList->add
			( ratio );
			row = ratiosToLoad.next();
		}
	}
}

void LiteRecipeDB::saveUnitRatio( const UnitRatio *ratio )
{
	QString command;

	// Check if it's a new ratio or it exists already.
	command = QString( "SELECT * FROM units_conversion WHERE unit1_id=%1 AND unit2_id=%2;" ).arg( ratio->uID1 ).arg( ratio->uID2 ); // Find ratio between units

	QSQLiteResult ratioFound = database->executeQuery( command ); // Find the entries
	bool newRatio = ( ratioFound.size() == 0 );

	if ( newRatio )
		command = QString( "INSERT INTO units_conversion VALUES(%1,%2,%3);" ).arg( ratio->uID1 ).arg( ratio->uID2 ).arg( ratio->ratio );
	else
		command = QString( "UPDATE units_conversion SET ratio=%3 WHERE unit1_id=%1 AND unit2_id=%2" ).arg( ratio->uID1 ).arg( ratio->uID2 ).arg( ratio->ratio );

	database->executeQuery( command ); // Enter the new ratio
}

double LiteRecipeDB::unitRatio( int unitID1, int unitID2 )
{

	if ( unitID1 == unitID2 )
		return ( 1.0 );
	QString command;

	command = QString( "SELECT ratio FROM units_conversion WHERE unit1_id=%1 AND unit2_id=%2;" ).arg( unitID1 ).arg( unitID2 );
	QSQLiteResult ratioToLoad = database->executeQuery( command );


	if ( ratioToLoad.getStatus() != QSQLiteResult::Failure ) {
		if ( !ratioToLoad.atEnd() ) {
			QSQLiteResultRow row = ratioToLoad.first();
			return ( row.data( 0 ).toDouble() );
		}
		else
			return ( -1 );
	}
	else
		return ( -1 ); // There is no ratio defined between the units
}


//Finds data dependant on this Ingredient/Unit combination
void LiteRecipeDB::findIngredientUnitDependancies( int ingredientID, int unitID, ElementList *recipes, ElementList *ingredientInfo )
{

	// Recipes using that combination

	QString command = QString( "SELECT DISTINCT r.id,r.title  FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1 AND il.unit_id=%2;" ).arg( ingredientID ).arg( unitID );
	QSQLiteResult unitToRemove = database->executeQuery( command );
	loadElementList( recipes, &unitToRemove );
	// Ingredient info using that combination
	command = QString( "SELECT i.name,ip.name,ip.units,u.name FROM ingredients i, ingredient_info ii, ingredient_properties ip, units u WHERE i.id=ii.ingredient_id AND ii.ingredient_id=%1 AND ii.per_units=%2 AND ii.property_id=ip.id AND ii.per_units=u.id;" ).arg( ingredientID ).arg( unitID );

	unitToRemove = database->executeQuery( command );
	loadPropertyElementList( ingredientInfo, &unitToRemove );
}

void LiteRecipeDB::findIngredientDependancies( int ingredientID, ElementList *recipes )
{
	QString command = QString( "SELECT DISTINCT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1" ).arg( ingredientID );

	QSQLiteResult ingredientToRemove = database->executeQuery( command );
	loadElementList( recipes, &ingredientToRemove );
}



//Finds data dependant on the removal of this Unit
void LiteRecipeDB::findUnitDependancies( int unitID, ElementList *properties, ElementList *recipes )
{

	// Ingredient-Info (ingredient->property) using this Unit

	QString command = QString( "SELECT i.name,ip.name,ip.units,u.name  FROM ingredients i, ingredient_info ii, ingredient_properties ip, units u WHERE i.id=ii.ingredient_id AND ii.per_units=%1 AND ii.property_id=ip.id  AND ii.per_units=u.id;" ).arg( unitID );
	QSQLiteResult unitToRemove = database->executeQuery( command );
	loadPropertyElementList( properties, &unitToRemove );

	// Recipes using this Unit
	command = QString( "SELECT DISTINCT r.id,r.title  FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.unit_id=%1;" ).arg( unitID ); // Without "DISTINCT" we get duplicates since ingredient_list has no unique recipe_id's
	unitToRemove = database->executeQuery( command );
	loadElementList( recipes, &unitToRemove );

}

void LiteRecipeDB::findPrepMethodDependancies( int prepMethodID, ElementList *recipes )
{
	QString command = QString( "SELECT DISTINCT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.prep_method_id=%1" ).arg( prepMethodID );

	QSQLiteResult prepMethodToRemove = database->executeQuery( command );
	loadElementList( recipes, &prepMethodToRemove );
}

void LiteRecipeDB::loadElementList( ElementList *elList, QSQLiteResult *query )
{
	if ( query->getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = query->first();
		while ( !query->atEnd() ) {
			Element el;
			el.id = row.data( 0 ).toInt();
			el.name = unescapeAndDecode( row.data( 1 ) );
			elList->append( el ); // Note that ElementList _copies_, does not take the pointer while adding.
			row = query->next();

		}
	}
}

// See function "findUnitDependancies" for use
void LiteRecipeDB::loadPropertyElementList( ElementList *elList, QSQLiteResult *query )
{
	if ( query->getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = query->first();
		while ( !query->atEnd() ) {
			Element el;
			el.id = -1; // There's no ID for the ingredient-property combination
			QString ingName = row.data( 0 );
			QString propName = unescapeAndDecode( row.data( 1 ) );
			QString propUnits = unescapeAndDecode( row.data( 2 ) );
			QString propPerUnits = unescapeAndDecode( row.data( 3 ) );

			el.name = QString( "In ingredient %1: property \"%2\" [%3/%4]" ).arg( ingName ).arg( propName ).arg( propUnits ).arg( propPerUnits );
			elList->append( el ); // Note that ElementList _copies_, does not take the pointer while adding.
		}
	}
}


QCString LiteRecipeDB::escapeAndEncode( const QString &s ) const
{
	QString s_escaped;

	// Escape
	s_escaped = escape( s.utf8() );

	// Return encoded
	return s_escaped.latin1(); // Note that the text has already been converted into utf8 before escaping.
}

QString LiteRecipeDB::escape( const QString &s ) const
{
	QString s_escaped = s;

	if ( !s_escaped.isEmpty() ) { //###: sqlite_mprintf() seems to fill an empty string with garbage
		// Escape using SQLite's function
#if HAVE_SQLITE
		char * escaped = sqlite_mprintf( "%q", s.latin1() ); // Escape the string(allocates memory)
#elif HAVE_SQLITE3
		char * escaped = sqlite3_mprintf( "%q", s.latin1() ); // Escape the string(allocates memory)
#endif
		s_escaped = escaped;
#if HAVE_SQLITE
		sqlite_freemem( escaped ); // free allocated memory
#elif HAVE_SQLITE3
		sqlite3_free( escaped ); // free allocated memory
#endif
	}

	return ( s_escaped );
}

QString LiteRecipeDB::unescapeAndDecode( const QString &s ) const
{
	QString s_escaped = QString::fromUtf8( s.latin1() );
	s_escaped.replace( ";@", ";" );
	return ( s_escaped ); // Use unicode encoding
}

bool LiteRecipeDB::ingredientContainsUnit( int ingredientID, int unitID )
{
	QString command = QString( "SELECT *  FROM unit_list WHERE ingredient_id= %1 AND unit_id=%2;" ).arg( ingredientID ).arg( unitID );
	QSQLiteResult recipeToLoad = database->executeQuery( command );
	if ( !recipeToLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = recipeToLoad.first();
		return ( recipeToLoad.size() > 0 );
	}
	return false;
}

bool LiteRecipeDB::ingredientContainsProperty( int ingredientID, int propertyID, int perUnitsID )
{
	QString command = QString( "SELECT *  FROM ingredient_info WHERE ingredient_id=%1 AND property_id=%2 AND per_units=%3;" ).arg( ingredientID ).arg( propertyID ).arg( perUnitsID );
	QSQLiteResult recipeToLoad = database->executeQuery( command );

	if ( recipeToLoad.getStatus() != QSQLiteResult::Failure ) {
		return ( recipeToLoad.size() > 0 );
	}
	return false;
}

QString LiteRecipeDB::categoryName( int ID )
{
	QString command = QString( "SELECT name FROM categories WHERE id=%1;" ).arg( ID );
	QSQLiteResult toLoad = database->executeQuery( command );
	if ( toLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = toLoad.first();
		if ( !toLoad.atEnd() )  // Go to the first record (there should be only one anyway.
			return ( unescapeAndDecode( row.data( 0 ) ) );
	}
	return ( QString::null );
}

IngredientProperty LiteRecipeDB::propertyName( int ID )
{
	QString command = QString( "SELECT name,units FROM ingredient_properties WHERE id=%1;" ).arg( ID );
	QSQLiteResult toLoad = database->executeQuery( command );
	if ( toLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = toLoad.first();
		if ( !toLoad.atEnd() )  // Go to the first record (there should be only one anyway.
			return ( IngredientProperty( unescapeAndDecode( row.data( 0 ) ), unescapeAndDecode( row.data( 1 ) ), ID ) );
	}
	return ( IngredientProperty( QString::null, QString::null ) );
}


Unit LiteRecipeDB::unitName( int ID )
{
	QString command = QString( "SELECT name,plural FROM units WHERE id=%1;" ).arg( ID );
	QSQLiteResult toLoad = database->executeQuery( command );
	if ( toLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = toLoad.first();
		if ( !toLoad.atEnd() ) { // Go to the first record (there should be only one anyway.
			Unit unit( unescapeAndDecode( row.data( 0 ) ), unescapeAndDecode( row.data( 1 ) ) );

			//if we don't have both name and plural, use what we have as both
			if ( unit.name.isEmpty() )
				unit.name = unit.plural;
			else if ( unit.plural.isEmpty() )
				unit.plural = unit.name;

			return unit;
		}
	}
	return Unit();
}

int LiteRecipeDB::getCount( const QString &table_name )
{
	QSQLiteResult count = database->executeQuery( "SELECT COUNT(1) FROM "+table_name+";" );
	if ( count.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = count.first();
		if ( !count.atEnd() ) { // Go to the first record (there should be only one anyway.
			return row.data(0).toInt();
		}
	}

	return -1;
}

int LiteRecipeDB::categoryTopLevelCount()
{
	QSQLiteResult count = database->executeQuery( "SELECT COUNT(1) FROM categories WHERE parent_id='-1';" );
	if ( count.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = count.first();
		if ( !count.atEnd() ) { // Go to the first record (there should be only one anyway.
			return row.data(0).toInt();
		}
	}

	return -1;
}

bool LiteRecipeDB::checkIntegrity( void )
{


	// Check existence of the necessary tables (the database may be created, but empty)
	QStringList tables;
	tables << "ingredient_info" << "ingredient_list" << "ingredient_properties" << "ingredients" << "prep_methods" << "recipes" << "unit_list" << "units" << "units_conversion" << "categories" << "category_list" << "authors" << "author_list" << "db_info" << "ingredient_groups" << "yield_types" << "prep_method_list";

	QString command = QString( "SELECT name FROM sqlite_master WHERE type='table' UNION ALL SELECT name FROM sqlite_temp_master WHERE type='table';" ); // Get the table names (equivalent to MySQL's "SHOW TABLES;" Easy to remember, right? ;)

	QSQLiteResult tablesToCheck = database->executeQuery( command );
	QStringList existingTableList;

	// Get the existing list first

	if ( tablesToCheck.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = tablesToCheck.first();
		while ( !tablesToCheck.atEnd() ) {
			QString tableName = row.data( 0 );
			existingTableList << tableName;
			row = tablesToCheck.next();
		}
	}

	else
		return ( false );


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

	if ( int( qRound( databaseVersion() * 1e5 ) ) < int( qRound( latestDBVersion() * 1e5 ) ) ) { //correct for float's imprecision
		switch ( KMessageBox::questionYesNo( 0, i18n( "<!doc>The database was created with a previous version of Krecipes.  Would you like Krecipes to update this database to work with this version of Krecipes?  Depending on the number of recipes and amount of data, this could take some time.<br><br><b>Warning: After updating, this database will no longer be compatible with previous versions of Krecipes.<br><br>Cancelling this operation may result in corrupting the database.</b>" ) ) ) {
		case KMessageBox::Yes:
			portOldDatabases( version );
			break;
		case KMessageBox::No:
			return false;
		}
	}

	return true;
}

void LiteRecipeDB::createTable( const QString &tableName )
{

	QStringList commands;

	if ( tableName == "recipes" )
		commands << QString( "CREATE TABLE recipes (id INTEGER NOT NULL,title VARCHAR(%1), yield_amount FLOAT, yield_amount_offset FLOAT, yield_type_id INTEGER DEFAULT '-1', instructions TEXT, photo BLOB, prep_time TIME, ctime TIMESTAMP, mtime TIMESTAMP, atime TIMESTAMP,  PRIMARY KEY (id));" ).arg( maxRecipeTitleLength() );

	else if ( tableName == "ingredients" )
		commands << QString( "CREATE TABLE ingredients (id INTEGER NOT NULL, name VARCHAR(%1), PRIMARY KEY (id));" ).arg( maxIngredientNameLength() );

	else if ( tableName == "ingredient_list" ) {
		commands << "CREATE TABLE ingredient_list (id INTEGER NOT NULL, recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, amount_offset FLOAT, unit_id INTEGER, prep_method_id INTEGER, order_index INTEGER, group_id INTEGER, PRIMARY KEY(id) );"
		<< "CREATE index ridil_index ON ingredient_list(recipe_id);"
		<< "CREATE index iidil_index ON ingredient_list(ingredient_id);";
	}

	else if ( tableName == "unit_list" )
		commands << "CREATE TABLE unit_list (ingredient_id INTEGER, unit_id INTEGER);";

	else if ( tableName == "units" )
		commands << QString( "CREATE TABLE units (id INTEGER NOT NULL, name VARCHAR(%1), plural VARCHAR(%2), PRIMARY KEY (id));" ).arg( maxUnitNameLength() ).arg( maxUnitNameLength() );

	else if ( tableName == "prep_methods" )
		commands << QString( "CREATE TABLE prep_methods (id INTEGER NOT NULL, name VARCHAR(%1), PRIMARY KEY (id));" ).arg( maxPrepMethodNameLength() );

	else if ( tableName == "prep_method_list" ) {
		commands << "CREATE TABLE prep_method_list (ingredient_list_id INTEGER NOT NULL,prep_method_id INTEGER NOT NULL, order_index INTEGER );"
		<< "CREATE index iid_index ON prep_method_list(ingredient_list_id);"
		<< "CREATE index pid_index ON prep_method_list(prep_method_id);";
	}

	else if ( tableName == "ingredient_info" )
		commands << "CREATE TABLE ingredient_info (ingredient_id INTEGER, property_id INTEGER, amount FLOAT, per_units INTEGER);";

	else if ( tableName == "ingredient_properties" )
		commands << "CREATE TABLE ingredient_properties (id INTEGER NOT NULL,name VARCHAR(20), units VARCHAR(20), PRIMARY KEY (id));";

	else if ( tableName == "units_conversion" )
		commands << "CREATE TABLE units_conversion (unit1_id INTEGER, unit2_id INTEGER, ratio FLOAT);";

	else if ( tableName == "categories" )
		commands << QString( "CREATE TABLE categories (id INTEGER NOT NULL, name varchar(%1) default NULL, parent_id INGEGER NOT NULL default -1, PRIMARY KEY (id));" ).arg( maxCategoryNameLength() );

	else if ( tableName == "category_list" ) {
		commands << "CREATE TABLE category_list (recipe_id INTEGER NOT NULL,category_id INTEGER NOT NULL);"
		<< "CREATE index rid_index ON category_list(recipe_id);"
		<< "CREATE index cid_index ON category_list(category_id);";
	}

	else if ( tableName == "authors" )
		commands << QString( "CREATE TABLE authors (id INTEGER NOT NULL, name varchar(%1) default NULL,PRIMARY KEY (id));" ).arg( maxAuthorNameLength() );

	else if ( tableName == "author_list" )
		commands << "CREATE TABLE author_list (recipe_id INTEGER NOT NULL,author_id INTEGER NOT NULL);";

	else if ( tableName == "db_info" ) {
		commands << "CREATE TABLE db_info (ver FLOAT NOT NULL,generated_by varchar(200) default NULL);";
		commands << QString( "INSERT INTO db_info VALUES(%1,'Krecipes %2');" ).arg( latestDBVersion() ).arg( krecipes_version() );
	}
	else if ( tableName == "ingredient_groups" ) {
		commands << QString( "CREATE TABLE ingredient_groups (id INTEGER NOT NULL, name varchar(%1), PRIMARY KEY (id));" ).arg( maxIngGroupNameLength() );
	}
	else if ( tableName == "yield_types" ) {
		commands << QString( "CREATE TABLE yield_types (id INTEGER NOT NULL, name varchar(%1), PRIMARY KEY (id));" ).arg( maxYieldTypeLength() );
	}

	else
		return ;

	// execute the queries
	for ( QStringList::const_iterator it = commands.begin(); it != commands.end(); ++it )
		database->executeQuery( *it );

}

void LiteRecipeDB::splitCommands( QString& s, QStringList& sl )
{
	sl = QStringList::split( QRegExp( ";{1}(?!@)" ), s );
}

void LiteRecipeDB::portOldDatabases( float version )
{
	QString command;
	if ( qRound(version*10) < 5 ) {
		command = QString( "CREATE TABLE prep_methods (id INTEGER NOT NULL, name VARCHAR(%1), PRIMARY KEY (id));" ).arg( maxPrepMethodNameLength() );
		database->executeQuery( command );

		//===========add prep_method_id to ingredient_list table
		//There's no ALTER command in SQLite, so we have to copy all data to a new table and then recreate the table with the prep_method_id
		database->executeQuery( "CREATE TABLE ingredient_list_copy (recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, unit_id INTEGER, order_index INTEGER);" );
		QSQLiteResult copyQuery = database->executeQuery( "SELECT * FROM ingredient_list;" );
		if ( copyQuery.getStatus() != QSQLiteResult::Failure ) {
			QSQLiteResultRow row = copyQuery.first();
			while ( !copyQuery.atEnd() ) {
				command = QString( "INSERT INTO ingredient_list_copy VALUES(%1,%2,%3,%4,%5);" )
				          .arg( row.data( 0 ).toInt() )
				          .arg( row.data( 1 ).toInt() )
				          .arg( row.data( 2 ).toFloat() )
				          .arg( row.data( 3 ).toInt() )
				          .arg( row.data( 4 ).toInt() );
				database->executeQuery( command );

				row = copyQuery.next();
			}
		}
		database->executeQuery( "DROP TABLE ingredient_list" );
		database->executeQuery( "CREATE TABLE ingredient_list (recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, unit_id INTEGER, prep_method_id INTEGER, order_index INTEGER);" );
		copyQuery = database->executeQuery( "SELECT * FROM ingredient_list_copy" );
		if ( copyQuery.getStatus() != QSQLiteResult::Failure ) {
			QSQLiteResultRow row = copyQuery.first();
			while ( !copyQuery.atEnd() ) {
				command = QString( "INSERT INTO ingredient_list VALUES(%1,%2,%3,%4,%5,%6);" )
				          .arg( row.data( 0 ).toInt() )
				          .arg( row.data( 1 ).toInt() )
				          .arg( row.data( 2 ).toFloat() )
				          .arg( row.data( 3 ).toInt() )
				          .arg( -1 )  //default prep method
				          .arg( row.data( 4 ).toInt() );
				database->executeQuery( command );

				row = copyQuery.next();
			}
		}
		database->executeQuery( "DROP TABLE ingredient_list_copy" );

		database->executeQuery( "CREATE index ridil_index ON ingredient_list(recipe_id);" );
		database->executeQuery( "CREATE index iidil_index ON ingredient_list(ingredient_id);" );


		//==============expand length of author name to 50 characters
		database->executeQuery( "CREATE TABLE authors_copy (id INTEGER, name varchar(20));" );
		copyQuery = database->executeQuery( "SELECT * FROM authors;" );
		if ( copyQuery.getStatus() != QSQLiteResult::Failure ) {
			QSQLiteResultRow row = copyQuery.first();
			while ( !copyQuery.atEnd() ) {
				command = QString( "INSERT INTO authors_copy VALUES(%1,'%2');" )
				          .arg( row.data( 0 ).toInt() )
				          .arg( escape( row.data( 1 ) ) );
				database->executeQuery( command );

				row = copyQuery.next();
			}
		}
		database->executeQuery( "DROP TABLE authors" );
		database->executeQuery( "CREATE TABLE authors (id INTEGER NOT NULL, name varchar(50) default NULL,PRIMARY KEY (id));" );
		copyQuery = database->executeQuery( "SELECT * FROM authors_copy" );
		if ( copyQuery.getStatus() != QSQLiteResult::Failure ) {
			QSQLiteResultRow row = copyQuery.first();
			while ( !copyQuery.atEnd() ) {
				command = QString( "INSERT INTO authors VALUES(%1,'%2');" )
				          .arg( row.data( 0 ).toInt() )
				          .arg( escape( row.data( 1 ) ) );
				database->executeQuery( command );

				row = copyQuery.next();
			}
		}
		database->executeQuery( "DROP TABLE authors_copy" );


		//==================expand length of category name to 40 characters
		database->executeQuery( "CREATE TABLE categories_copy (id INTEGER, name varchar(20));" );
		copyQuery = database->executeQuery( "SELECT * FROM categories;" );
		if ( copyQuery.getStatus() != QSQLiteResult::Failure ) {
			QSQLiteResultRow row = copyQuery.first();
			while ( !copyQuery.atEnd() ) {
				command = QString( "INSERT INTO categories_copy VALUES(%1,'%2');" )
				          .arg( row.data( 0 ).toInt() )
				          .arg( escape( row.data( 1 ) ) );
				database->executeQuery( command );

				row = copyQuery.next();
			}
		}
		database->executeQuery( "DROP TABLE categories" );
		database->executeQuery( "CREATE TABLE categories (id INTEGER NOT NULL, name varchar(40) default NULL,PRIMARY KEY (id));" );
		copyQuery = database->executeQuery( "SELECT * FROM categories_copy" );
		if ( copyQuery.getStatus() != QSQLiteResult::Failure ) {
			QSQLiteResultRow row = copyQuery.first();
			while ( !copyQuery.atEnd() ) {
				command = QString( "INSERT INTO categories VALUES(%1,'%2');" )
				          .arg( row.data( 0 ).toInt() )
				          .arg( escape( row.data( 1 ) ) );
				database->executeQuery( command );

				row = copyQuery.next();
			}
		}
		database->executeQuery( "DROP TABLE categories_copy" );

		//================Set the version to the new one (0.5)
		command = "DELETE FROM db_info;"; // Remove previous version records if they exist
		database->executeQuery( command );
		command = "INSERT INTO db_info VALUES(0.5,'Krecipes 0.5');";
		database->executeQuery( command );
	}

	if ( qRound(version*10) < 6 ) {
		//==================add a column to 'categories' to allow subcategories
		database->executeQuery( "CREATE TABLE categories_copy (id INTEGER, name varchar(40));" );
		QSQLiteResult copyQuery = database->executeQuery( "SELECT * FROM categories;" );
		if ( copyQuery.getStatus() != QSQLiteResult::Failure ) {
			QSQLiteResultRow row = copyQuery.first();
			while ( !copyQuery.atEnd() ) {
				command = QString( "INSERT INTO categories_copy VALUES(%1,'%2');" )
				          .arg( row.data( 0 ).toInt() )
				          .arg( escape( row.data( 1 ) ) );
				database->executeQuery( command );

				row = copyQuery.next();
			}
		}
		database->executeQuery( "DROP TABLE categories" );
		database->executeQuery( "CREATE TABLE categories (id INTEGER NOT NULL, name varchar(40) default NULL, parent_id INTEGER NOT NULL, PRIMARY KEY (id));" );
		copyQuery = database->executeQuery( "SELECT * FROM categories_copy" );
		if ( copyQuery.getStatus() != QSQLiteResult::Failure ) {
			QSQLiteResultRow row = copyQuery.first();
			while ( !copyQuery.atEnd() ) {
				command = QString( "INSERT INTO categories VALUES(%1,'%2',-1);" )
				          .arg( row.data( 0 ).toInt() )
				          .arg( escape( row.data( 1 ) ) );
				database->executeQuery( command );

				row = copyQuery.next();
			}
		}
		database->executeQuery( "DROP TABLE categories_copy" );

		//================Set the version to the new one (0.6)
		command = "DELETE FROM db_info;"; // Remove previous version records if they exist
		database->executeQuery( command );
		command = "INSERT INTO db_info VALUES(0.6,'Krecipes 0.6');";
		database->executeQuery( command );
	}

	if ( qRound(version*100) < 61 ) {
		//==================add a column to 'recipes' to allow prep time
		database->executeQuery( "CREATE TABLE recipes_copy (id INTEGER NOT NULL,title VARCHAR(200),persons INTEGER,instructions TEXT, photo BLOB,   PRIMARY KEY (id));" );
		QSQLiteResult copyQuery = database->executeQuery( "SELECT * FROM recipes;" );
		if ( copyQuery.getStatus() != QSQLiteResult::Failure ) {
			QSQLiteResultRow row = copyQuery.first();
			while ( !copyQuery.atEnd() ) {
				command = QString( "INSERT INTO recipes_copy VALUES(%1,'%2','%3','%4','%5');" )
				          .arg( escape( row.data( 0 ) ) )
				          .arg( escape( row.data( 1 ) ) )
				          .arg( escape( row.data( 2 ) ) )
				          .arg( escape( row.data( 3 ) ) )
				          .arg( escape( row.data( 4 ) ) );
				database->executeQuery( command );

				row = copyQuery.next();
			}
		}
		database->executeQuery( "DROP TABLE recipes" );
		database->executeQuery( "CREATE TABLE recipes (id INTEGER NOT NULL,title VARCHAR(200),persons INTEGER,instructions TEXT, photo BLOB, prep_time TIME,   PRIMARY KEY (id));" );
		copyQuery = database->executeQuery( "SELECT * FROM recipes_copy" );
		if ( copyQuery.getStatus() != QSQLiteResult::Failure ) {
			QSQLiteResultRow row = copyQuery.first();
			while ( !copyQuery.atEnd() ) {
				command = QString( "INSERT INTO recipes VALUES(%1,'%2','%3','%4','%5',NULL);" )
				          .arg( escape( row.data( 0 ) ) )
				          .arg( escape( row.data( 1 ) ) )
				          .arg( escape( row.data( 2 ) ) )
				          .arg( escape( row.data( 3 ) ) )
				          .arg( escape( row.data( 4 ) ) );

				database->executeQuery( command );

				row = copyQuery.next();
			}
		}
		database->executeQuery( "DROP TABLE recipes_copy" );

		//================Set the version to the new one (0.61)
		command = "DELETE FROM db_info;"; // Remove previous version records if they exist
		database->executeQuery( command );
		command = "INSERT INTO db_info VALUES(0.61,'Krecipes 0.6');";
		database->executeQuery( command );
	}

	if ( qRound(version*100) < 62 ) {
		database->executeQuery( "BEGIN TRANSACTION;" );

		//==================add a column to 'ingredient_list' to allow grouping ingredients
		database->executeQuery( "CREATE TABLE ingredient_list_copy (recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, unit_id INTEGER, prep_method_id INTEGER, order_index INTEGER);" );
		QSQLiteResult copyQuery = database->executeQuery( "SELECT * FROM ingredient_list;" );
		if ( copyQuery.getStatus() != QSQLiteResult::Failure ) {
			QSQLiteResultRow row = copyQuery.first();
			while ( !copyQuery.atEnd() ) {
				command = "INSERT INTO ingredient_list_copy VALUES('" + escape( row.data( 0 ) )
				          + "','" + escape( row.data( 1 ) )
				          + "','" + escape( row.data( 2 ) )
				          + "','" + escape( row.data( 3 ) )
				          + "','" + escape( row.data( 4 ) )
				          + "','" + escape( row.data( 5 ) )
				          + "');";
				database->executeQuery( command );

				row = copyQuery.next();
			}
		}
		database->executeQuery( "DROP TABLE ingredient_list" );
		database->executeQuery( "CREATE TABLE ingredient_list (recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, unit_id INTEGER, prep_method_id INTEGER, order_index INTEGER, group_id INTEGER);" );
		copyQuery = database->executeQuery( "SELECT * FROM ingredient_list_copy" );
		if ( copyQuery.getStatus() != QSQLiteResult::Failure ) {
			QSQLiteResultRow row = copyQuery.first();
			while ( !copyQuery.atEnd() ) {
				command = "INSERT INTO ingredient_list VALUES('" + escape( row.data( 0 ) )
				          + "','" + escape( row.data( 1 ) )
				          + "','" + escape( row.data( 2 ) )
				          + "','" + escape( row.data( 3 ) )
				          + "','" + escape( row.data( 4 ) )
				          + "','" + escape( row.data( 5 ) )
				          + "',-1)";
				database->executeQuery( command );

				row = copyQuery.next();
			}
		}
		database->executeQuery( "DROP TABLE ingredient_list_copy" );

		command = "DELETE FROM db_info;"; // Remove previous version records if they exist
		database->executeQuery( command );
		command = "INSERT INTO db_info VALUES(0.62,'Krecipes 0.7');";
		database->executeQuery( command );

		database->executeQuery( "COMMIT TRANSACTION;" );
	}

	if ( qRound(version*100) < 63 ) {
		database->executeQuery( "BEGIN TRANSACTION;" );

		//==================add a column to 'units' to allow handling plurals
		database->executeQuery( "CREATE TABLE units_copy (id INTEGER NOT NULL, name VARCHAR(20), PRIMARY KEY (id));" );
		QSQLiteResult copyQuery = database->executeQuery( "SELECT id,name FROM units;" );
		if ( copyQuery.getStatus() != QSQLiteResult::Failure ) {
			QSQLiteResultRow row = copyQuery.first();
			while ( !copyQuery.atEnd() ) {
				command = "INSERT INTO units_copy VALUES('" + escape( row.data( 0 ) )
				          + "','" + escape( row.data( 1 ) )
				          + "');";
				database->executeQuery( command );

				row = copyQuery.next();
			}
		}
		database->executeQuery( "DROP TABLE units" );
		database->executeQuery( "CREATE TABLE units (id INTEGER NOT NULL, name VARCHAR(20), plural VARCHAR(20), PRIMARY KEY (id));" );
		copyQuery = database->executeQuery( "SELECT id,name FROM units_copy" );
		if ( copyQuery.getStatus() != QSQLiteResult::Failure ) {
			QSQLiteResultRow row = copyQuery.first();
			while ( !copyQuery.atEnd() ) {
				command = "INSERT INTO units VALUES('" + escape( row.data( 0 ) )
				          + "','" + escape( row.data( 1 ) )
				          + "',NULL)";
				database->executeQuery( command );

				row = copyQuery.next();
			}
		}
		database->executeQuery( "DROP TABLE units_copy" );

		QSQLiteResult result = database->executeQuery( "SELECT id,name FROM units WHERE plural ISNULL;" );
		if ( result.getStatus() != QSQLiteResult::Failure ) {
			QSQLiteResultRow row = result.first();
			while ( !result.atEnd() ) {
				command = "UPDATE units SET plural='" + escape( row.data( 1 ) ) + "' WHERE id=" + QString::number( row.data( 0 ).toInt() );
				database->executeQuery( command );

				row = result.next();
			}
		}

		command = "DELETE FROM db_info;"; // Remove previous version records if they exist
		database->executeQuery( command );
		command = "INSERT INTO db_info VALUES(0.63,'Krecipes 0.7');";
		database->executeQuery( command );

		database->executeQuery( "COMMIT TRANSACTION;" );
	}

	if ( qRound(version*10) < 7 ) { //simply call 0.63 -> 0.7
		database->executeQuery( "UPDATE db_info SET ver='0.7';" );
	}

	if ( qRound(version*100) < 81 ) {
		database->executeQuery( "BEGIN TRANSACTION;" );
		addColumn("CREATE TABLE %1 (recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, %2 unit_id INTEGER, prep_method_id INTEGER, order_index INTEGER, group_id INTEGER)","amount_offset FLOAT","'0'","ingredient_list",3);

		//addColumn() doesn't preserve indexes
		database->executeQuery("CREATE index ridil_index ON ingredient_list(recipe_id)");
		database->executeQuery("CREATE index iidil_index ON ingredient_list(ingredient_id)");

		database->executeQuery( "UPDATE db_info SET ver='0.81',generated_by='Krecipes SVN (20050816)';" );
		database->executeQuery( "COMMIT TRANSACTION;" );
	}

	if ( qRound(version*100) < 82 ) {
		database->executeQuery( "BEGIN TRANSACTION;" );

		//==================add a columns to 'recipes' to allow yield range + yield type
		database->executeQuery( "CREATE TABLE recipes_copy (id INTEGER NOT NULL,title VARCHAR(200),persons INTEGER,instructions TEXT, photo BLOB, prep_time TIME, PRIMARY KEY (id));" );
		QSQLiteResult copyQuery = database->executeQuery( "SELECT id,title,persons,instructions,photo,prep_time FROM recipes;" );
		if ( copyQuery.getStatus() != QSQLiteResult::Failure ) {
			QSQLiteResultRow row = copyQuery.first();
			while ( !copyQuery.atEnd() ) {
				command = "INSERT INTO recipes_copy VALUES('"
				                  + escape( row.data( 0 ) ) //id
				          + "','" + escape( row.data( 1 ) ) //title
				          + "','" + escape( row.data( 2 ) ) //persons
				          + "','" + escape( row.data( 3 ) ) //instructions
				          + "','" + escape( row.data( 4 ) ) //photo
				          + "','" + escape( row.data( 5 ) ) //prep_time
				          + "')";
				database->executeQuery( command );

				row = copyQuery.next();
			}
		}
		database->executeQuery( "DROP TABLE recipes" );
		database->executeQuery( "CREATE TABLE recipes (id INTEGER NOT NULL,title VARCHAR(200), yield_amount FLOAT, yield_amount_offset FLOAT, yield_type_id INTEGER, instructions TEXT, photo BLOB, prep_time TIME, PRIMARY KEY (id));" );
		copyQuery = database->executeQuery( "SELECT id,title,persons,instructions,photo,prep_time FROM recipes_copy" );
		if ( copyQuery.getStatus() != QSQLiteResult::Failure ) {
			QSQLiteResultRow row = copyQuery.first();
			while ( !copyQuery.atEnd() ) {
				command = "INSERT INTO recipes VALUES('" 
				                  + escape( row.data( 0 ) ) //id
				          + "','" + escape( row.data( 1 ) ) //title
				          + "','" + escape( row.data( 2 ) ) //persons, now yield_amount
				          + "','0"                          //yield_amount_offset
				          + "','-1"                         //yield_type_id
				          + "','" + escape( row.data( 3 ) ) //instructions
				          + "','" + escape( row.data( 4 ) ) //photo
				          + "','" + escape( row.data( 5 ) ) //prep_time
				          + "')";
				database->executeQuery( command );

				row = copyQuery.next();
			}
		}
		database->executeQuery( "DROP TABLE recipes_copy" );

		database->executeQuery( "UPDATE db_info SET ver='0.82',generated_by='Krecipes SVN (20050902)';" );
		database->executeQuery( "COMMIT TRANSACTION;" );
	}

	if ( qRound(version*100) < 83 ) {
		database->executeQuery( "BEGIN TRANSACTION;" );

		//====add a id columns to 'ingredient_list' to identify it for the prep method list
		database->executeQuery( "CREATE TABLE ingredient_list_copy (recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, amount_offset FLOAT, unit_id INTEGER, prep_method_id INTEGER, order_index INTEGER, group_id INTEGER);" );
		QSQLiteResult copyQuery = database->executeQuery( "SELECT recipe_id,ingredient_id,amount,amount_offset,unit_id,prep_method_id,order_index,group_id FROM ingredient_list" );
		if ( copyQuery.getStatus() != QSQLiteResult::Failure ) {
			QSQLiteResultRow row = copyQuery.first();
			while ( !copyQuery.atEnd() ) {
				command = "INSERT INTO ingredient_list_copy VALUES('" 
				                  + escape( row.data( 0 ) )
				          + "','" + escape( row.data( 1 ) )
				          + "','" + escape( row.data( 2 ) )
				          + "','" + escape( row.data( 3 ) )
				          + "','" + escape( row.data( 4 ) )
				          + "','" + escape( row.data( 5 ) )
				          + "','" + escape( row.data( 6 ) )
				          + "','" + escape( row.data( 7 ) )
				          + "')";
				database->executeQuery( command );
				row = copyQuery.next();
			}
		}
		database->executeQuery( "DROP TABLE ingredient_list" );
		database->executeQuery( "CREATE TABLE ingredient_list (id INTEGER NOT NULL, recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, amount_offset FLOAT, unit_id INTEGER, order_index INTEGER, group_id INTEGER, PRIMARY KEY(id) );" );

		copyQuery = database->executeQuery( "SELECT recipe_id,ingredient_id,amount,amount_offset,unit_id,prep_method_id,order_index,group_id FROM ingredient_list_copy" );
		if ( copyQuery.getStatus() != QSQLiteResult::Failure ) {
			QSQLiteResultRow row = copyQuery.first();
			while ( !copyQuery.atEnd() ) {
				command = "INSERT INTO ingredient_list VALUES(" 
				                  + QString("NULL")
				          + ",'" + escape( row.data( 0 ) )
				          + "','" + escape( row.data( 1 ) )
				          + "','" + escape( row.data( 2 ) )
				          + "','" + escape( row.data( 3 ) )
				          + "','" + escape( row.data( 4 ) )
				          + "','" + escape( row.data( 6 ) )
				          + "','" + escape( row.data( 7 ) )
				          + "')";
				database->executeQuery( command );

				int prep_method_id = row.data( 5 ).toInt();
				if ( prep_method_id != -1 ) {
					command = "INSERT INTO prep_method_list VALUES('" 
							+ QString::number(lastInsertID())
						+ "','" + QString::number(prep_method_id)
						+ "','1" //order_index
						+ "')";
					database->executeQuery( command );
				}
				row = copyQuery.next();
			}
		}
		database->executeQuery( "DROP TABLE ingredient_list_copy" );

		database->executeQuery( "CREATE INDEX ridil_index ON ingredient_list(recipe_id);" );
		database->executeQuery( "CREATE INDEX iidil_index ON ingredient_list(ingredient_id);" );

		database->executeQuery( "UPDATE db_info SET ver='0.83',generated_by='Krecipes SVN (20050909)';" );

		database->executeQuery( "COMMIT TRANSACTION;" );
	}

	if ( qRound(version*100) < 84 ) {
		database->executeQuery( "BEGIN TRANSACTION;" );

		//==================add a columns to 'recipes' to allow storing atime, mtime, ctime
		database->executeQuery( "CREATE TABLE recipes_copy (id INTEGER NOT NULL,title VARCHAR(200), yield_amount FLOAT, yield_amount_offset FLOAT, yield_type_id INTEGER, instructions TEXT, photo BLOB, prep_time TIME, PRIMARY KEY (id));" );
		QSQLiteResult copyQuery = database->executeQuery( "SELECT id,title,yield_amount,yield_amount_offset,yield_type_id,instructions,photo,prep_time FROM recipes;" );
		if ( copyQuery.getStatus() != QSQLiteResult::Failure ) {
			QSQLiteResultRow row = copyQuery.first();
			while ( !copyQuery.atEnd() ) {
				command = "INSERT INTO recipes_copy VALUES('"
				                  + escape( row.data( 0 ) )
				          + "','" + escape( row.data( 1 ) )
				          + "','" + escape( row.data( 2 ) )
				          + "','" + escape( row.data( 3 ) )
				          + "','" + escape( row.data( 4 ) )
				          + "','" + escape( row.data( 5 ) )
				          + "','" + escape( row.data( 6 ) )
				          + "','" + escape( row.data( 7 ) )
				          + "')";
				database->executeQuery( command );

				row = copyQuery.next();
			}
		}
		database->executeQuery( "DROP TABLE recipes" );
		database->executeQuery( "CREATE TABLE recipes (id INTEGER NOT NULL,title VARCHAR(200), yield_amount FLOAT, yield_amount_offset FLOAT, yield_type_id INTEGER DEFAULT '-1', instructions TEXT, photo BLOB, prep_time TIME, ctime TIMESTAMP, mtime TIMESTAMP, atime TIMESTAMP,  PRIMARY KEY (id))" );
		copyQuery = database->executeQuery( "SELECT id,title,yield_amount,yield_amount_offset,yield_type_id,instructions,photo,prep_time FROM recipes_copy" );
		if ( copyQuery.getStatus() != QSQLiteResult::Failure ) {
			QSQLiteResultRow row = copyQuery.first();
			QString current_timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
			while ( !copyQuery.atEnd() ) {
				command = "INSERT INTO recipes VALUES('" 
				                  + escape( row.data( 0 ) )
				          + "','" + escape( row.data( 1 ) )
				          + "','" + escape( row.data( 2 ) )
				          + "','" + escape( row.data( 3 ) )
				          + "','" + escape( row.data( 4 ) )
				          + "','" + escape( row.data( 5 ) )
				          + "','" + escape( row.data( 6 ) )
				          + "','" + escape( row.data( 7 ) )
				          + "','" + escape( current_timestamp ) //ctime
				          + "','" + escape( current_timestamp ) //mtime
				          + "','" + escape( current_timestamp ) //atime
				          + "')";
				database->executeQuery( command );

				row = copyQuery.next();
			}
		}
		database->executeQuery( "DROP TABLE recipes_copy" );

		database->executeQuery( "UPDATE db_info SET ver='0.84',generated_by='Krecipes SVN (20050913)';" );
		database->executeQuery( "COMMIT TRANSACTION;" );
	}
}

void LiteRecipeDB::addColumn( const QString &new_table_sql, const QString &new_col_info, const QString &default_value, const QString &table_name, int col_index )
{
	QString command;

	command = QString(new_table_sql).arg(table_name+"_copy").arg(QString::null);
	kdDebug()<<"calling: "<<command<<endl;
	database->executeQuery( command );

	command = "SELECT * FROM "+table_name;
	kdDebug()<<"calling: "<<command<<endl;
	QSQLiteResult copyQuery = database->executeQuery( command );
	if ( copyQuery.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = copyQuery.first();
		while ( !copyQuery.atEnd() ) {
			QStringList dataList;
			bool ok = true;
			for ( int i = 0 ;; ++i ) {
				QString data = row.data(i,&ok);
				if ( !ok ) break;

				dataList << "'"+escape(data)+"'";
			}
			command = "INSERT INTO "+table_name+"_copy VALUES("+dataList.join(",")+");";
			kdDebug()<<"calling: "<<command<<endl;
			database->executeQuery( command );

			row = copyQuery.next();
		}
	}
	database->executeQuery( "DROP TABLE "+table_name );
	database->executeQuery( QString(new_table_sql).arg(table_name).arg(new_col_info+",") );
	copyQuery = database->executeQuery( "SELECT * FROM "+table_name+"_copy" );
	if ( copyQuery.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = copyQuery.first();
		while ( !copyQuery.atEnd() ) {
			QStringList dataList;
			bool ok = true; 
			for ( int i = 0 ;; ++i ) {
				if ( i == col_index )
					dataList << default_value;

				QString data = row.data(i,&ok);
				if ( !ok ) break;

				dataList << "'"+escape(data)+"'";
			}
			command = "INSERT INTO "+table_name+" VALUES(" +dataList.join(",")+")";
			kdDebug()<<"calling: "<<command<<endl;
			database->executeQuery( command );

			row = copyQuery.next();
		}
	}
	database->executeQuery( "DROP TABLE "+table_name+"_copy" );
}

float LiteRecipeDB::databaseVersion( void )
{

	QString command = "SELECT ver FROM db_info;";
	QSQLiteResult dbVersion = database->executeQuery( command );

	if ( dbVersion.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = dbVersion.first();
		if ( !dbVersion.atEnd() )
			return ( row.data( 0 ).toDouble() ); // There should be only one (or none for old DB) element, so go to first
		else
			return ( 0.4 ); // if table is empty, assume oldest (0.4), and port
	}
	else
		return ( 0.4 ); // By default go for oldest (0.4)
}

void LiteRecipeDB::loadCategories( CategoryTree *list, int limit, int offset, int parent_id, bool recurse )
{
	QString limit_str;
	if ( parent_id == -1 ) {
		list->clear();

		emit progressBegin(0,QString::null,i18n("Loading category list"));

		//only limit the number of top-level categories
		limit_str = (limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset);
	}

	QString command = "SELECT id,name,parent_id FROM categories WHERE parent_id='"+QString::number(parent_id)+"' ORDER BY name"+limit_str+";";
	QSQLiteResult categoryToLoad = database->executeQuery( command );
	if ( categoryToLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = categoryToLoad.first();
		while ( !categoryToLoad.atEnd() ) {
			emit progress();

			int id = row.data( 0 ).toInt();
			Element el;
			el.id = id;
			el.name = unescapeAndDecode( row.data( 1 ) );
			CategoryTree *list_child = list->add( el );

			if ( recurse )
				loadCategories( list_child, -1, -1, id ); //limit and offset won't be used

			row = categoryToLoad.next();
		}
	}

	if ( parent_id == -1 )
		emit progressDone();
}

void LiteRecipeDB::loadCategories( ElementList *list, int limit, int offset )
{
	list->clear();
	QString command = "SELECT id,name FROM categories ORDER BY name"
	  +((limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset));
	QSQLiteResult categoryToLoad = database->executeQuery( command );
	if ( categoryToLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = categoryToLoad.first();
		while ( !categoryToLoad.atEnd() ) {
			Element el;
			el.id = row.data( 0 ).toInt();
			el.name = unescapeAndDecode( row.data( 1 ) );
			list->append( el );
			row = categoryToLoad.next();
		}
	}
}

void LiteRecipeDB::createNewCategory( const QString &categoryName, int parent_id )
{
	QString command;
	QString real_name = categoryName.left( maxCategoryNameLength() );

	command = QString( "INSERT INTO categories VALUES(NULL,'%1',%2);" ).arg( escapeAndEncode( real_name ) ).arg( parent_id );
	database->executeQuery( command );

	emit categoryCreated( Element( real_name, lastInsertID() ), parent_id );
}

void LiteRecipeDB::modCategory( int categoryID, const QString &newLabel )
{
	QString command;
	command = QString( "UPDATE categories SET name='%1' WHERE id=%2;" ).arg( escapeAndEncode( newLabel ) ).arg( categoryID );
	database->executeQuery( command );

	emit categoryModified( Element( newLabel, categoryID ) );
}

void LiteRecipeDB::modCategory( int categoryID, int new_parent_id )
{
	QString command;
	command = QString( "UPDATE categories SET parent_id=%1 WHERE id=%2;" ).arg( new_parent_id ).arg( categoryID );
	database->executeQuery( command );

	emit categoryModified( categoryID, new_parent_id );
}

void LiteRecipeDB::removeCategory( int categoryID )
{
	QString command;

	command = QString( "DELETE FROM categories WHERE id=%1;" ).arg( categoryID );
	database->executeQuery( command );

	command = QString( "DELETE FROM category_list WHERE category_id=%1;" ).arg( categoryID );
	database->executeQuery( command );

	//recursively delete subcategories
	command = QString( "SELECT id FROM categories WHERE parent_id=%1;" ).arg( categoryID );
	QSQLiteResult categoryToRemove = database->executeQuery( command );
	if ( categoryToRemove.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = categoryToRemove.first();
		while ( !categoryToRemove.atEnd() ) {
			removeCategory( row.data( 0 ).toInt() );
			row = categoryToRemove.next();
		}
	}

	emit categoryRemoved( categoryID );
}


void LiteRecipeDB::loadAuthors( ElementList *list, int limit, int offset )
{
	list->clear();
	QString command = "SELECT id,name FROM authors ORDER BY name"
	  +((limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset));
	QSQLiteResult authorToLoad = database->executeQuery( command );
	if ( authorToLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = authorToLoad.first();
		while ( !authorToLoad.atEnd() ) {
			Element el;
			el.id = row.data( 0 ).toInt();
			el.name = unescapeAndDecode( row.data( 1 ) );
			list->append( el );
			row = authorToLoad.next();
		}
	}
}

void LiteRecipeDB::createNewAuthor( const QString &authorName )
{
	QString command;
	QString real_name = authorName.left( maxAuthorNameLength() );

	command = QString( "INSERT INTO authors VALUES(NULL,'%1');" ).arg( escapeAndEncode( real_name ) );
	database->executeQuery( command );

	emit authorCreated( Element( real_name, lastInsertID() ) );
}

void LiteRecipeDB::modAuthor( int authorID, const QString &newLabel )
{
	QString command;

	command = QString( "UPDATE authors SET name='%1' WHERE id=%2;" ).arg( escapeAndEncode( newLabel ) ).arg( authorID );
	database->executeQuery( command );

	emit authorRemoved( authorID );
	emit authorCreated( Element( newLabel, authorID ) );
}

void LiteRecipeDB::removeAuthor( int authorID )
{
	QString command;

	command = QString( "DELETE FROM authors WHERE id=%1;" ).arg( authorID );
	database->executeQuery( command );

	emit authorRemoved( authorID );
}

int LiteRecipeDB::findExistingUnitsByName( const QString& name, int ingredientID, ElementList *list )
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

	QSQLiteResult unitsToLoad = database->executeQuery( command ); // Run the query

	if ( list )  //If the pointer exists, then load all the values found into it
	{
		if ( unitsToLoad.getStatus() != QSQLiteResult::Failure )
		{
			QSQLiteResultRow row = unitsToLoad.first();
			while ( !unitsToLoad.atEnd() ) {
				Element el;
				el.id = row.data( 0 ).toInt();
				el.name = unescapeAndDecode( row.data( 1 ) );
				list->append( el );
				row = unitsToLoad.next();
			}
		}
	}

	return ( unitsToLoad.size() );
}

int LiteRecipeDB::findExistingAuthorByName( const QString& name )
{
	QCString search_str = escapeAndEncode( name.left( maxAuthorNameLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM authors WHERE name LIKE '%1';" ).arg( search_str );
	QSQLiteResult elementToLoad = database->executeQuery( command ); // Run the query
	int id = -1;

	if ( elementToLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = elementToLoad.first();
		if ( !elementToLoad.atEnd() )
			id = row.data( 0 ).toInt();
	}

	return id;
}

int LiteRecipeDB::findExistingCategoryByName( const QString& name )
{
	QCString search_str = escapeAndEncode( name.left( maxCategoryNameLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM categories WHERE name LIKE '%1';" ).arg( search_str );
	QSQLiteResult elementToLoad = database->executeQuery( command ); // Run the query
	int id = -1;

	if ( elementToLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = elementToLoad.first();
		if ( !elementToLoad.atEnd() )
			id = row.data( 0 ).toInt();
	}

	return id;
}

int LiteRecipeDB::findExistingIngredientByName( const QString& name )
{
	QCString search_str = escapeAndEncode( name.left( maxIngredientNameLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM ingredients WHERE name LIKE '%1';" ).arg( search_str );
	QSQLiteResult elementToLoad = database->executeQuery( command ); // Run the query
	int id = -1;

	if ( elementToLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = elementToLoad.first();
		if ( !elementToLoad.atEnd() )
			id = row.data( 0 ).toInt();
	}

	return id;
}

int LiteRecipeDB::findExistingPrepByName( const QString& name )
{
	QCString search_str = escapeAndEncode( name.left( maxPrepMethodNameLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM prep_methods WHERE name LIKE '%1';" ).arg( search_str );
	QSQLiteResult elementToLoad = database->executeQuery( command ); // Run the query
	int id = -1;

	if ( elementToLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = elementToLoad.first();
		if ( !elementToLoad.atEnd() )
			id = row.data( 0 ).toInt();
	}

	return id;
}

int LiteRecipeDB::findExistingPropertyByName( const QString& name )
{
	QCString search_str = escapeAndEncode( name.left( maxPropertyNameLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM ingredient_properties WHERE name LIKE '%1';" ).arg( search_str );
	QSQLiteResult elementToLoad = database->executeQuery( command ); // Run the query
	int id = -1;

	if ( elementToLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = elementToLoad.first();
		if ( !elementToLoad.atEnd() )
			id = row.data( 0 ).toInt();
	}

	return id;
}

int LiteRecipeDB::findExistingUnitByName( const QString& name )
{
	QCString search_str = escapeAndEncode( name.left( maxUnitNameLength() ) ); //truncate to the maximum size db holds

	QString command = "SELECT id FROM units WHERE name LIKE '" + search_str + "' OR plural LIKE '" + search_str + "';";
	QSQLiteResult elementToLoad = database->executeQuery( command ); // Run the query
	int id = -1;

	if ( elementToLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = elementToLoad.first();
		if ( !elementToLoad.atEnd() )
			id = row.data( 0 ).toInt();
	}

	return id;
}

int LiteRecipeDB::findExistingRecipeByName( const QString& name )
{
	QCString search_str = escapeAndEncode( name.left( maxRecipeTitleLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM recipes WHERE title LIKE '%1';" ).arg( search_str );
	QSQLiteResult elementToLoad = database->executeQuery( command ); // Run the query
	int id = -1;

	if ( elementToLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = elementToLoad.first();
		if ( !elementToLoad.atEnd() )
			id = row.data( 0 ).toInt();
	}
	return id;
}

int LiteRecipeDB::findExistingYieldTypeByName( const QString& name )
{
	QCString search_str = escapeAndEncode( name.left( maxYieldTypeLength() ) ); //truncate to the maximum size db holds

	QString command = "SELECT id FROM yield_types WHERE name LIKE '" + search_str + "';";
	QSQLiteResult elementToLoad = database->executeQuery( command ); // Run the query
	int id = -1;

	if ( elementToLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = elementToLoad.first();
		if ( !elementToLoad.atEnd() )
			id = row.data( 0 ).toInt();
	}

	return id;
}

void LiteRecipeDB::mergeAuthors( int id1, int id2 )
{
	//change all instances of 'id2' to 'id1'
	QString command = QString( "UPDATE author_list SET author_id=%1 WHERE author_id=%2" )
	                  .arg( id1 )
	                  .arg( id2 );
	database->executeQuery( command );

	//and ensure no duplicates were created in this process
	command = QString( "SELECT recipe_id FROM author_list WHERE author_id=%1 ORDER BY recipe_id" )
	          .arg( id1 );
	QSQLiteResult unit_list = database->executeQuery( command );
	int last_id = -1;
	if ( unit_list.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = unit_list.first();
		while ( !unit_list.atEnd() ) {
			int current_id = row.data( 0 ).toInt();
			if ( last_id == current_id ) {
				command = QString( "DELETE FROM author_list WHERE author_id=%1 AND recipe_id=%2" )
				          .arg( id1 )
				          .arg( last_id );
				database->executeQuery( command );
				database->executeQuery( QString( "INSERT INTO author_list VALUES(%1,%2)" ).arg( last_id ).arg( id1 ) );
				row = unit_list.next();
			}
			last_id = current_id;
		}
	}

	//remove author with id 'id2'
	command = QString( "DELETE FROM authors WHERE id=%1;" ).arg( id2 );
	database->executeQuery( command );
	emit authorRemoved( id2 );
}

void LiteRecipeDB::mergeCategories( int id1, int id2 )
{
	//change all instances of 'id2' to 'id1'
	QString command = QString( "UPDATE category_list SET category_id=%1 WHERE category_id=%2" )
	                  .arg( id1 )
	                  .arg( id2 );
	database->executeQuery( command );

	//and ensure no duplicates were created in this process
	command = QString( "SELECT recipe_id FROM category_list WHERE category_id=%1 ORDER BY recipe_id" )
	          .arg( id1 );
	QSQLiteResult unit_list = database->executeQuery( command );
	int last_id = -1;
	if ( unit_list.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = unit_list.first();
		while ( !unit_list.atEnd() ) {
			int current_id = row.data( 0 ).toInt();
			if ( last_id == current_id ) {
				command = QString( "DELETE FROM category_list WHERE category_id=%1 AND recipe_id=%2" )
				          .arg( id1 )
				          .arg( last_id );
				database->executeQuery( command );
				database->executeQuery( QString( "INSERT INTO category_list VALUES(%1,%2)" ).arg( last_id ).arg( id1 ) );
				row = unit_list.next();
			}
			last_id = current_id;
		}
	}

	command = QString( "UPDATE categories SET parent_id=%1 WHERE parent_id=%2" )
	          .arg( id1 )
	          .arg( id2 );
	database->executeQuery( command );

	//we don't want to have a category be its own parent...
	command = QString( "UPDATE categories SET parent_id=-1 WHERE parent_id=id" );
	database->executeQuery( command );

	//remove category with id 'id2'
	command = QString( "DELETE FROM categories WHERE id=%1;" ).arg( id2 );
	database->executeQuery( command );

	emit categoriesMerged( id1, id2 );
}

void LiteRecipeDB::mergeIngredients( int id1, int id2 )
{
	//change all instances of 'id2' to 'id1'
	QString command = QString( "UPDATE ingredient_list SET ingredient_id=%1 WHERE ingredient_id=%2" )
	                  .arg( id1 )
	                  .arg( id2 );
	database->executeQuery( command );

	//delete nutrient info associated with ingredient with id 'id2'
	command = QString( "DELETE FROM ingredient_info WHERE ingredient_id=%1" )
	          .arg( id2 );
	database->executeQuery( command );

	//update the unit_list
	command = QString( "UPDATE unit_list SET ingredient_id=%1 WHERE ingredient_id=%2" )
	          .arg( id1 )
	          .arg( id2 );
	database->executeQuery( command );

	//and ensure no duplicates were created in this process
	command = QString( "SELECT unit_id FROM unit_list WHERE ingredient_id=%1 ORDER BY unit_id" )
	          .arg( id1 );
	QSQLiteResult unit_list = database->executeQuery( command );
	int last_id = -1;
	if ( unit_list.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = unit_list.first();
		while ( !unit_list.atEnd() ) {
			int current_id = row.data( 0 ).toInt();
			if ( last_id == current_id ) {
				command = QString( "DELETE FROM unit_list WHERE ingredient_id=%1 AND unit_id=%2" )
				          .arg( id1 )
				          .arg( last_id );
				database->executeQuery( command );
				database->executeQuery( QString( "INSERT INTO unit_list VALUES(%1,%2)" ).arg( id1 ).arg( last_id ) );
				row = unit_list.next();
			}
			last_id = current_id;
		}
	}

	//update the ingredient_info
	command = QString( "UPDATE ingredient_info SET ingredient_id=%1 WHERE ingredient_id=%2" )
	          .arg( id1 )
	          .arg( id2 );
	database->executeQuery( command );

	//ensure no duplicates were created in this process
	command = QString( "SELECT * FROM ingredient_info WHERE ingredient_id=%1 ORDER BY property_id" )
	          .arg( id1 );
	unit_list = database->executeQuery( command );
	last_id = -1;
	if ( unit_list.getStatus() != QSQLiteResult::Failure ) {
		double stored_amount = -1;
		int stored_unit_id = -1;
		QSQLiteResultRow row = unit_list.first();
		while ( !unit_list.atEnd() ) {
			int current_id = row.data( 1 ).toInt();
			double current_amount = row.data( 2 ).toDouble();
			int current_unit_id = row.data( 3 ).toInt();
			if ( last_id == current_id ) {
				command = QString( "DELETE FROM ingredient_info WHERE ingredient_id=%1 AND property_id=%2" )  //delete them all (doesn't support "LIMIT")
				          .arg( id1 )
				          .arg( last_id );
				database->executeQuery( command );
				command = QString( "INSERT INTO ingredient_info VALUES(%1,%2,%3,%4)" )  //put back one of what we deleted
				          .arg( id1 )
				          .arg( last_id )
				          .arg( stored_amount )
				          .arg( stored_unit_id );
				database->executeQuery( command );
				row = unit_list.next();
			}
			last_id = current_id;
			stored_amount = current_amount;
			stored_unit_id = current_unit_id;
		}
	}

	//remove ingredient with id 'id2'
	command = QString( "DELETE FROM ingredients WHERE id=%1;" ).arg( id2 );
	database->executeQuery( command );
	emit ingredientRemoved( id2 );
}

void LiteRecipeDB::mergePrepMethods( int id1, int id2 )
{
	//change all instances of 'id2' to 'id1' in ingredient list
	QString command = QString( "UPDATE ingredient_list SET prep_method_id=%1 WHERE prep_method_id=%2" )
	                  .arg( id1 )
	                  .arg( id2 );
	database->executeQuery( command );

	//remove prep method with id 'id2'
	command = QString( "DELETE FROM prep_methods WHERE id=%1;" ).arg( id2 );
	database->executeQuery( command );
	emit prepMethodRemoved( id2 );
}

void LiteRecipeDB::mergeProperties( int id1, int id2 )
{
	//change all instances of 'id2' to 'id1'
	QString command = QString( "UPDATE ingredient_properties SET id=%1 WHERE id=%2" )
	                  .arg( id1 )
	                  .arg( id2 );
	database->executeQuery( command );

	command = QString( "UPDATE ingredient_info SET property_id=%1 WHERE property_id=%2" )
	          .arg( id1 )
	          .arg( id2 );
	database->executeQuery( command );

	//remove prep method with id 'id2'
	command = QString( "DELETE FROM ingredient_properties WHERE id=%1" ).arg( id2 );
	database->executeQuery( command );
	emit propertyRemoved( id2 );
}

void LiteRecipeDB::mergeUnits( int id1, int id2 )
{
	//change all instances of 'id2' to 'id1' in ingredient list
	QString command = QString( "UPDATE unit_list SET unit_id=%1 WHERE unit_id=%2" )
	                  .arg( id1 )
	                  .arg( id2 );
	database->executeQuery( command );

	//change all instances of 'id2' to 'id1' in ingredient list
	command = QString( "UPDATE ingredient_list SET unit_id=%1 WHERE unit_id=%2" )
	          .arg( id1 )
	          .arg( id2 );
	database->executeQuery( command );

	//and ensure no duplicates were created in this process
	command = QString( "SELECT ingredient_id FROM unit_list WHERE unit_id=%1 ORDER BY ingredient_id" )
	          .arg( id1 );
	QSQLiteResult unit_list = database->executeQuery( command );
	int last_id = -1;
	if ( unit_list.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = unit_list.first();
		while ( !unit_list.atEnd() ) {
			int current_id = row.data( 0 ).toInt();
			if ( last_id == current_id ) {
				command = QString( "DELETE FROM unit_list WHERE unit_id=%1 AND ingredient_id=%2" )
				          .arg( id1 )
				          .arg( last_id );
				database->executeQuery( command );
				database->executeQuery( QString( "INSERT INTO unit_list VALUES(%1,%2)" ).arg( last_id ).arg( id1 ) );
				row = unit_list.next();
			}
			last_id = current_id;
		}
	}

	//change all instances of 'id2' to 'id1' in ingredient_info
	command = QString( "UPDATE ingredient_info SET per_units=%1 WHERE per_units=%2" )
	          .arg( id1 )
	          .arg( id2 );
	database->executeQuery( command );

	//change all instances of 'id2' to 'id1' in unit_conversion
	command = QString( "UPDATE units_conversion SET unit1_id=%1 WHERE unit1_id=%2" )
	          .arg( id1 )
	          .arg( id2 );
	database->executeQuery( command );
	command = QString( "UPDATE units_conversion SET unit2_id=%1 WHERE unit2_id=%2" )
	          .arg( id1 )
	          .arg( id2 );
	database->executeQuery( command );

	//and ensure that the one to one ratio wasn't created
	command = QString( "DELETE FROM units_conversion WHERE unit1_id=unit2_id" );
	database->executeQuery( command );

	//remove units with id 'id2'
	command = QString( "DELETE FROM units WHERE id=%1;" ).arg( id2 );
	database->executeQuery( command );
	emit unitRemoved( id2 );
}

void LiteRecipeDB::givePermissions( const QString & /*dbName*/, const QString & /*username*/, const QString & /*password*/, const QString & /*clientHost*/ )
{
	// FIXME: you can't give permissions in SQLite :)
	/*QString command;
	 
	if ((!password.isEmpty())&&(!password.isNull())) command=QString("GRANT ALL ON %1.* TO %2@%3 IDENTIFIED BY '%4';").arg(dbName).arg(username).arg(clientHost).arg(password);
	else command=QString("GRANT ALL ON %1.* TO %2@%3;").arg(dbName).arg(username).arg(clientHost);
	 
	std::cerr<<"I'm doing the query to setup permissions\n";
	 
	QSqlQuery permissionsToSet( command,database);*/
}
QString LiteRecipeDB::getUniqueRecipeTitle( const QString &recipe_title )
{
	//already is unique
	if ( findExistingRecipeByName( recipe_title ) == -1 )
		return recipe_title;

	QString return_title = recipe_title; //If any error is produced, just go for default value (always return something)

	QString command = QString( "SELECT COUNT(title) FROM recipes WHERE title LIKE '%1 (%)';" ).arg( escapeAndEncode( recipe_title ) );

	QSQLiteResult alikeRecipes = database->executeQuery( command );
	if ( alikeRecipes.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = alikeRecipes.first();
		int count = 0;
		if ( !alikeRecipes.atEnd() )
		{
			count = row.data( 0 ).toInt();
			return_title = QString( "%1 (%2)" ).arg( recipe_title ).arg( count + 2 );

			//make sure this newly created title is unique (just in case)
			while ( findExistingRecipeByName( return_title ) != -1 ) {
				count--; //go down to find the skipped recipe(s)
				return_title = QString( "%1 (%2)" ).arg( recipe_title ).arg( count + 2 );
			}
		}
	}

	return return_title;
}

QString LiteRecipeDB::recipeTitle( int recipeID )
{
	QString command = QString( "SELECT * FROM recipes WHERE id=%1;" ).arg( recipeID );
	QSQLiteResult recipeToLoad = database->executeQuery( command );
	if ( recipeToLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = recipeToLoad.first();
		if ( !recipeToLoad.atEnd() )  // Go to the first record (there should be only one anyway.
			return ( row.data( 1 ) );
	}
	return ( QString::null );
}

int LiteRecipeDB::lastInsertID()
{
	int lastID;
	QSQLiteResult lastInsertID = database->executeQuery( "SELECT ver from db_info;", &lastID ); // Execute whatever query that doesn't insert rows, although let's make it a query that doesn't give an error (so we don't generate a false error in the console output)

	return lastID;
}

void LiteRecipeDB::emptyData( void )
{
	QStringList tables;
	tables << "ingredient_info" << "ingredient_list" << "ingredient_properties" << "ingredients" << "recipes" << "unit_list" << "units" << "units_conversion" << "categories" << "category_list" << "authors" << "author_list" << "prep_methods" << "ingredient_groups" << "yield_types" << "prep_method_list";

	for ( QStringList::Iterator it = tables.begin(); it != tables.end(); ++it ) {
		QString command = QString( "DELETE FROM %1;" ).arg( *it );
		database->executeQuery( command );
	}
}

void LiteRecipeDB::empty( void )
{
	QStringList tables;
	tables << "ingredient_info" << "ingredient_list" << "ingredient_properties" << "ingredients" << "recipes" << "unit_list" << "units" << "units_conversion" << "categories" << "category_list" << "authors" << "author_list" << "prep_methods" << "ingredient_groups" << "db_info" << "yield_types" << "prep_method_list";

	for ( QStringList::Iterator it = tables.begin(); it != tables.end(); ++it ) {
		QString command = QString( "DROP TABLE %1;" ).arg( *it );
		database->executeQuery( command );
	}
}

void LiteRecipeDB::search( RecipeList *list, int items, const RecipeSearchParameters &parameters )
{
	QString query = buildSearchQuery(parameters);
	
	QSQLiteResult recipesToLoad = database->executeQuery( query );

	QValueList<int> ids;
	if ( recipesToLoad.getStatus() != QSQLiteResult::Failure ) {
		QSQLiteResultRow row = recipesToLoad.first();
		while ( !recipesToLoad.atEnd() ) {
			ids << row.data( 0 ).toInt();

			row = recipesToLoad.next();
		}
	}

	if ( ids.count() > 0 )
		loadRecipes( list, items, ids );
}

/*
** Encode a binary buffer "in" of size n bytes so that it contains
** no instances of characters '\'' or '\000'.  The output is
** null-terminated and can be used as a string value in an INSERT
** or UPDATE statement.  Use sqlite_decode_binary() to convert the
** string back into its original binary.
**
** The result is written into a preallocated output buffer "out".
** "out" must be able to hold at least 2 +(257*n)/254 bytes.
** In other words, the output will be expanded by as much as 3
** bytes for every 254 bytes of input plus 2 bytes of fixed overhead.
** (This is approximately 2 + 1.0118*n or about a 1.2% size increase.)
**
** The return value is the number of characters in the encoded
** string, excluding the "\000" terminator.
*/

int LiteRecipeDB::sqlite_encode_binary( const unsigned char *in, int n, unsigned char *out )
{
	int i = 0, j = 0, e = 0, m = 0;
	int cnt[ 256 ];
	if ( n <= 0 ) {
		out[ 0 ] = 'x';
		out[ 1 ] = 0;
		return 1;
	}
	memset( cnt, 0, sizeof( cnt ) );
	for ( i = n - 1; i >= 0; i-- ) {
		cnt[ in[ i ] ] ++;
	}
	m = n;
	for ( i = 1; i < 256; i++ ) {
		int sum;
		if ( i == '\'' )
			continue;
		sum = cnt[ i ] + cnt[ ( i + 1 ) & 0xff ] + cnt[ ( i + '\'' ) & 0xff ];
		if ( sum < m ) {
			m = sum;
			e = i;
			if ( m == 0 )
				break;
		}
	}
	out[ 0 ] = e;
	j = 1;
	for ( i = 0; i < n; i++ ) {
		int c = ( in[ i ] - e ) & 0xff;
		if ( c == 0 ) {
			out[ j++ ] = 1;
			out[ j++ ] = 1;
		}
		else if ( c == 1 ) {
			out[ j++ ] = 1;
			out[ j++ ] = 2;
		}
		else if ( c == '\'' ) {
			out[ j++ ] = 1;
			out[ j++ ] = 3;
		}
		else {
			out[ j++ ] = c;
		}
	}
	out[ j ] = 0;
	return j;
}

/*
** Decode the string "in" into binary data and write it into "out".
** This routine reverses the encoding created by sqlite_encode_binary().
** The output will always be a few bytes less than the input.  The number
** of bytes of output is returned.  If the input is not a well-formed
** encoding, -1 is returned.
**
** The "in" and "out" parameters may point to the same buffer in order
** to decode a string in place.
*/
int LiteRecipeDB::sqlite_decode_binary( const unsigned char *in, unsigned char *out )
{
	int i, c, e;
	e = *( in++ );
	i = 0;
	while ( ( c = *( in++ ) ) != 0 ) {
		if ( c == 1 ) {
			c = *( in++ );
			if ( c == 1 ) {
				c = 0;
			}
			else if ( c == 2 ) {
				c = 1;
			}
			else if ( c == 3 ) {
				c = '\'';
			}
			else {
				return -1;
			}
		}
		out[ i++ ] = ( c + e ) & 0xff;
	}
	return i;
}

#include "literecipedb.moc"
