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

#include "psqlrecipedb.h"

#include <kdebug.h>
#include <kstandarddirs.h>
#include <ktemporaryfile.h>
#include <klocale.h>
#include <kconfiggroup.h>

#include <QVariant>
#include <QSqlQuery>
#include <QSqlError>

//Note: PostgreSQL's database names are always lowercase
PSqlRecipeDB::PSqlRecipeDB( const QString& host, const QString& user, const QString& pass, const QString& DBname, int port ) : QSqlRecipeDB( host, user, pass, DBname.toLower(), port )
{}

PSqlRecipeDB::~PSqlRecipeDB()
{}

void PSqlRecipeDB::createDB()
{
	QString real_db_name = database->databaseName();

	//we have to be connected to some database in order to create the Krecipes database
	//so long as the permissions given are allowed access to "template1', this works
	database->setDatabaseName( "template1" );
	if ( database->open() ) {
		QSqlQuery query( QString( "CREATE DATABASE %1" ).arg( real_db_name ), *database );
		if ( !query.isActive() )
			kDebug() << "create query failed: " << database->lastError().databaseText() ;

		database->close();
	}
	else
		kDebug() << "create open failed: " << database->lastError().databaseText() ;

	database->setDatabaseName( real_db_name );
}

QStringList PSqlRecipeDB::backupCommand() const
{
	KConfigGroup config = KGlobal::config()->group("Server");

	QStringList command;
	command<<config.readEntry( "PgDumpPath", "pg_dump" )<<"--inserts"<<database->databaseName()
	  <<"-U"<<config.readEntry( "Username" );

	int port = config.readEntry( "Port", 0 );
	if ( port > 0 )
		command<<"-p"<<QString::number(port);

	return command;
}

QStringList PSqlRecipeDB::restoreCommand() const
{
	KConfigGroup config = KGlobal::config()->group("Server");

	QStringList command;
	command<<config.readEntry( "PsqlPath", "psql" )<<database->databaseName()
	  <<"-U"<<config.readEntry( "Username" );

	int port = config.readEntry( "Port", 0 );
	if ( port > 0 )
		command<<"-p"<<QString::number(port);

	return command;
}

