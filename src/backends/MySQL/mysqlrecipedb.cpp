/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003, 2006 Jason Kivlighn <jkivlighn@gmail.com>           *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "mysqlrecipedb.h"

#include <kdebug.h>
#include <kstandarddirs.h>
#include <ktemporaryfile.h>
#include <KConfigGroup>
#include <klocale.h>
#include <kconfig.h>
#include <kglobal.h>
#include <QSqlQuery>
#include <QSqlError>

MySQLRecipeDB::MySQLRecipeDB( const QString &host, const QString &user, const QString &pass, const QString &DBname, int port ) : QSqlRecipeDB( host, user, pass, DBname, port )
{}

MySQLRecipeDB::~MySQLRecipeDB()
{}

void MySQLRecipeDB::createDB()
{
	QString real_db_name = database->databaseName();

	//we have to be connected to some database in order to create the Krecipes database
	//so long as the permissions given are allowed access to "mysql', this works
	database->setDatabaseName( "mysql" );
	if ( database->open() ) {
		// Create the Database (Note: needs permissions)
		//FIXME: I've noticed certain characters cause this to fail (such as '-').  Somehow let the user know.
		QSqlQuery query( QString( "CREATE DATABASE %1" ).arg( real_db_name ), *database );
		if ( !query.isActive() )
			kDebug() << "create query failed: " << database->lastError().databaseText() ;

		database->close();
	}
	else
		kDebug() << "create open failed: " << database->lastError().databaseText() ;

	database->setDatabaseName( real_db_name );
}

QStringList MySQLRecipeDB::backupCommand() const
{
	KConfigGroup config( KGlobal::config(), "Server");

	QStringList command;
	command<<config.readEntry( "MySQLDumpPath", "mysqldump" )<<"-q";

	QString pass = config.readEntry("Password", QString());
	if ( !pass.isEmpty() )
		command<<"-p"+pass;

	QString user = config.readEntry("Username", QString());
	command<<"-u"+user;

	command<<"-h"+config.readEntry("Host", "localhost");

	int port = config.readEntry("Port", 0);
	if ( port > 0 )
		command<<"-P"+QString::number(port);

	command<<database->databaseName();
	return command;
}

QStringList MySQLRecipeDB::restoreCommand() const
{
	KConfigGroup config( KGlobal::config(), "Server");

	QStringList command;
	command<<config.readEntry( "MySQLPath", "mysql" );

	QString pass = config.readEntry("Password", QString());
	if ( !pass.isEmpty() )
		command<<"-p"+pass;

	QString user = config.readEntry("Username", QString());
	command<<"-u"+user;

	int port = config.readEntry("Port", 0);
	if ( port > 0 )
		command<<"-P"+QString::number(port);

	command<<"-h"+config.readEntry("Host", "localhost");

	command<<database->databaseName();
	return command;
}

