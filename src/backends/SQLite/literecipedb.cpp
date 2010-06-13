/***************************************************************************
*   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2004, 2006 Jason Kivlighn <jkivlighn@gmail.com>      *
*   Copyright © 2003-2004 Cyril Bosselut <bosselut@b1project.com>         *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "literecipedb.h"

#include <QBuffer>
//Added by qt3to4:
#include <QSqlQuery>

#include <kdebug.h>
#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <KConfigGroup>
#include <config-krecipes.h>
#include <QImageWriter>
#include <KCodecs>

#ifdef HAVE_SQLITE3
#include <sqlite3.h>
#elif HAVE_SQLITE
#include <sqlite.h>
#endif

//keep these two around for porting old databases
int sqlite_decode_binary( const unsigned char *in, unsigned char *out );
QString escape( const QString &s );

LiteRecipeDB::LiteRecipeDB( const QString &_dbFile ) : QSqlRecipeDB( QString(), QString(), QString(), _dbFile )
{
	kDebug();
/*	KConfig * config = KGlobal::config();
	config->setGroup( "Server" );

	if ( dbFile.isNull() )
		dbFile = config->readEntry( "DBFile", KStandardDirs::locateLocal ( "appdata", DB_FILENAME ) );
*/
}

LiteRecipeDB::~LiteRecipeDB()
{
}

int LiteRecipeDB::lastInsertID()
{
	kDebug();
	int lastID = -1;
	QSqlQuery query( "SELECT lastInsertID()", *database );
	if ( query.isActive() && query.first() )
		lastID = query.value(0).toInt();

	//kDebug()<<"lastInsertID(): "<<lastID;

	return lastID;
}

QStringList LiteRecipeDB::backupCommand() const
{
#ifdef HAVE_SQLITE
	QString binary = "sqlite";
#elif HAVE_SQLITE3
	QString binary = "sqlite3";
#endif

	KConfigGroup config( KGlobal::config(), "Server" );
	binary = config.readEntry( "SQLitePath", binary );

	QStringList command;
	command<<binary<<database->databaseName()<<".dump";
	return command;
}

QStringList LiteRecipeDB::restoreCommand() const
{
#ifdef HAVE_SQLITE
	QString binary = "sqlite";
#elif HAVE_SQLITE3
	QString binary = "sqlite3";
#endif
	kDebug()<<" binary: "<<binary;
	KConfigGroup config( KGlobal::config(), "Server" );
	binary = config.readEntry( "SQLitePath", binary );

	QStringList command;
	command<<binary<<database->databaseName();
	return command;
}

void LiteRecipeDB::createDB()
{
	//The file is created by SQLite automatically
}

void LiteRecipeDB::createTable( const QString &tableName )
{
	kDebug()<<" tableName :"<<tableName;
	QStringList commands;

	if ( tableName == "recipes" )
		commands << QString( "CREATE TABLE recipes (id INTEGER NOT NULL,title VARCHAR(%1), yield_amount FLOAT, yield_amount_offset FLOAT, yield_type_id INTEGER DEFAULT '-1', instructions TEXT, photo BLOB, prep_time TIME, ctime TIMESTAMP, mtime TIMESTAMP, atime TIMESTAMP,  PRIMARY KEY (id));" ).arg( maxRecipeTitleLength() );

	else if ( tableName == "ingredients" )
		commands << QString( "CREATE TABLE ingredients (id INTEGER NOT NULL, name VARCHAR(%1), PRIMARY KEY (id));" ).arg( maxIngredientNameLength() );

	else if ( tableName == "ingredient_list" ) {
		commands << "CREATE TABLE ingredient_list (id INTEGER NOT NULL, recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, amount_offset FLOAT, unit_id INTEGER, order_index INTEGER, group_id INTEGER, substitute_for INTEGER, PRIMARY KEY(id) );"
		<< "CREATE index ridil_index ON ingredient_list(recipe_id);"
		<< "CREATE index iidil_index ON ingredient_list(ingredient_id);"
		<< "CREATE index gidil_index ON ingredient_list(group_id);";
	}

	else if ( tableName == "unit_list" )
		commands << "CREATE TABLE unit_list (ingredient_id INTEGER, unit_id INTEGER);";

	else if ( tableName == "units" )
		commands << QString( "CREATE TABLE units (id INTEGER NOT NULL, name VARCHAR(%1), name_abbrev VARCHAR(%2), plural VARCHAR(%3), plural_abbrev VARCHAR(%4), type INTEGER NOT NULL DEFAULT '0',PRIMARY KEY (id));" )
		   .arg( maxUnitNameLength() ).arg( maxUnitNameLength() ).arg( maxUnitNameLength() ).arg( maxUnitNameLength() );

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

	else if ( tableName == "ingredient_weights" ) {
		commands << "CREATE TABLE ingredient_weights (id INTEGER NOT NULL, ingredient_id INTEGER NOT NULL, amount FLOAT, unit_id INTEGER, weight FLOAT, weight_unit_id INTEGER, prep_method_id INTEGER, PRIMARY KEY (id) );"

		<< "CREATE index weight_wid_index ON ingredient_weights(weight_unit_id)"
		<< "CREATE index weight_pid_index ON ingredient_weights(prep_method_id)"
		<< "CREATE index weight_uid_index ON ingredient_weights(unit_id)"
		<< "CREATE index weight_iid_index ON ingredient_weights(ingredient_id)";
	}

	else if ( tableName == "units_conversion" )
		commands << "CREATE TABLE units_conversion (unit1_id INTEGER, unit2_id INTEGER, ratio FLOAT);";

	else if ( tableName == "categories" ) {
		commands << QString( "CREATE TABLE categories (id INTEGER NOT NULL, name varchar(%1) default NULL, parent_id INGEGER NOT NULL default -1, PRIMARY KEY (id));" ).arg( maxCategoryNameLength() );
		commands << "CREATE index parent_id_index ON categories(parent_id);";
	}
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

	else if ( tableName == "ratings" )
		commands << "CREATE TABLE ratings (id INTEGER NOT NULL, recipe_id int(11) NOT NULL, comment TEXT, rater TEXT, created TIMESTAMP, PRIMARY KEY (id));";

	else if ( tableName == "rating_criteria" )
		commands << "CREATE TABLE rating_criteria (id INTEGER NOT NULL, name TEXT, PRIMARY KEY (id));";

	else if ( tableName == "rating_criterion_list" )
		commands << "CREATE TABLE rating_criterion_list (rating_id INTEGER NOT NULL, rating_criterion_id INTEGER, stars FLOAT);";

	else
		return ;

	// execute the queries
	for ( QStringList::const_iterator it = commands.constBegin(); it != commands.constEnd(); ++it )
		database->exec( *it );

}