void PSqlRecipeDB::createTable( const QString &tableName )
{

	QStringList commands;

	if ( tableName == "recipes" )
		commands << "CREATE TABLE recipes (id SERIAL NOT NULL PRIMARY KEY,title CHARACTER VARYING, yield_amount FLOAT, yield_amount_offset FLOAT, yield_type_id INTEGER DEFAULT '-1', instructions TEXT, photo TEXT, prep_time TIME, ctime TIMESTAMP, mtime TIMESTAMP, atime TIMESTAMP );";

	else if ( tableName == "ingredients" )
		commands << "CREATE TABLE ingredients (id SERIAL NOT NULL PRIMARY KEY, name CHARACTER VARYING);";

	else if ( tableName == "ingredient_list" ) {
		commands << "CREATE TABLE ingredient_list (id SERIAL NOT NULL PRIMARY KEY, recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, amount_offset FLOAT, unit_id INTEGER, order_index INTEGER, group_id INTEGER, substitute_for INTEGER);";
		commands << "CREATE INDEX ridil_index ON ingredient_list USING BTREE (recipe_id);";
		commands << "CREATE INDEX iidil_index ON ingredient_list USING BTREE (ingredient_id);";
		commands << "CREATE INDEX gidil_index ON ingredient_list USING BTREE (group_id);";
	}

	else if ( tableName == "unit_list" )
		commands << "CREATE TABLE unit_list (ingredient_id INTEGER, unit_id INTEGER);";

	else if ( tableName == "units" )
		commands << "CREATE TABLE units (id SERIAL NOT NULL PRIMARY KEY, name CHARACTER VARYING, name_abbrev CHARACTER VARYING, plural CHARACTER VARYING, plural_abbrev CHARACTER VARYING, type INTEGER NOT NULL DEFAULT '0' );";

	else if ( tableName == "prep_methods" )
		commands << "CREATE TABLE prep_methods (id SERIAL NOT NULL PRIMARY KEY, name CHARACTER VARYING);";
	else if ( tableName == "prep_method_list" ) {
		commands << "CREATE TABLE prep_method_list (ingredient_list_id INTEGER NOT NULL,prep_method_id INTEGER NOT NULL, order_index INTEGER);";
		commands << "CREATE INDEX iid_index ON prep_method_list USING BTREE (ingredient_list_id);";
		commands << "CREATE INDEX pid_index ON prep_method_list USING BTREE (prep_method_id);";
	}
	else if ( tableName == "ingredient_info" )
		commands << "CREATE TABLE ingredient_info (ingredient_id INTEGER, property_id INTEGER, amount FLOAT, per_units INTEGER);";

	else if ( tableName == "ingredient_properties" )
		commands << "CREATE TABLE ingredient_properties (id SERIAL NOT NULL,name CHARACTER VARYING, units CHARACTER VARYING);";

	else if ( tableName == "ingredient_weights" ) {
		commands << "CREATE TABLE ingredient_weights (id SERIAL NOT NULL PRIMARY KEY, ingredient_id INTEGER NOT NULL, amount FLOAT, unit_id INTEGER, weight FLOAT, weight_unit_id INTEGER, prep_method_id INTEGER );"
		  << "CREATE INDEX weight_wid_index ON ingredient_weights USING BTREE (weight_unit_id)"
		  << "CREATE INDEX weight_pid_index ON ingredient_weights USING BTREE (prep_method_id)"
		  << "CREATE INDEX weight_uid_index ON ingredient_weights USING BTREE (unit_id)"
		  << "CREATE INDEX weight_iid_index ON ingredient_weights USING BTREE (ingredient_id)";
	}

	else if ( tableName == "units_conversion" )
		commands << "CREATE TABLE units_conversion (unit1_id INTEGER, unit2_id INTEGER, ratio FLOAT);";

	else if ( tableName == "categories" ) {
		commands << "CREATE TABLE categories (id SERIAL NOT NULL PRIMARY KEY, name CHARACTER VARYING default NULL, parent_id INTEGER NOT NULL default -1);";
		commands << "CREATE index parent_id_index ON categories USING BTREE(parent_id);";
	}
	else if ( tableName == "category_list" ) {
		commands << "CREATE TABLE category_list (recipe_id INTEGER NOT NULL,category_id INTEGER NOT NULL);";
		commands << "CREATE INDEX rid_index ON category_list USING BTREE (recipe_id);";
		commands << "CREATE INDEX cid_index ON category_list USING BTREE (category_id);";
	}

	else if ( tableName == "authors" )
		commands << "CREATE TABLE authors (id SERIAL NOT NULL PRIMARY KEY, name CHARACTER VARYING default NULL);";

	else if ( tableName == "author_list" )
		commands << "CREATE TABLE author_list (recipe_id INTEGER NOT NULL,author_id INTEGER NOT NULL);";

	else if ( tableName == "db_info" ) {
		commands << "CREATE TABLE db_info (ver FLOAT NOT NULL,generated_by CHARACTER VARYING default NULL);";
		commands << QString( "INSERT INTO db_info VALUES(%1,'Krecipes %2');" ).arg( latestDBVersion() ).arg( krecipes_version() );
	}
	else if ( tableName == "ingredient_groups" ) {
		commands << "CREATE TABLE ingredient_groups (id SERIAL NOT NULL PRIMARY KEY, name CHARACTER VARYING);";
	}
	else if ( tableName == "yield_types" ) {
		commands << "CREATE TABLE yield_types (id SERIAL NOT NULL PRIMARY KEY, name CHARACTER VARYING);";
	}

	else if ( tableName == "ratings" )
		commands << "CREATE TABLE ratings (id SERIAL NOT NULL PRIMARY KEY, recipe_id INTEGER NOT NULL, comment CHARACTER VARYING, rater CHARACTER VARYING, created TIMESTAMP);";

	else if ( tableName == "rating_criteria" )
		commands << "CREATE TABLE rating_criteria (id SERIAL NOT NULL PRIMARY KEY, name CHARACTER VARYING);";

	else if ( tableName == "rating_criterion_list" )
		commands << "CREATE TABLE rating_criterion_list (rating_id INTEGER NOT NULL, rating_criterion_id INTEGER, stars FLOAT);";
	else
		return ;

	QSqlQuery databaseToCreate( QString(), *database );

	// execute the queries
	for ( QStringList::const_iterator it = commands.constBegin(); it != commands.constEnd(); ++it )
		databaseToCreate.exec( *it );
}