void MySQLRecipeDB::createTable( const QString &tableName )
{

	QStringList commands;

	if ( tableName == "recipes" )
		commands << QString( "CREATE TABLE recipes (id INTEGER NOT NULL AUTO_INCREMENT,title VARCHAR(%1), yield_amount FLOAT, yield_amount_offset FLOAT, yield_type_id int(11) DEFAULT '-1', instructions TEXT, photo BLOB, prep_time TIME, ctime TIMESTAMP, mtime TIMESTAMP, atime TIMESTAMP,  PRIMARY KEY (id));" ).arg( maxRecipeTitleLength() );

	else if ( tableName == "ingredients" )
		commands << QString( "CREATE TABLE ingredients (id INTEGER NOT NULL AUTO_INCREMENT, name VARCHAR(%1), PRIMARY KEY (id));" ).arg( maxIngredientNameLength() );

	else if ( tableName == "ingredient_list" )
		commands << "CREATE TABLE ingredient_list (id INTEGER NOT NULL AUTO_INCREMENT, recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, amount_offset FLOAT, unit_id INTEGER, order_index INTEGER, group_id INTEGER, substitute_for INTEGER, PRIMARY KEY(id), INDEX ridil_index(recipe_id), INDEX iidil_index(ingredient_id), INDEX gidil_index(group_id))";

	else if ( tableName == "unit_list" )
		commands << "CREATE TABLE unit_list (ingredient_id INTEGER, unit_id INTEGER);";

	else if ( tableName == "units" )
		commands << QString( "CREATE TABLE units (id INTEGER NOT NULL AUTO_INCREMENT, name VARCHAR(%1), name_abbrev VARCHAR(%2), plural VARCHAR(%3), plural_abbrev VARCHAR(%4), type INTEGER NOT NULL DEFAULT 0, PRIMARY KEY (id));" )
		   .arg( maxUnitNameLength() ).arg( maxUnitNameLength() ).arg( maxUnitNameLength() ).arg( maxUnitNameLength() );

	else if ( tableName == "prep_methods" )
		commands << QString( "CREATE TABLE prep_methods (id INTEGER NOT NULL AUTO_INCREMENT, name VARCHAR(%1), PRIMARY KEY (id));" ).arg( maxPrepMethodNameLength() );

	else if ( tableName == "prep_method_list" )
		commands << "CREATE TABLE prep_method_list (ingredient_list_id int(11) NOT NULL,prep_method_id int(11) NOT NULL, order_index int(11), INDEX  iid_index (ingredient_list_id), INDEX pid_index (prep_method_id));";

	else if ( tableName == "ingredient_info" )
		commands << "CREATE TABLE ingredient_info (ingredient_id INTEGER, property_id INTEGER, amount FLOAT, per_units INTEGER);";

	else if ( tableName == "ingredient_properties" )
		commands << QString( "CREATE TABLE ingredient_properties "
			"(id INTEGER NOT NULL AUTO_INCREMENT, "
			"name VARCHAR(%1), units VARCHAR(%2), "
			"PRIMARY KEY (id));" 
			).arg( maxPropertyNameLength() ).arg( maxUnitNameLength() ) ;

	else if ( tableName == "ingredient_weights" )
		commands << "CREATE TABLE ingredient_weights (id INTEGER NOT NULL AUTO_INCREMENT, ingredient_id INTEGER NOT NULL, amount FLOAT, unit_id INTEGER, weight FLOAT, weight_unit_id INTEGER, prep_method_id INTEGER, PRIMARY KEY (id), INDEX(ingredient_id), INDEX(unit_id), INDEX(weight_unit_id), INDEX(prep_method_id) );";

	else if ( tableName == "units_conversion" )
		commands << "CREATE TABLE units_conversion (unit1_id INTEGER, unit2_id INTEGER, ratio FLOAT);";

	else if ( tableName == "categories" )
		commands << QString( "CREATE TABLE categories (id int(11) NOT NULL auto_increment, name varchar(%1) default NULL, parent_id int(11) NOT NULL default -1, PRIMARY KEY (id), INDEX parent_id_index(parent_id));" ).arg( maxCategoryNameLength() );

	else if ( tableName == "category_list" )
		commands << "CREATE TABLE category_list (recipe_id int(11) NOT NULL,category_id int(11) NOT NULL, INDEX  rid_index (recipe_id), INDEX cid_index (category_id));";

	else if ( tableName == "authors" )
		commands << QString( "CREATE TABLE authors (id int(11) NOT NULL auto_increment, name varchar(%1) default NULL,PRIMARY KEY (id));" ).arg( maxAuthorNameLength() );

	else if ( tableName == "author_list" )
		commands << "CREATE TABLE author_list (recipe_id int(11) NOT NULL,author_id int(11) NOT NULL);";

	else if ( tableName == "db_info" ) {
		commands << "CREATE TABLE db_info (ver FLOAT NOT NULL,generated_by varchar(200) default NULL);";
		commands << QString( "INSERT INTO db_info VALUES(%1,'Krecipes %2');" ).arg( latestDBVersion() ).arg( krecipes_version() );
	}
	else if ( tableName == "ingredient_groups" ) {
		commands << QString( "CREATE TABLE `ingredient_groups` (`id` int(11) NOT NULL auto_increment, `name` varchar(%1), PRIMARY KEY (`id`));" ).arg( maxIngGroupNameLength() );
	}
	else if ( tableName == "yield_types" ) {
		commands << QString( "CREATE TABLE 'yield_types' "
			"('id' int(11) NOT NULL auto_increment, "
			"'name' VARCHAR(%1), "
			"PRIMARY KEY ('id'));" ).arg( maxYieldTypeLength() );
	}

	else if ( tableName == "ratings" )
		commands << "CREATE TABLE ratings (id INTEGER NOT NULL AUTO_INCREMENT, recipe_id int(11) NOT NULL, comment TEXT, rater TEXT, created TIMESTAMP, PRIMARY KEY (id));";

	else if ( tableName == "rating_criteria" )
		commands << "CREATE TABLE rating_criteria (id INTEGER NOT NULL AUTO_INCREMENT, name TEXT, PRIMARY KEY (id));";

	else if ( tableName == "rating_criterion_list" )
		commands << "CREATE TABLE rating_criterion_list (rating_id INTEGER NOT NULL, rating_criterion_id INTEGER, stars FLOAT);";

	else
		return ;

	commands << QString( "ALTER TABLE %1 DEFAULT CHARSET UTF8" ).arg( tableName ) ;
	commands << QString( "ALTER TABLE %1 CONVERT TO CHARACTER SET UTF8" ).arg( tableName );

	QSqlQuery databaseToCreate( QString(), *database );

	// execute the queries
	for ( QStringList::const_iterator it = commands.constBegin(); it != commands.constEnd(); ++it )
		databaseToCreate.exec( ( *it ) );
}