void LiteRecipeDB::portOldDatabases( float version )
{
	QString command;
	if ( qRound(version*10) < 5 ) {
		//===========add prep_method_id to ingredient_list table
		//There's no ALTER command in SQLite, so we have to copy all data to a new table and then recreate the table with the prep_method_id
		database->exec( "CREATE TABLE ingredient_list_copy (recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, unit_id INTEGER, order_index INTEGER);" );
		QSqlQuery copyQuery( "SELECT recipe_id,ingredient_id,amount,unit_id,order_index FROM ingredient_list;", *database );
		if ( copyQuery.isActive() ) {
			while ( copyQuery.next() ) {
				command = QString( "INSERT INTO ingredient_list_copy VALUES(%1,%2,%3,%4,%5);" )
				          .arg( copyQuery.value( 0 ).toInt() )
				          .arg( copyQuery.value( 1 ).toInt() )
				          .arg( copyQuery.value( 2 ).toString().toDouble() )
				          .arg( copyQuery.value( 3 ).toInt() )
				          .arg( copyQuery.value( 4 ).toInt() );
				database->exec( command );

				emit progress();
			}
		}
		database->exec( "DROP TABLE ingredient_list" );
		database->exec( "CREATE TABLE ingredient_list (recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, unit_id INTEGER, prep_method_id INTEGER, order_index INTEGER);" );
		copyQuery = database->exec( "SELECT * FROM ingredient_list_copy" );
		if ( copyQuery.isActive() ) {
			while ( copyQuery.next() ) {
				command = QString( "INSERT INTO ingredient_list VALUES(%1,%2,%3,%4,%5,%6);" )
				          .arg( copyQuery.value( 0 ).toInt() )
				          .arg( copyQuery.value( 1 ).toInt() )
				          .arg( copyQuery.value( 2 ).toString().toDouble() )
				          .arg( copyQuery.value( 3 ).toInt() )
				          .arg( -1 )  //default prep method
				          .arg( copyQuery.value( 4 ).toInt() );
				database->exec( command );

				emit progress();
			}
		}
		database->exec( "DROP TABLE ingredient_list_copy" );

		database->exec( "CREATE index ridil_index ON ingredient_list(recipe_id);" );
		database->exec( "CREATE index iidil_index ON ingredient_list(ingredient_id);" );


		//==============expand length of author name to 50 characters
		database->exec( "CREATE TABLE authors_copy (id INTEGER, name varchar(20));" );
		copyQuery = database->exec( "SELECT * FROM authors;" );
		if ( copyQuery.isActive() ) {

			while ( copyQuery.next() ) {
				command = QString( "INSERT INTO authors_copy VALUES(%1,'%2');" )
				          .arg( copyQuery.value( 0 ).toInt() )
				          .arg( escape( copyQuery.value( 1 ).toString() ) );
				database->exec( command );

				emit progress();
			}
		}
		database->exec( "DROP TABLE authors" );
		database->exec( "CREATE TABLE authors (id INTEGER NOT NULL, name varchar(50) default NULL,PRIMARY KEY (id));" );
		copyQuery = database->exec( "SELECT * FROM authors_copy" );
		if ( copyQuery.isActive() ) {

			while ( copyQuery.next() ) {
				command = QString( "INSERT INTO authors VALUES(%1,'%2');" )
				          .arg( copyQuery.value( 0 ).toInt() )
				          .arg( escape( copyQuery.value( 1 ).toString() ) );
				database->exec( command );

				emit progress();
			}
		}
		database->exec( "DROP TABLE authors_copy" );


		//==================expand length of category name to 40 characters
		database->exec( "CREATE TABLE categories_copy (id INTEGER, name varchar(20));" );
		copyQuery = database->exec( "SELECT * FROM categories;" );
		if ( copyQuery.isActive() ) {

			while ( copyQuery.next() ) {
				command = QString( "INSERT INTO categories_copy VALUES(%1,'%2');" )
				          .arg( copyQuery.value( 0 ).toInt() )
				          .arg( escape( copyQuery.value( 1 ).toString() ) );
				database->exec( command );

				emit progress();
			}
		}
		database->exec( "DROP TABLE categories" );
		database->exec( "CREATE TABLE categories (id INTEGER NOT NULL, name varchar(40) default NULL,PRIMARY KEY (id));" );
		copyQuery = database->exec( "SELECT * FROM categories_copy" );
		if ( copyQuery.isActive() ) {

			while ( copyQuery.next() ) {
				command = QString( "INSERT INTO categories VALUES(%1,'%2');" )
				          .arg( copyQuery.value( 0 ).toInt() )
				          .arg( escape( copyQuery.value( 1 ).toString() ) );
				database->exec( command );

				emit progress();
			}
		}
		database->exec( "DROP TABLE categories_copy" );

		//================Set the version to the new one (0.5)
		command = "DELETE FROM db_info;"; // Remove previous version records if they exist
		database->exec( command );
		command = "INSERT INTO db_info VALUES(0.5,'Krecipes 0.5');";
		database->exec( command );
	}

	if ( qRound(version*10) < 6 ) {
		//==================add a column to 'categories' to allow subcategories
		database->exec( "CREATE TABLE categories_copy (id INTEGER, name varchar(40));" );
		QSqlQuery copyQuery = database->exec( "SELECT * FROM categories;" );
		if ( copyQuery.isActive() ) {

			while ( copyQuery.next() ) {
				command = QString( "INSERT INTO categories_copy VALUES(%1,'%2');" )
				          .arg( copyQuery.value( 0 ).toInt() )
				          .arg( escape( copyQuery.value( 1 ).toString() ) );
				database->exec( command );

				emit progress();
			}
		}
		database->exec( "DROP TABLE categories" );
		database->exec( "CREATE TABLE categories (id INTEGER NOT NULL, name varchar(40) default NULL, parent_id INTEGER NOT NULL, PRIMARY KEY (id));" );
		copyQuery = database->exec( "SELECT * FROM categories_copy" );
		if ( copyQuery.isActive() ) {

			while ( copyQuery.next() ) {
				command = QString( "INSERT INTO categories VALUES(%1,'%2',-1);" )
				          .arg( copyQuery.value( 0 ).toInt() )
				          .arg( escape( copyQuery.value( 1 ).toString() ) );
				database->exec( command );

				emit progress();
			}
		}
		database->exec( "DROP TABLE categories_copy" );

		//================Set the version to the new one (0.6)
		command = "DELETE FROM db_info;"; // Remove previous version records if they exist
		database->exec( command );
		command = "INSERT INTO db_info VALUES(0.6,'Krecipes 0.6');";
		database->exec( command );
	}

	if ( qRound(version*100) < 61 ) {
		//==================add a column to 'recipes' to allow prep time
		database->exec( "CREATE TABLE recipes_copy (id INTEGER NOT NULL,title VARCHAR(200),persons INTEGER,instructions TEXT, photo BLOB,   PRIMARY KEY (id));" );
		QSqlQuery copyQuery = database->exec( "SELECT * FROM recipes;" );
		if ( copyQuery.isActive() ) {

			while ( copyQuery.next() ) {
				command = QString( "INSERT INTO recipes_copy VALUES(%1,'%2','%3','%4','%5');" )
				          .arg( escape( copyQuery.value( 0 ).toString() ) )
				          .arg( escape( copyQuery.value( 1 ).toString() ) )
				          .arg( escape( copyQuery.value( 2 ).toString() ) )
				          .arg( escape( copyQuery.value( 3 ).toString() ) )
				          .arg( escape( copyQuery.value( 4 ).toString() ) );
				database->exec( command );

				emit progress();
			}
		}
		database->exec( "DROP TABLE recipes" );
		database->exec( "CREATE TABLE recipes (id INTEGER NOT NULL,title VARCHAR(200),persons INTEGER,instructions TEXT, photo BLOB, prep_time TIME,   PRIMARY KEY (id));" );
		copyQuery = database->exec( "SELECT * FROM recipes_copy" );
		if ( copyQuery.isActive() ) {

			while ( copyQuery.next() ) {
				command = QString( "INSERT INTO recipes VALUES(%1,'%2','%3','%4','%5',NULL);" )
				          .arg( escape( copyQuery.value( 0 ).toString() ) )
				          .arg( escape( copyQuery.value( 1 ).toString() ) )
				          .arg( escape( copyQuery.value( 2 ).toString() ) )
				          .arg( escape( copyQuery.value( 3 ).toString() ) )
				          .arg( escape( copyQuery.value( 4 ).toString() ) );

				database->exec( command );

				emit progress();
			}
		}
		database->exec( "DROP TABLE recipes_copy" );

		//================Set the version to the new one (0.61)
		command = "DELETE FROM db_info;"; // Remove previous version records if they exist
		database->exec( command );
		command = "INSERT INTO db_info VALUES(0.61,'Krecipes 0.6');";
		database->exec( command );
	}

	if ( qRound(version*100) < 62 ) {
		database->transaction();

		//==================add a column to 'ingredient_list' to allow grouping ingredients
		database->exec( "CREATE TABLE ingredient_list_copy (recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, unit_id INTEGER, prep_method_id INTEGER, order_index INTEGER);" );
		QSqlQuery copyQuery = database->exec( "SELECT * FROM ingredient_list;" );
		if ( copyQuery.isActive() ) {

			while ( copyQuery.next() ) {
				command = "INSERT INTO ingredient_list_copy VALUES('" + escape( copyQuery.value( 0 ).toString() )
				          + "','" + escape( copyQuery.value( 1 ).toString() )
				          + "','" + escape( copyQuery.value( 2 ).toString() )
				          + "','" + escape( copyQuery.value( 3 ).toString() )
				          + "','" + escape( copyQuery.value( 4 ).toString() )
				          + "','" + escape( copyQuery.value( 5 ).toString() )
				          + "');";
				database->exec( command );

				emit progress();
			}
		}
		database->exec( "DROP TABLE ingredient_list" );
		database->exec( "CREATE TABLE ingredient_list (recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, unit_id INTEGER, prep_method_id INTEGER, order_index INTEGER, group_id INTEGER);" );
		copyQuery = database->exec( "SELECT * FROM ingredient_list_copy" );
		if ( copyQuery.isActive() ) {

			while ( copyQuery.next() ) {
				command = "INSERT INTO ingredient_list VALUES('" + escape( copyQuery.value( 0 ).toString() )
				          + "','" + escape( copyQuery.value( 1 ).toString() )
				          + "','" + escape( copyQuery.value( 2 ).toString() )
				          + "','" + escape( copyQuery.value( 3 ).toString() )
				          + "','" + escape( copyQuery.value( 4 ).toString() )
				          + "','" + escape( copyQuery.value( 5 ).toString() )
				          + "',-1)";
				database->exec( command );

				emit progress();
			}
		}
		database->exec( "DROP TABLE ingredient_list_copy" );

		command = "DELETE FROM db_info;"; // Remove previous version records if they exist
		database->exec( command );
		command = "INSERT INTO db_info VALUES(0.62,'Krecipes 0.7');";
		database->exec( command );

		database->commit();
	}

	if ( qRound(version*100) < 63 ) {
		database->transaction();

		//==================add a column to 'units' to allow handling plurals
		database->exec( "CREATE TABLE units_copy (id INTEGER NOT NULL, name VARCHAR(20), PRIMARY KEY (id));" );
		QSqlQuery copyQuery = database->exec( "SELECT id,name FROM units;" );
		if ( copyQuery.isActive() ) {

			while ( copyQuery.next() ) {
				command = "INSERT INTO units_copy VALUES('" + escape( copyQuery.value( 0 ).toString() )
				          + "','" + escape( copyQuery.value( 1 ).toString() )
				          + "');";
				database->exec( command );

				emit progress();
			}
		}
		database->exec( "DROP TABLE units" );
		database->exec( "CREATE TABLE units (id INTEGER NOT NULL, name VARCHAR(20), plural VARCHAR(20), PRIMARY KEY (id));" );
		copyQuery = database->exec( "SELECT id,name FROM units_copy" );
		if ( copyQuery.isActive() ) {
			while ( copyQuery.next() ) {
				command = "INSERT INTO units VALUES('" + escape( copyQuery.value( 0 ).toString() )
				          + "','" + escape( copyQuery.value( 1 ).toString() )
				          + "',NULL)";
				database->exec( command );

				emit progress();
			}
		}
		database->exec( "DROP TABLE units_copy" );

		QSqlQuery result = database->exec( "SELECT id,name FROM units WHERE plural ISNULL;" );
		if ( result.isActive() ) {
			while ( result.next() ) {
				command = "UPDATE units SET plural='" + escape( result.value( 1 ).toString() ) + "' WHERE id=" + QString::number( result.value( 0 ).toInt() );
				database->exec( command );

				emit progress();
			}
		}

		command = "DELETE FROM db_info;"; // Remove previous version records if they exist
		database->exec( command );
		command = "INSERT INTO db_info VALUES(0.63,'Krecipes 0.7');";
		database->exec( command );

		database->commit();
	}

	if ( qRound(version*10) < 7 ) { //simply call 0.63 -> 0.7
		database->exec( "UPDATE db_info SET ver='0.7';" );
	}

	if ( qRound(version*100) < 81 ) {
		database->transaction();
		addColumn("CREATE TABLE %1 (recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, %2 unit_id INTEGER, prep_method_id INTEGER, order_index INTEGER, group_id INTEGER)","amount_offset FLOAT","'0'","ingredient_list",3);

		//addColumn() doesn't preserve indexes
		database->exec("CREATE index ridil_index ON ingredient_list(recipe_id)");
		database->exec("CREATE index iidil_index ON ingredient_list(ingredient_id)");

		database->exec( "UPDATE db_info SET ver='0.81',generated_by='Krecipes SVN (20050816)';" );
		database->commit();
	}

	if ( qRound(version*100) < 82 ) {
		database->transaction();

		//==================add a columns to 'recipes' to allow yield range + yield type
		database->exec( "CREATE TABLE recipes_copy (id INTEGER NOT NULL,title VARCHAR(200),persons INTEGER,instructions TEXT, photo BLOB, prep_time TIME, PRIMARY KEY (id));" );
		QSqlQuery copyQuery = database->exec( "SELECT id,title,persons,instructions,photo,prep_time FROM recipes;" );
		if ( copyQuery.isActive() ) {

			while ( copyQuery.next() ) {
				command = "INSERT INTO recipes_copy VALUES('"
				                  + escape( copyQuery.value( 0 ).toString() ) //id
				          + "','" + escape( copyQuery.value( 1 ).toString() ) //title
				          + "','" + escape( copyQuery.value( 2 ).toString() ) //persons
				          + "','" + escape( copyQuery.value( 3 ).toString() ) //instructions
				          + "','" + escape( copyQuery.value( 4 ).toString() ) //photo
				          + "','" + escape( copyQuery.value( 5 ).toString() ) //prep_time
				          + "')";
				database->exec( command );

				emit progress();
			}
		}
		database->exec( "DROP TABLE recipes" );
		database->exec( "CREATE TABLE recipes (id INTEGER NOT NULL,title VARCHAR(200), yield_amount FLOAT, yield_amount_offset FLOAT, yield_type_id INTEGER, instructions TEXT, photo BLOB, prep_time TIME, PRIMARY KEY (id));" );
		copyQuery = database->exec( "SELECT id,title,persons,instructions,photo,prep_time FROM recipes_copy" );
		if ( copyQuery.isActive() ) {

			while ( copyQuery.next() ) {
				command = "INSERT INTO recipes VALUES('"
				                  + escape( copyQuery.value( 0 ).toString() ) //id
				          + "','" + escape( copyQuery.value( 1 ).toString() ) //title
				          + "','" + escape( copyQuery.value( 2 ).toString() ) //persons, now yield_amount
				          + "','0"                          //yield_amount_offset
				          + "','-1"                         //yield_type_id
				          + "','" + escape( copyQuery.value( 3 ).toString() ) //instructions
				          + "','" + escape( copyQuery.value( 4 ).toString() ) //photo
				          + "','" + escape( copyQuery.value( 5 ).toString() ) //prep_time
				          + "')";
				database->exec( command );

				emit progress();
			}
		}
		database->exec( "DROP TABLE recipes_copy" );

		database->exec( "UPDATE db_info SET ver='0.82',generated_by='Krecipes SVN (20050902)';" );
		database->commit();
	}

	if ( qRound(version*100) < 83 ) {
		database->transaction();

		//====add a id columns to 'ingredient_list' to identify it for the prep method list
		database->exec( "CREATE TABLE ingredient_list_copy (recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, amount_offset FLOAT, unit_id INTEGER, prep_method_id INTEGER, order_index INTEGER, group_id INTEGER);" );
		QSqlQuery copyQuery = database->exec( "SELECT recipe_id,ingredient_id,amount,amount_offset,unit_id,prep_method_id,order_index,group_id FROM ingredient_list" );
		if ( copyQuery.isActive() ) {

			while ( copyQuery.next() ) {
				command = "INSERT INTO ingredient_list_copy VALUES('"
				                  + escape( copyQuery.value( 0 ).toString() )
				          + "','" + escape( copyQuery.value( 1 ).toString() )
				          + "','" + escape( copyQuery.value( 2 ).toString() )
				          + "','" + escape( copyQuery.value( 3 ).toString() )
				          + "','" + escape( copyQuery.value( 4 ).toString() )
				          + "','" + escape( copyQuery.value( 5 ).toString() )
				          + "','" + escape( copyQuery.value( 6 ).toString() )
				          + "','" + escape( copyQuery.value( 7 ).toString() )
				          + "')";
				database->exec( command );

				emit progress();
			}
		}
		database->exec( "DROP TABLE ingredient_list" );
		database->exec( "CREATE TABLE ingredient_list (id INTEGER NOT NULL, recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, amount_offset FLOAT, unit_id INTEGER, order_index INTEGER, group_id INTEGER, PRIMARY KEY(id) );" );

		copyQuery = database->exec( "SELECT recipe_id,ingredient_id,amount,amount_offset,unit_id,prep_method_id,order_index,group_id FROM ingredient_list_copy" );
		if ( copyQuery.isActive() ) {

			while ( copyQuery.next() ) {
				command = "INSERT INTO ingredient_list VALUES("
				                  + QString("NULL")
				          + ",'" + escape( copyQuery.value( 0 ).toString() )
				          + "','" + escape( copyQuery.value( 1 ).toString() )
				          + "','" + escape( copyQuery.value( 2 ).toString() )
				          + "','" + escape( copyQuery.value( 3 ).toString() )
				          + "','" + escape( copyQuery.value( 4 ).toString() )
				          + "','" + escape( copyQuery.value( 6 ).toString() )
				          + "','" + escape( copyQuery.value( 7 ).toString() )
				          + "')";
				database->exec( command );

				int prep_method_id = copyQuery.value( 5 ).toInt();
				if ( prep_method_id != -1 ) {
					command = "INSERT INTO prep_method_list VALUES('"
							+ QString::number(lastInsertID())
						+ "','" + QString::number(prep_method_id)
						+ "','1" //order_index
						+ "')";
					database->exec( command );
				}

				emit progress();
			}
		}
		database->exec( "DROP TABLE ingredient_list_copy" );

		database->exec( "CREATE INDEX ridil_index ON ingredient_list(recipe_id);" );
		database->exec( "CREATE INDEX iidil_index ON ingredient_list(ingredient_id);" );

		database->exec( "UPDATE db_info SET ver='0.83',generated_by='Krecipes SVN (20050909)';" );

		database->commit();
	}

	if ( qRound(version*100) < 84 ) {
		database->transaction();

		//==================add a columns to 'recipes' to allow storing atime, mtime, ctime
		database->exec( "CREATE TABLE recipes_copy (id INTEGER NOT NULL,title VARCHAR(200), yield_amount FLOAT, yield_amount_offset FLOAT, yield_type_id INTEGER, instructions TEXT, photo BLOB, prep_time TIME, PRIMARY KEY (id));" );
		QSqlQuery copyQuery = database->exec( "SELECT id,title,yield_amount,yield_amount_offset,yield_type_id,instructions,photo,prep_time FROM recipes;" );
		if ( copyQuery.isActive() ) {

			while ( copyQuery.next() ) {
				command = "INSERT INTO recipes_copy VALUES('"
				                  + escape( copyQuery.value( 0 ).toString() )
				          + "','" + escape( copyQuery.value( 1 ).toString() )
				          + "','" + escape( copyQuery.value( 2 ).toString() )
				          + "','" + escape( copyQuery.value( 3 ).toString() )
				          + "','" + escape( copyQuery.value( 4 ).toString() )
				          + "','" + escape( copyQuery.value( 5 ).toString() )
				          + "','" + escape( copyQuery.value( 6 ).toString() )
				          + "','" + escape( copyQuery.value( 7 ).toString() )
				          + "')";
				database->exec( command );

				emit progress();
			}
		}
		database->exec( "DROP TABLE recipes" );
		database->exec( "CREATE TABLE recipes (id INTEGER NOT NULL,title VARCHAR(200), yield_amount FLOAT, yield_amount_offset FLOAT, yield_type_id INTEGER DEFAULT '-1', instructions TEXT, photo BLOB, prep_time TIME, ctime TIMESTAMP, mtime TIMESTAMP, atime TIMESTAMP,  PRIMARY KEY (id))" );
		copyQuery = database->exec( "SELECT id,title,yield_amount,yield_amount_offset,yield_type_id,instructions,photo,prep_time FROM recipes_copy" );
		if ( copyQuery.isActive() ) {

			QString current_timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
			while ( copyQuery.next() ) {
				command = "INSERT INTO recipes VALUES('"
				                  + escape( copyQuery.value( 0 ).toString() )
				          + "','" + escape( copyQuery.value( 1 ).toString() )
				          + "','" + escape( copyQuery.value( 2 ).toString() )
				          + "','" + escape( copyQuery.value( 3 ).toString() )
				          + "','" + escape( copyQuery.value( 4 ).toString() )
				          + "','" + escape( copyQuery.value( 5 ).toString() )
				          + "','" + escape( copyQuery.value( 6 ).toString() )
				          + "','" + escape( copyQuery.value( 7 ).toString() )
				          + "','" + escape( current_timestamp ) //ctime
				          + "','" + escape( current_timestamp ) //mtime
				          + "','" + escape( current_timestamp ) //atime
				          + "')";
				database->exec( command );

				emit progress();
			}
		}
		database->exec( "DROP TABLE recipes_copy" );

		database->exec( "UPDATE db_info SET ver='0.84',generated_by='Krecipes SVN (20050913)';" );
		database->commit();
	}

	if ( qRound(version*100) < 85 ) {
		database->transaction();

		QSqlQuery query( "SELECT id,photo FROM recipes", *database );

		if ( query.isActive() ) {
			while ( query.next() ) {
				QImage photo;
				QString photoString = query.value(1).toString();

				// Decode the photo
				uchar *photoArray = new uchar [ photoString.length() + 1 ];
				memcpy( photoArray, photoString.toLatin1(), photoString.length() * sizeof( char ) );
				sqlite_decode_binary( ( uchar* ) photoArray, ( uchar* ) photoArray );

				photo.loadFromData( photoArray, photoString.length() );

				// picture will now have a ready-to-use image
				delete[] photoArray;

				QByteArray ba;
				QBuffer buffer( &ba );
				buffer.open( QIODevice::WriteOnly );
				QImageWriter iio( &buffer, "JPEG" );
				iio.write( photo );
				//recipe->photo.save( &buffer, "JPEG" ); don't need QImageIO in QT 3.2
				storePhoto( query.value(0).toInt(), ba );

				emit progress();
			}
		}


		database->exec( "UPDATE db_info SET ver='0.85',generated_by='Krecipes SVN (20050926)';" );
		database->commit();
	}

	if ( qRound(version*100) < 86 ) {
		database->transaction();

		database->exec( "CREATE index gidil_index ON ingredient_list(group_id)" );

		QSqlQuery query( "SELECT id,name FROM ingredient_groups ORDER BY name", *database );

		QString last;
		int lastID=-1;
		if ( query.isActive() ) {
			while ( query.next() ) {
				QString name = query.value(1).toString();
				int id = query.value(0).toInt();
				if ( last == name ) {
					QString command = QString("UPDATE ingredient_list SET group_id=%1 WHERE group_id=%2").arg(lastID).arg(id);
					database->exec(command);

					command = QString("DELETE FROM ingredient_groups WHERE id=%1").arg(id);
					database->exec(command);
				}
				last = name;
				lastID = id;

				emit progress();
			}
		}

		database->exec( "UPDATE db_info SET ver='0.86',generated_by='Krecipes SVN (20050928)'" );
		if ( !database->commit() )
			kDebug()<<"Update to 0.86 failed.  Maybe you should try again.";
	}

	if ( qRound(version*100) < 87 ) {
		//Load this default data so the user knows what rating criteria is
		database->exec( QString("INSERT INTO rating_criteria VALUES (1,'%1')").arg(i18nc("Overall rating", "Overall")) );
		database->exec( QString("INSERT INTO rating_criteria VALUES (2,'%1')").arg(i18n("Taste") ) );
		database->exec( QString("INSERT INTO rating_criteria VALUES (3,'%1')").arg(i18n("Appearance") ) );
		database->exec( QString("INSERT INTO rating_criteria VALUES (4,'%1')").arg(i18n("Originality") ) );
		database->exec( QString("INSERT INTO rating_criteria VALUES (5,'%1')").arg(i18n("Ease of Preparation") ) );

		database->exec( "UPDATE db_info SET ver='0.87',generated_by='Krecipes SVN (20051014)'" );
	}

	if ( qRound(version*100) < 90 ) {
		database->exec("UPDATE db_info SET ver='0.9',generated_by='Krecipes 0.9'");
	}

	if ( qRound(version*100) < 91 ) {
		database->exec("CREATE index parent_id_index ON categories(parent_id)");
		database->exec("UPDATE db_info SET ver='0.91',generated_by='Krecipes SVN (20060526)'");
	}

	if ( qRound(version*100) < 92 ) {
		database->transaction();

		//==================add a columns to 'units' to allow unit abbreviations
		database->exec( "CREATE TABLE units_copy (id INTEGER NOT NULL, name VARCHAR(20), plural VARCHAR(20))" );
		QSqlQuery copyQuery = database->exec( "SELECT id,name,plural FROM units" );
		if ( copyQuery.isActive() ) {

			while ( copyQuery.next() ) {
				command = "INSERT INTO units_copy VALUES('"
				                  + escape( copyQuery.value( 0 ).toString() ) //id
				          + "','" + escape( copyQuery.value( 1 ).toString() ) //name
				          + "','" + escape( copyQuery.value( 2 ).toString() ) //plural
				          + "')";
				database->exec( command );

				emit progress();
			}
		}
		database->exec( "DROP TABLE units" );
		database->exec( "CREATE TABLE units (id INTEGER NOT NULL, name VARCHAR(20), name_abbrev VARCHAR(20), plural VARCHAR(20), plural_abbrev VARCHAR(20), PRIMARY KEY (id))" );
		copyQuery = database->exec( "SELECT id,name,plural FROM units_copy" );
		if ( copyQuery.isActive() ) {
			while ( copyQuery.next() ) {
				command = "INSERT INTO units VALUES('"
				                  + escape( copyQuery.value( 0 ).toString() ) //id
				          + "','" + escape( copyQuery.value( 1 ).toString() ) //name
				          + "',NULL"                                         //name_abbrev
				          + ",'" + escape( copyQuery.value( 2 ).toString() ) //plural
				          + "',NULL"                                         //plural_abbrev
				          + ')';
				database->exec( command );

				emit progress();
			}
		}
		database->exec( "DROP TABLE units_copy" );

		database->exec("UPDATE db_info SET ver='0.92',generated_by='Krecipes SVN (20060609)'");
		if ( !database->commit() )
			kDebug()<<"Update to 0.92 failed.  Maybe you should try again.";
	}

	if ( qRound(version*100) < 93 ) {
		database->transaction();

		addColumn("CREATE TABLE %1 (id INTEGER NOT NULL, recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, amount_offset FLOAT, unit_id INTEGER, order_index INTEGER, group_id INTEGER, %2 PRIMARY KEY(id) )","substitute_for INTEGER","NULL","ingredient_list",8);

		database->exec( "CREATE index ridil_index ON ingredient_list(recipe_id)" );
		database->exec( "CREATE index iidil_index ON ingredient_list(ingredient_id)" );
		database->exec( "CREATE index gidil_index ON ingredient_list(group_id)" );

		database->exec( "UPDATE db_info SET ver='0.93',generated_by='Krecipes SVN (20060616)';" );
		database->commit();
	}

	if ( qRound(version*100) < 94 ) {
		database->transaction();

		//==================add a column to 'units' to allow specifying a type
		database->exec( "CREATE TABLE units_copy (id INTEGER NOT NULL, name VARCHAR(20), name_abbrev VARCHAR(20), plural VARCHAR(20), plural_abbrev VARCHAR(20))" );
		QSqlQuery copyQuery = database->exec( "SELECT id,name,name_abbrev,plural,plural_abbrev FROM units" );
		if ( copyQuery.isActive() ) {

			while ( copyQuery.next() ) {
				QString name_abbrev = escape( copyQuery.value( 2 ).toString() );
				if ( name_abbrev.isEmpty() )
					name_abbrev = "NULL";
				else {
					name_abbrev.prepend("'");
					name_abbrev.append("'");
				}
				QString plural_abbrev = escape( copyQuery.value( 4 ).toString() );
				if ( plural_abbrev.isEmpty() )
					plural_abbrev = "NULL";
				else {
					plural_abbrev.prepend("'");
					plural_abbrev.append("'");
				}

				command = "INSERT INTO units_copy VALUES('"
				                  + escape( copyQuery.value( 0 ).toString() ) //id
				          + "','" + escape( copyQuery.value( 1 ).toString() ) //name
				          + "'," +  name_abbrev //name_abbrev
				          + ",'" + escape( copyQuery.value( 3 ).toString() ) //plural
				          + "'," + plural_abbrev //plural_abbrev
				          + ')';
				database->exec( command );

				emit progress();
			}
		}
		database->exec( "DROP TABLE units" );
		database->exec( "CREATE TABLE units (id INTEGER NOT NULL, name VARCHAR(20), name_abbrev VARCHAR(20), plural VARCHAR(20), plural_abbrev VARCHAR(20), type INTEGER NOT NULL, PRIMARY KEY (id))" );
		copyQuery = database->exec( "SELECT id,name,name_abbrev,plural,plural_abbrev FROM units_copy" );
		if ( copyQuery.isActive() ) {

			while ( copyQuery.next() ) {
				QString name_abbrev = escape( copyQuery.value( 2 ).toString() );
				if ( name_abbrev.isEmpty() )
					name_abbrev = "NULL";
				else {
					name_abbrev.prepend("'");
					name_abbrev.append("'");
				}
				QString plural_abbrev = escape( copyQuery.value( 4 ).toString() );
				if ( plural_abbrev.isEmpty() )
					plural_abbrev = "NULL";
				else {
					plural_abbrev.prepend("'");
					plural_abbrev.append("'");
				}

				command = "INSERT INTO units VALUES('"
				                  + escape( copyQuery.value( 0 ).toString() ) //id
				          + "','" + escape( copyQuery.value( 1 ).toString() ) //name
				          + "'," + name_abbrev //name_abbrev
				          + ",'" + escape( copyQuery.value( 3 ).toString() ) //plural
				          + "'," + plural_abbrev //plural_abbrev
				          + ",'0')";
				database->exec( command );

				emit progress();
			}
		}
		database->exec( "DROP TABLE units_copy" );


		database->exec( "UPDATE db_info SET ver='0.94',generated_by='Krecipes SVN (20060712)';" );
		database->commit();
	}

	if ( qRound(version*100) < 95 ) {
		database->exec( "DROP TABLE ingredient_weights" );
		createTable( "ingredient_weights" );
		database->exec( "UPDATE db_info SET ver='0.95',generated_by='Krecipes SVN (20060726)'" );
	}

	if ( qRound(version*100) < 96 ) {
		fixUSDAPropertyUnits();
		database->exec( "UPDATE db_info SET ver='0.96',generated_by='Krecipes SVN (20060903)'" );
	}
}