int PSqlRecipeDB::maxAuthorNameLength() const
{
	return RecipeDB::UnlimitedLength;
}

int PSqlRecipeDB::maxCategoryNameLength() const
{
	return RecipeDB::UnlimitedLength;
}

int PSqlRecipeDB::maxIngredientNameLength() const
{
	return RecipeDB::UnlimitedLength;
}

int PSqlRecipeDB::maxIngGroupNameLength() const
{
	return RecipeDB::UnlimitedLength;
}

int PSqlRecipeDB::maxRecipeTitleLength() const
{
	return RecipeDB::UnlimitedLength;
}

int PSqlRecipeDB::maxUnitNameLength() const
{
	return RecipeDB::UnlimitedLength;
}

int PSqlRecipeDB::maxPrepMethodNameLength() const
{
	return RecipeDB::UnlimitedLength;
}

int PSqlRecipeDB::maxPropertyNameLength() const
{
	return RecipeDB::UnlimitedLength;
}

int PSqlRecipeDB::maxYieldTypeLength() const 
{
	return RecipeDB::UnlimitedLength;
}


void PSqlRecipeDB::initializeData()
{
	QSqlRecipeDB::initializeData();

	QSqlQuery updateSeq( "SELECT setval('units_id_seq',(SELECT COUNT(1) FROM units))", *database );
	updateSeq.exec( "SELECT setval('categories_id_seq',(SELECT COUNT(1) FROM categories))" );
}

float PSqlRecipeDB::databaseVersion( void )
{
	/* We're overriding this method to workaround a bug in the PostgreSQL
	driver in Qt 4.5.1; for some reason "select ver from db_info" returns
	zero but typing the same query on the psql prompt returns the proper
	float number.*/
	kDebug();
	QString command = "SELECT cast(ver as varchar) FROM db_info";
	QSqlQuery dbVersion( command, *database);
	kDebug()<<"dbVersion.isActive():"<< dbVersion.isActive()<<"database:"<<database;
	kDebug()<<"dbVersion.isSelect():"<<dbVersion.isSelect();
	if ( dbVersion.isActive() && dbVersion.isSelect() && dbVersion.next() )
	{
		kDebug()<<"dbVersion.value( 0 ).toString().toDouble() :"<<dbVersion.value( 0 ).toString().toDouble();
		return ( dbVersion.value( 0 ).toString().toDouble() ); // There should be only one (or none for old DB) element, so go to first
	}
	else {
		kDebug()<<" old version";
		return ( 0.2 ); // if table is empty, assume oldest (0.2), and port
	}
}