int MySQLRecipeDB::maxAuthorNameLength() const
{
	return 50;
}

int MySQLRecipeDB::maxCategoryNameLength() const
{
	return 40;
}

int MySQLRecipeDB::maxIngredientNameLength() const
{
	return 50;
}

int MySQLRecipeDB::maxIngGroupNameLength() const
{
	return 50;
}

int MySQLRecipeDB::maxRecipeTitleLength() const
{
	return 200;
}

int MySQLRecipeDB::maxUnitNameLength() const
{
	return 20;
}

int MySQLRecipeDB::maxPrepMethodNameLength() const
{
	return 20;
}

int MySQLRecipeDB::maxPropertyNameLength() const
{
	return 20;
}

int MySQLRecipeDB::maxYieldTypeLength() const 
{
	return 20;
}

void MySQLRecipeDB::portOldDatabases( float version )
{
	kDebug() << "Current database version is..." << version;
	QString command;

	// Note that version no. means the version in which this DB structure
	// was introduced.  To work with SVN users, the database will be incrementally
	// upgraded for each change made between releases (e.g. 0.81, 0.82,... are
	// what will become 0.9)

	if ( qRound(version*10) < 3 ) 	// The database was generated with a version older than v 0.3. First update to 0.3 version
	{

		// Add new columns to existing tables (creating new tables is not necessary. Integrity check does that before)
		command = "ALTER TABLE recipes ADD COLUMN persons int(11) AFTER title;";
		QSqlQuery tableToAlter( command, *database );

		// Set the version to the new one (0.3)

		command = "DELETE FROM db_info;"; // Remove previous version records if they exist
		tableToAlter.exec( command );
		command = "INSERT INTO db_info VALUES(0.3,'Krecipes 0.4');"; // Set the new version
		tableToAlter.exec( command );
	}

	if ( qRound(version*10) < 4 )   // Upgrade to the current DB version 0.4
	{

		// Add new columns to existing tables (creating any new tables is not necessary. Integrity check does that before)
		command = "ALTER TABLE ingredient_list ADD COLUMN order_index int(11) AFTER unit_id;";
		QSqlQuery tableToAlter( command, *database );

		// Missing indexes in the previous versions
		command = "CREATE index rid_index ON category_list(recipe_id)";
		tableToAlter.exec( command );

		command = "CREATE index cid_index ON category_list(category_id)";
		tableToAlter.exec( command );

		command = "CREATE index ridil_index ON ingredient_list(recipe_id)";
		tableToAlter.exec( command );

		command = "CREATE index iidil_index ON ingredient_list(ingredient_id)";
		tableToAlter.exec( command );

		// Port data

		//*1:: Recipes have always category -1 to speed up searches (no JOINs needed)
		command = "SELECT r.id FROM recipes r;"; // Find all recipes
		QSqlQuery categoryToAdd( QString(), *database );
		tableToAlter.exec( command );
		if ( tableToAlter.isActive() )
		{
			while ( tableToAlter.next() ) {
				int recipeId = tableToAlter.value( 0 ).toInt();
				QString cCommand = QString( "INSERT INTO category_list VALUES (%1,-1);" ).arg( recipeId );
				categoryToAdd.exec( cCommand );

				emit progress();
			}
		}

		// Set the version to the new one (0.4)

		command = "DELETE FROM db_info;"; // Remove previous version records if they exist
		tableToAlter.exec( command );
		command = "INSERT INTO db_info VALUES(0.4,'Krecipes 0.4');"; // Set the new version
		tableToAlter.exec( command );
	}

	if ( qRound(version*10) < 5 ) {
		command = QString( "CREATE TABLE prep_methods (id INTEGER NOT NULL AUTO_INCREMENT, name VARCHAR(%1), PRIMARY KEY (id));" ).arg( maxPrepMethodNameLength() );
		QSqlQuery tableToAlter( command, *database );

		command = "ALTER TABLE ingredient_list ADD COLUMN prep_method_id int(11) AFTER unit_id;";
		tableToAlter.exec( command );
		command = "UPDATE ingredient_list SET prep_method_id=-1 WHERE prep_method_id IS NULL;";
		tableToAlter.exec( command );

		command = "ALTER TABLE authors MODIFY name VARCHAR(50);";
		tableToAlter.exec( command );
		command = "ALTER TABLE categories MODIFY name VARCHAR(40);";
		tableToAlter.exec( command );

		// Set the version to the new one (0.5)
		command = "DELETE FROM db_info;"; // Remove previous version records if they exist
		tableToAlter.exec( command );
		command = "INSERT INTO db_info VALUES(0.5,'Krecipes 0.5');";
		tableToAlter.exec( command );
	}

	if ( qRound(version*10) < 6 ) {
		command = "ALTER TABLE categories ADD COLUMN parent_id int(11) NOT NULL default '-1' AFTER name;";
		QSqlQuery tableToAlter( command, *database );

		command = "DELETE FROM db_info;"; // Remove previous version records if they exist
		tableToAlter.exec( command );
		command = "INSERT INTO db_info VALUES(0.6,'Krecipes 0.6');";
		tableToAlter.exec( command );
	}

	if ( qRound(version*100) < 61 ) {
		QString command = "ALTER TABLE `recipes` ADD COLUMN `prep_time` TIME DEFAULT NULL";
		QSqlQuery tableToAlter( command, *database );

		command = "DELETE FROM db_info;"; // Remove previous version records if they exist
		tableToAlter.exec( command );
		command = "INSERT INTO db_info VALUES(0.61,'Krecipes 0.6');";
		tableToAlter.exec( command );
	}

	if ( qRound(version*100) < 62 ) {
		QString command = "ALTER TABLE `ingredient_list` ADD COLUMN `group_id` int(11) default '-1' AFTER order_index;";
		QSqlQuery tableToAlter( command, *database );

		command = "DELETE FROM db_info;"; // Remove previous version records if they exist
		tableToAlter.exec( command );
		command = "INSERT INTO db_info VALUES(0.62,'Krecipes 0.7');";
		tableToAlter.exec( command );
	}

	if ( qRound(version*100) < 63 ) {
		QString command = "ALTER TABLE `units` ADD COLUMN `plural` varchar(20) DEFAULT NULL AFTER name;";
		QSqlQuery tableToAlter( command, *database );

		QSqlQuery result( "SELECT id,name FROM units WHERE plural IS NULL", *database );
		if ( result.isActive() ) {
			while ( result.next() ) {
				command = "UPDATE units SET plural='" + result.value( 1 ).toString() + "' WHERE id=" + QString::number( result.value( 0 ).toInt() );
				QSqlQuery query( command, *database );

				emit progress();
			}
		}

		command = "DELETE FROM db_info;"; // Remove previous version records if they exist
		tableToAlter.exec( command );
		command = "INSERT INTO db_info VALUES(0.63,'Krecipes 0.7');";
		tableToAlter.exec( command );
	}

	if ( qRound(version*10) < 7 ) { //simply call 0.63 -> 0.7
		QString command = "UPDATE db_info SET ver='0.7';";
		QSqlQuery query( command, *database );
	}

	if ( qRound(version*100) < 81 ) {
		QString command = "ALTER TABLE `ingredient_list` ADD COLUMN `amount_offset` FLOAT DEFAULT '0' AFTER amount;";
		QSqlQuery tableToAlter( command, *database );

		command = "UPDATE db_info SET ver='0.81',generated_by='Krecipes SVN (20050816)';";
		tableToAlter.exec( command );
	}

	if ( qRound(version*100) < 82 ) {
		QString command = "ALTER TABLE `recipes` ADD COLUMN `yield_amount` FLOAT DEFAULT '0' AFTER persons;";
		QSqlQuery tableToAlter( command, *database );

		command = "ALTER TABLE `recipes` ADD COLUMN `yield_amount_offset` FLOAT DEFAULT '0' AFTER yield_amount;";
		tableToAlter.exec(command);

		command = "ALTER TABLE `recipes` ADD COLUMN `yield_type_id` INTEGER DEFAULT '-1' AFTER yield_amount_offset;";
		tableToAlter.exec(command);

		QSqlQuery result( "SELECT id,persons FROM recipes", *database );
		if ( result.isActive() ) {
			while ( result.next() ) {
				command = "UPDATE recipes SET yield_amount='" + QString::number( result.value( 1 ).toInt() ) + "' WHERE id=" + QString::number( result.value( 0 ).toInt() );
				QSqlQuery query( command, *database );

				emit progress();
			}
		}

		command = "ALTER TABLE `recipes` DROP COLUMN `persons`;";
		tableToAlter.exec( command );

		command = "UPDATE db_info SET ver='0.82',generated_by='Krecipes SVN (20050902)';";
		tableToAlter.exec( command );
	}

	if ( qRound(version*100) < 83 ) {
		database->transaction();

		//====add a id columns to 'ingredient_list' to identify it for the prep method list
		database->exec( "RENAME TABLE ingredient_list TO ingredient_list_copy;" );
		database->exec( "CREATE TABLE ingredient_list (id INTEGER NOT NULL AUTO_INCREMENT, recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, amount_offset FLOAT, unit_id INTEGER, order_index INTEGER, group_id INTEGER, PRIMARY KEY(id), INDEX ridil_index(recipe_id), INDEX iidil_index(ingredient_id));" );

		QSqlQuery copyQuery = database->exec( "SELECT recipe_id,ingredient_id,amount,amount_offset,unit_id,prep_method_id,order_index,group_id FROM ingredient_list_copy" );
		if ( copyQuery.isActive() ) {
			while ( copyQuery.next() ) {
				QSqlQuery query(QString(),*database);
				query.prepare( "INSERT INTO ingredient_list VALUES (NULL, ?, ?, ?, ?, ?, ?, ?)" );
				query.addBindValue( copyQuery.value( 0 ) );
				query.addBindValue( copyQuery.value( 1 ) );
				query.addBindValue( copyQuery.value( 2 ) );
				query.addBindValue( copyQuery.value( 3 ) );
				query.addBindValue( copyQuery.value( 4 ) );
				query.addBindValue( copyQuery.value( 6 ) );
				query.addBindValue( copyQuery.value( 7 ) );
				query.exec();

				int prep_method_id = copyQuery.value( 5 ).toInt();
				if ( prep_method_id != -1 ) {
					query.prepare( "INSERT INTO prep_method_list VALUES (?, ?, ?);" );
					query.addBindValue( lastInsertId(query) );
					query.addBindValue( prep_method_id );
					query.addBindValue( 1 );
					query.exec();
				}

				emit progress();
			}
		}
		database->exec( "DROP TABLE ingredient_list_copy" );

		database->exec( "UPDATE db_info SET ver='0.83',generated_by='Krecipes SVN (20050909)';" );

		if ( !database->commit() )
			kDebug()<<"Update to 0.83 failed.  Maybe you should try again.";
	}

	if ( qRound(version*100) < 84 ) {
		database->transaction();

		database->exec( "ALTER TABLE recipes ADD COLUMN ctime TIMESTAMP;" );
		database->exec( "ALTER TABLE recipes ADD COLUMN mtime TIMESTAMP;" );
		database->exec( "ALTER TABLE recipes ADD COLUMN atime TIMESTAMP;" );

		database->exec( "UPDATE recipes SET ctime=CURRENT_TIMESTAMP, mtime=CURRENT_TIMESTAMP, atime=CURRENT_TIMESTAMP;" );

		database->exec( "UPDATE db_info SET ver='0.84',generated_by='Krecipes SVN (20050913)';" );

		if ( !database->commit() )
			kDebug()<<"Update to 0.84 failed.  Maybe you should try again.";
	}

	if ( qRound(version*100) < 85 ) {
		database->transaction();

		QSqlQuery query( "SELECT id,photo FROM recipes", *database );

		if ( query.isActive() ) {
			while ( query.next() ) {
				storePhoto( query.value(0).toInt(), query.value(1).toByteArray() );

				emit progress();
			}
		}


		database->exec( "UPDATE db_info SET ver='0.85',generated_by='Krecipes SVN (20050926)';" );
		if ( !database->commit() )
			kDebug()<<"Update to 0.85 failed.  Maybe you should try again.";
	}

	if ( qRound(version*100) < 86 ) {
		database->transaction();

		database->exec( "ALTER TABLE ingredient_list ADD INDEX (group_id)" );

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

		database->exec( "UPDATE db_info SET ver='0.86',generated_by='Krecipes SVN (20050928)';" );
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

		database->exec( "ALTER TABLE units ADD COLUMN name_abbrev VARCHAR(20) AFTER name");
		database->exec( "ALTER TABLE units ADD COLUMN plural_abbrev VARCHAR(20) AFTER plural");

		database->exec("UPDATE db_info SET ver='0.92',generated_by='Krecipes SVN (20060609)'");
		if ( !database->commit() )
			kDebug()<<"Update to 0.92 failed.  Maybe you should try again.";
	}

	if ( qRound(version*100) < 93 ) {
		database->transaction();

		database->exec( "ALTER TABLE ingredient_list ADD COLUMN substitute_for INTEGER AFTER group_id");

		database->exec("UPDATE db_info SET ver='0.93',generated_by='Krecipes SVN (20060615)'");
		if ( !database->commit() )
			kDebug()<<"Update to 0.93 failed.  Maybe you should try again.";
	}

	if ( qRound(version*100) < 94 ) {
		database->transaction();

		database->exec( "ALTER TABLE units ADD COLUMN type INTEGER NOT NULL DEFAULT 0 AFTER plural_abbrev");

		database->exec("UPDATE db_info SET ver='0.94',generated_by='Krecipes SVN (20060712)'");
		if ( !database->commit() )
			kDebug()<<"Update to 0.94 failed.  Maybe you should try again.";
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

	if ( qRound(version*100) < 97 ) {
		QStringList tables;
		tables << "author_list" << "authors" << "categories" 
			<< "category_list" << "db_info" << "ingredient_groups"
			<< "ingredient_info" << "ingredient_list" << "ingredient_properties"
			<< "ingredient_weights" << "ingredients" << "prep_method_list"
			<< "prep_methods" << "rating_criteria" << "rating_criterion_list"
			<< "ratings" << "recipes" << "unit_list" << "units"
			<< "units_conversion" << "yield_types";
		QStringList::const_iterator it;
		for ( it = tables.constBegin(); it != tables.constEnd(); it++ ) {
			QString command = QString( "ALTER TABLE %1 DEFAULT CHARSET UTF8" ).arg( *it );
			database->exec( command );
			command = QString( "ALTER TABLE %1 CONVERT TO CHARACTER SET UTF8" ).arg( *it );
			database->exec( command );
		}
		database->exec( "UPDATE db_info SET ver='0.97',generated_by='Krecipes 2.0.0'" );
	}
}

void MySQLRecipeDB::givePermissions( const QString &dbName, const QString &username, const QString &password, const QString &clientHost )
{
	QString command;

	if ( !password.isEmpty() )
		command = QString( "GRANT ALL ON %1.* TO '%2'@'%3' IDENTIFIED BY '%4';" ).arg( dbName ).arg( username ).arg( clientHost ).arg( password );
	else
		command = QString( "GRANT ALL ON %1.* TO '%2'@'%3';" ).arg( dbName ).arg( username ).arg( clientHost );

	kDebug() << "I'm doing the query to setup permissions";

	QSqlQuery permissionsToSet( command, *database );
}

#include "mysqlrecipedb.moc"