void LiteRecipeDB::addColumn( const QString &new_table_sql, const QString &new_col_info, const QString &default_value, const QString &table_name, int col_index )
{
	QString command;

	command = QString(new_table_sql).arg(table_name+"_copy").arg(QString());
	kDebug()<<"calling: "<<command;
	database->exec( command );

	command = "SELECT * FROM "+table_name;
	kDebug()<<"calling: "<<command;
	QSqlQuery copyQuery = database->exec( command );
	if ( copyQuery.isActive() ) {
		while ( copyQuery.next() ) {
			QStringList dataList;
			for ( int i = 0 ;; ++i ) {
				if ( copyQuery.value(i).isNull() )
					break;

				QString data = copyQuery.value(i).toString();

				dataList << '\''+escape(data)+'\'';
			}
			command = "INSERT INTO "+table_name+"_copy VALUES("+dataList.join(",")+");";
			kDebug()<<"calling: "<<command;
			database->exec( command );

			emit progress();
		}
	}
	database->exec( "DROP TABLE "+table_name );
	database->exec( QString(new_table_sql).arg(table_name).arg(new_col_info+',') );
	copyQuery = database->exec( "SELECT * FROM "+table_name+"_copy" );
	if ( copyQuery.isActive() ) {
		while ( copyQuery.next() ) {
			QStringList dataList;
			for ( int i = 0 ;; ++i ) {
				if ( i == col_index )
					dataList << default_value;

				if ( copyQuery.value(i).isNull() )
					break;

				QString data = copyQuery.value(i).toString();

				dataList << '\''+escape(data)+'\'';
			}
			command = "INSERT INTO "+table_name+" VALUES(" +dataList.join(",")+')';
			kDebug()<<"calling: "<<command;
			database->exec( command );

			emit progress();
		}
	}
	database->exec( "DROP TABLE "+table_name+"_copy" );
}