void PSqlRecipeDB::portOldDatabases( float version )
{
	kDebug() << "Current database version is..." << version;
	QString command;

	if ( qRound(version*10) < 7 ) {
		//version added
	}

	if ( qRound(version*100) < 81 ) {
		database->transaction();

		addColumn("CREATE TABLE %1 (recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, %2 unit_id INTEGER, prep_method_id INTEGER, order_index INTEGER, group_id INTEGER);","amount_offset FLOAT","'0'","ingredient_list",3);

		QSqlQuery query( QString(), *database );
		query.exec( "CREATE INDEX ridil_index ON ingredient_list USING BTREE (recipe_id);" );
		query.exec( "CREATE INDEX iidil_index ON ingredient_list USING BTREE (ingredient_id);");

		query.exec( "UPDATE db_info SET ver='0.81',generated_by='Krecipes SVN (20050816)';" );

		if ( !database->commit() )
			kDebug()<<"Update to 0.81 failed.  Maybe you should try again.";
	}

	if ( qRound(version*100) < 82 ) {
		database->transaction();

		//==================add a columns to 'recipes' to allow yield range + yield type
		database->exec( "CREATE TABLE recipes_copy (id SERIAL NOT NULL PRIMARY KEY,title CHARACTER VARYING, persons INTEGER, instructions TEXT, photo TEXT, prep_time TIME);" );
		QSqlQuery copyQuery = database->exec( "SELECT id,title,persons,instructions,photo,prep_time FROM recipes;" );
		if ( copyQuery.isActive() ) {
			while ( copyQuery.next() ) {
				QSqlQuery query( QString(), *database );
				query.prepare( "INSERT INTO recipes_copy VALUES (?, ?, ?, ?, ?, ?)" );
				query.addBindValue( copyQuery.value( 0 ) );
				query.addBindValue( copyQuery.value( 1 ) );
				query.addBindValue( copyQuery.value( 2 ) );
				query.addBindValue( copyQuery.value( 3 ) );
				query.addBindValue( copyQuery.value( 4 ) );
				query.addBindValue( copyQuery.value( 5 ) );
				query.exec();

				emit progress();
			}
		}
		database->exec( "DROP TABLE recipes" );
		database->exec( "CREATE TABLE recipes (id SERIAL NOT NULL PRIMARY KEY,title CHARACTER VARYING, yield_amount FLOAT, yield_amount_offset FLOAT, yield_type_id INTEGER DEFAULT '-1', instructions TEXT, photo TEXT, prep_time TIME);" );
		copyQuery = database->exec( "SELECT id,title,persons,instructions,photo,prep_time FROM recipes_copy" );
		if ( copyQuery.isActive() ) {
			while ( copyQuery.next() ) {
				QSqlQuery query( QString(), *database );
				query.prepare( "INSERT INTO recipes VALUES (?, ?, ?, ?, ?, ?, ?, ?)" );
				query.addBindValue( copyQuery.value( 0 ) ); //id
				query.addBindValue( copyQuery.value( 1 ) ); //title
				query.addBindValue( copyQuery.value( 2 ) ); //persons, now yield_amount
				query.addBindValue( 0 );                    //yield_amount_offset
				query.addBindValue( -1 );                   //yield_type_id
				query.addBindValue( copyQuery.value( 3 ) ); //instructions
				query.addBindValue( copyQuery.value( 4 ) ); //photo
				query.addBindValue( copyQuery.value( 5 ) ); //prep_time
				query.exec();

				emit progress();
			}
		}
		database->exec( "DROP TABLE recipes_copy" );

		database->exec( "UPDATE db_info SET ver='0.82',generated_by='Krecipes SVN (20050902)';" );

		if ( !database->commit() )
			kDebug()<<"Update to 0.82 failed.  Maybe you should try again.";
	}

	if ( qRound(version*100) < 83 ) {
		database->transaction();

		//====add a id columns to 'ingredient_list' to identify it for the prep method list
		database->exec( "ALTER TABLE ingredient_list RENAME TO ingredient_list_copy;" );

		database->exec( "CREATE TABLE ingredient_list (id SERIAL NOT NULL PRIMARY KEY, recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, amount_offset FLOAT, unit_id INTEGER, order_index INTEGER, group_id INTEGER);" );

		QSqlQuery copyQuery = database->exec( "SELECT recipe_id,ingredient_id,amount,amount_offset,unit_id,prep_method_id,order_index,group_id FROM ingredient_list_copy" );
		if ( copyQuery.isActive() ) {
			while ( copyQuery.next() ) {
				int ing_list_id = getNextInsertID("ingredient_list","id");

				QSqlQuery query( QString(), *database );
				query.prepare( "INSERT INTO ingredient_list VALUES (?, ?, ?, ?, ?, ?, ?, ?)" );
				query.addBindValue( ing_list_id );
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
					query.addBindValue( ing_list_id );
					query.addBindValue( prep_method_id );
					query.addBindValue( 1 );
					query.exec();
				}

				emit progress();
			}
		}
		database->exec( "DROP TABLE ingredient_list_copy" );

		database->exec( "CREATE INDEX ridil_index ON ingredient_list USING BTREE (recipe_id);" );
		database->exec( "CREATE INDEX iidil_index ON ingredient_list USING BTREE (ingredient_id);" );

		database->exec( "UPDATE db_info SET ver='0.83',generated_by='Krecipes SVN (20050909)';" );

		if ( !database->commit() ) {
			kDebug()<<"Update to 0.83 failed.  Maybe you should try again.";
			return;
		}
	}

	if ( qRound(version*100) < 84 ) {
		database->transaction();

		database->exec( "ALTER TABLE recipes ADD COLUMN ctime TIMESTAMP" );
		database->exec( "ALTER TABLE recipes ADD COLUMN mtime TIMESTAMP" );
		database->exec( "ALTER TABLE recipes ADD COLUMN atime TIMESTAMP" );

		database->exec( "UPDATE recipes SET ctime=CURRENT_TIMESTAMP, mtime=CURRENT_TIMESTAMP, atime=CURRENT_TIMESTAMP;" );

		database->exec( "UPDATE db_info SET ver='0.84',generated_by='Krecipes SVN (20050913)';" );

		if ( !database->commit() ) {
			kDebug()<<"Update to 0.84 failed.  Maybe you should try again.";
			return;
		}
	}

	if ( qRound(version*100) < 85 ) { //this change altered the photo format, but this backend already used the newer format
		database->transaction();

		database->exec( "UPDATE db_info SET ver='0.85',generated_by='Krecipes SVN (20050926)';" );

		if ( !database->commit() ) {
			kDebug()<<"Update to 0.85 failed.  Maybe you should try again.";
			return;
		}
	}

	if ( qRound(version*100) < 86 ) {
		database->transaction();

		database->exec( "CREATE INDEX gidil_index ON ingredient_list USING BTREE (group_id);" );

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
		database->exec( QString("INSERT INTO rating_criteria VALUES (1,'%1')").arg(i18nc("Overall Rating", "Overall")) );
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
		database->exec("CREATE index parent_id_index ON categories USING BTREE(parent_id)");
		database->exec("UPDATE db_info SET ver='0.91',generated_by='Krecipes SVN (20060526)'");
	}

	if ( qRound(version*100) < 92 ) {
		database->transaction();

		//==================add a columns to 'units' to allow unit abbreviations
		database->exec( "ALTER TABLE units RENAME TO units_copy" );

		int nextval = -1;
		QSqlQuery getID( "SELECT nextval('units_id_seq')", *database );
		if ( getID.isActive() && getID.first() )
			nextval = getID.value( 0 ).toInt();
		if ( nextval == -1 )
			kDebug() << "Database update failed! Unable to update units sequence." ;

		database->exec( "CREATE TABLE units (id SERIAL NOT NULL PRIMARY KEY, name CHARACTER VARYING, name_abbrev CHARACTER VARYING, plural CHARACTER VARYING, plural_abbrev CHARACTER VARYING )" );
		QSqlQuery copyQuery = database->exec( "SELECT id,name,plural FROM units_copy" );
		if ( copyQuery.isActive() ) {
			while ( copyQuery.next() ) {
				QSqlQuery query( QString(), *database );
				query.prepare( "INSERT INTO units VALUES(?, ?, ?, ?, ?)" );
				query.addBindValue( copyQuery.value( 0 ) );
				query.addBindValue( copyQuery.value( 1 ) );
				query.addBindValue( QVariant() );
				query.addBindValue( copyQuery.value( 2 ) );
				query.addBindValue( QVariant() );
				query.exec();

				emit progress();
			}
		}
		database->exec( "DROP TABLE units_copy" );

		database->exec( "ALTER TABLE units_id_seq1 RENAME TO units_id_seq" );
		database->exec( "ALTER SEQUENCE units_id_seq RESTART WITH "+QString::number(nextval) );

		database->exec("UPDATE db_info SET ver='0.92',generated_by='Krecipes SVN (20060609)'");
		if ( !database->commit() )
			kDebug()<<"Update to 0.92 failed.  Maybe you should try again.";
	}

	if ( qRound(version*100) < 93 ) {
		database->transaction();

		database->exec( "ALTER TABLE ingredient_list ADD COLUMN substitute_for INTEGER" );

		database->exec("UPDATE db_info SET ver='0.93',generated_by='Krecipes SVN (20060616)'");
		if ( !database->commit() )
			kDebug()<<"Update to 0.93 failed.  Maybe you should try again.";
	}

	if ( qRound(version*100) < 94 ) {
		database->transaction();

		database->exec( "ALTER TABLE units ADD COLUMN type INTEGER NOT NULL DEFAULT '0'" );

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
		database->exec( "UPDATE db_info SET ver='0.97',generated_by='Krecipes 2.0.0'" );
	}

}