QString LiteRecipeDB::escapeAndEncode( const QString &s ) const
{
	QString s_escaped;

	// Escape
	s_escaped = escape( QString::fromLatin1(s.toUtf8()) );

	// Return encoded
	return s_escaped.toLatin1(); // Note that the text has already been converted before escaping.
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
int sqlite_decode_binary( const unsigned char *in, unsigned char *out )
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

QString escape( const QString &s )
{
	QString s_escaped = s;

	if ( !s_escaped.isEmpty() ) { //###: sqlite_mprintf() seems to fill an empty string with garbage
		// Escape using SQLite's function
#ifdef HAVE_SQLITE
		char * escaped = sqlite_mprintf( "%q", s.toLatin1().data() ); // Escape the string(allocates memory)
#elif HAVE_SQLITE3
		char * escaped = sqlite3_mprintf( "%q", s.toLatin1().data() ); // Escape the string(allocates memory)
#endif
		s_escaped = escaped;
#ifdef HAVE_SQLITE
		sqlite_freemem( escaped ); // free allocated memory
#elif HAVE_SQLITE3
		sqlite3_free( escaped ); // free allocated memory
#endif
	}

	return ( s_escaped );
}

void LiteRecipeDB::storePhoto( int recipeID, const QByteArray &data )
{
	QSqlQuery query( QString(), *database);

	query.prepare( "UPDATE recipes SET photo=\"?\",ctime=ctime,atime=atime,mtime=mtime WHERE id=" + QString::number( recipeID ) );
	query.addBindValue( KCodecs::base64Encode( data ) );
	query.exec();
}
#include "literecipedb.moc"