void PSqlRecipeDB::addColumn( const QString &new_table_sql, const QString &new_col_info, const QString &default_value, const QString &table_name, int col_index )
{
	QString command;

	command = QString(new_table_sql).arg(table_name+"_copy").arg(QString());
	kDebug()<<"calling: "<<command;
	QSqlQuery query( command, *database );

	command = "SELECT * FROM "+table_name+';';
	query.exec( command );
	if ( query.isActive() ) {
		while ( query.next() ) {
			QStringList dataList;
			for ( int i = 0 ;; ++i ) {
				QVariant variant = query.value(i);
				if ( variant.type() == QVariant::Invalid ) break;

				dataList << '\''+variant.toString()+'\'';
			}
			command = "INSERT INTO "+table_name+"_copy VALUES("+dataList.join(",")+");";
			kDebug()<<"calling: "<<command;
			QSqlQuery insert_query( command, *database );

			emit progress();
		}
	}
	query.exec( "DROP TABLE "+table_name+';' );
	query.exec( QString(new_table_sql).arg(table_name).arg(new_col_info+',') );
	query.exec( "SELECT * FROM "+table_name+"_copy;" );
	if ( query.isActive() ) {
		while ( query.next() ) {
			QStringList dataList;
			for ( int i = 0 ;; ++i ) {
				if ( i == col_index )
					dataList << default_value;

				QVariant variant = query.value(i);
				if ( variant.type() == QVariant::Invalid ) break;

				dataList << '\''+variant.toString()+'\'';
			}
			command = "INSERT INTO "+table_name+" VALUES(" +dataList.join(",")+");";
			QSqlQuery insert_query( command, *database );
			kDebug()<<"calling: "<<command;

			emit progress();
		}
	}
	query.exec( "DROP TABLE "+table_name+"_copy;" );
}

RecipeDB::IdType PSqlRecipeDB::lastInsertId(const QSqlQuery &query)
{	
	Q_UNUSED( query )
	return last_insert_id;
}

int PSqlRecipeDB::getNextInsertID( const QString &table, const QString &column )
{
	QString command = QString( "SELECT nextval('%1_%2_seq');" ).arg( table ).arg( column );
	QSqlQuery getID( command, *database );

	if ( getID.isActive() && getID.first() ) {
		last_insert_id = getID.value( 0 ).toInt();
	}
	else
		last_insert_id = -1;

	return last_insert_id;
}

void PSqlRecipeDB::givePermissions( const QString & /*dbName*/, const QString &username, const QString &password, const QString & /*clientHost*/ )
{
	QStringList tables;
	tables << "ingredient_info" << "ingredient_list" << "ingredient_properties" << "ingredients" << "recipes" << "unit_list" << "units" << "units_conversion" << "categories" << "category_list" << "authors" << "author_list" << "prep_methods" << "db_info" << "ingredient_groups" << "ingredient_weights" << "prep_method_list" << "yield_types" << "ratings" << "rating_criteria" << "rating_criterion_list";

	//we also have to grant permissions on the sequences created
	tables << "authors_id_seq" << "categories_id_seq" << "ingredient_properties_id_seq" << "ingredient_weights_id_seq" << "ingredients_id_seq" << "prep_methods_id_seq" << "recipes_id_seq" << "units_id_seq" << "ingredient_groups_id_seq" << "yield_types_id_seq" << "ingredient_list_id_seq" << "ratings_id_seq" << "rating_criteria_id_seq";

	QString command;

	kDebug() << "I'm doing the query to create the new user" ;
	command = "CREATE USER " + username;
	if ( !password.isEmpty() )
		command.append( "WITH PASSWORD '" + password + '\'' );
	command.append( ";" );
	QSqlQuery permissionsToSet( command, *database );

	kDebug() << "I'm doing the query to setup permissions";
	command = QString( "GRANT ALL ON %1 TO %2;" ).arg( tables.join( "," ) ).arg( username );
	permissionsToSet.exec( command );
}

void PSqlRecipeDB::empty( void )
{
	QSqlRecipeDB::empty();

	QStringList tables;
	tables << "authors_id_seq" << "categories_id_seq" << "ingredient_properties_id_seq" << "ingredient_weights_id_seq" << "ingredients_id_seq" << "prep_methods_id_seq" << "recipes_id_seq" << "units_id_seq" << "ingredient_groups_id_seq" << "yield_types_id_seq" << "ingredient_list_id_seq" << "prep_method_list_id_seq" << "ratings_id_seq" << "rating_criteria_id_seq";

	QSqlQuery tablesToEmpty( QString(), *database );
	for ( QStringList::Iterator it = tables.begin(); it != tables.end(); ++it ) {
		QString command = QString( "DELETE FROM %1;" ).arg( *it );
		tablesToEmpty.exec( command );
	}
}

#include "psqlrecipedb.moc"
