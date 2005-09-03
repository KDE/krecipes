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

#include "psqlrecipedb.h"

#include <kmdcodec.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <ktempfile.h>
#include <klocale.h>
#include <kconfig.h>

#include <qvariant.h>

//Note: PostgreSQL's database names are always lowercase
PSqlRecipeDB::PSqlRecipeDB( QString host, QString user, QString pass, QString DBname ) : QSqlRecipeDB( host, user, pass, DBname.lower() )
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
		QSqlQuery query( QString( "CREATE DATABASE %1" ).arg( real_db_name ), database );
		if ( !query.isActive() )
			kdDebug() << "create query failed: " << database->lastError().databaseText() << endl;

		database->close();
	}
	else
		kdDebug() << "create open failed: " << database->lastError().databaseText() << endl;

	database->setDatabaseName( real_db_name );
}

QStringList PSqlRecipeDB::backupCommand() const
{
	KConfig *config = KGlobal::config();
	config->setGroup("Server");

	QStringList command;
	command<<config->readEntry( "PgDumpPath", "pg_dump" )<<"-d"<<database->databaseName();
	return command;
}

QStringList PSqlRecipeDB::restoreCommand() const
{
	KConfig *config = KGlobal::config();
	config->setGroup("Server");

	QStringList command;
	command<<config->readEntry( "PsqlPath", "psql" )<<database->databaseName();
	return command;
}

void PSqlRecipeDB::createTable( const QString &tableName )
{

	QStringList commands;

	if ( tableName == "recipes" )
		commands << "CREATE TABLE recipes (id SERIAL NOT NULL PRIMARY KEY,title CHARACTER VARYING, yield_amount FLOAT, yield_amount_offset FLOAT, yield_type_id INTEGER DEFAULT '-1', instructions TEXT, photo TEXT, prep_time TIME);";

	else if ( tableName == "ingredients" )
		commands << "CREATE TABLE ingredients (id SERIAL NOT NULL PRIMARY KEY, name CHARACTER VARYING);";

	else if ( tableName == "ingredient_list" ) {
		commands << "CREATE TABLE ingredient_list (recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, amount_offset FLOAT, unit_id INTEGER, prep_method_id INTEGER, order_index INTEGER, group_id INTEGER);";
		commands << "CREATE INDEX ridil_index ON ingredient_list USING BTREE (recipe_id);";
		commands << "CREATE INDEX iidil_index ON ingredient_list USING BTREE (ingredient_id);";
	}

	else if ( tableName == "unit_list" )
		commands << "CREATE TABLE unit_list (ingredient_id INTEGER, unit_id INTEGER);";

	else if ( tableName == "units" )
		commands << "CREATE TABLE units (id SERIAL NOT NULL PRIMARY KEY, name CHARACTER VARYING, plural CHARACTER VARYING );";

	else if ( tableName == "prep_methods" )
		commands << "CREATE TABLE prep_methods (id SERIAL NOT NULL PRIMARY KEY, name CHARACTER VARYING);";

	else if ( tableName == "ingredient_info" )
		commands << "CREATE TABLE ingredient_info (ingredient_id INTEGER, property_id INTEGER, amount FLOAT, per_units INTEGER);";

	else if ( tableName == "ingredient_properties" )
		commands << "CREATE TABLE ingredient_properties (id SERIAL NOT NULL,name CHARACTER VARYING, units CHARACTER VARYING);";

	else if ( tableName == "units_conversion" )
		commands << "CREATE TABLE units_conversion (unit1_id INTEGER, unit2_id INTEGER, ratio FLOAT);";

	else if ( tableName == "categories" )
		commands << "CREATE TABLE categories (id SERIAL NOT NULL PRIMARY KEY, name CHARACTER VARYING default NULL, parent_id INTEGER NOT NULL default -1);";

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
	else
		return ;

	QSqlQuery databaseToCreate( QString::null, database );

	// execute the queries
	for ( QStringList::const_iterator it = commands.begin(); it != commands.end(); ++it )
		databaseToCreate.exec( *it );
}

void PSqlRecipeDB::initializeData()
{
	QSqlRecipeDB::initializeData();

	QSqlQuery updateSeq( "SELECT setval('units_id_seq',(SELECT COUNT(1) FROM units))", database );
	updateSeq.exec( "SELECT setval('categories_id_seq',(SELECT COUNT(1) FROM categories))" );
}

void PSqlRecipeDB::portOldDatabases( float version )
{
	kdDebug() << "Current database version is..." << version << "\n";
	QString command;

	if ( version < 0.7 ) {
		//version added
	}

	if ( version < 0.81 ) {
		database->transaction();

		addColumn("CREATE TABLE %1 (recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, %2 unit_id INTEGER, prep_method_id INTEGER, order_index INTEGER, group_id INTEGER);","amount_offset FLOAT","'0'","ingredient_list",3);

		QSqlQuery query(database);
		query.exec( "CREATE INDEX ridil_index ON ingredient_list USING BTREE (recipe_id);" );
		query.exec( "CREATE INDEX iidil_index ON ingredient_list USING BTREE (ingredient_id);");

		query.exec( "UPDATE db_info SET ver='0.81',generated_by='Krecipes SVN (20050816)';" );

		if ( !database->commit() )
			kdDebug()<<"Update to 0.81 failed.  Maybe you should try again."<<endl;
	}

	if ( version < 0.82 ) {
		database->transaction();

		//==================add a columns to 'recipes' to allow yield range + yield type
		database->exec( "CREATE TABLE recipes_copy (id SERIAL NOT NULL PRIMARY KEY,title CHARACTER VARYING, persons INTEGER, instructions TEXT, photo TEXT, prep_time TIME);" );
		QSqlQuery copyQuery = database->exec( "SELECT id,title,persons,instructions,photo,prep_time FROM recipes;" );
		if ( copyQuery.isActive() ) {
			while ( copyQuery.next() ) {
				QSqlQuery query(database);
				query.prepare( "INSERT INTO recipes_copy VALUES (?, ?, ?, ?, ?, ?)" );
				query.addBindValue( copyQuery.value( 0 ) );
				query.addBindValue( copyQuery.value( 1 ) );
				query.addBindValue( copyQuery.value( 2 ) );
				query.addBindValue( copyQuery.value( 3 ) );
				query.addBindValue( copyQuery.value( 4 ) );
				query.addBindValue( copyQuery.value( 5 ) );
				query.exec();
			}
		}
		database->exec( "DROP TABLE recipes" );
		database->exec( "CREATE TABLE recipes (id SERIAL NOT NULL PRIMARY KEY,title CHARACTER VARYING, yield_amount FLOAT, yield_amount_offset FLOAT, yield_type_id INTEGER DEFAULT '-1', instructions TEXT, photo TEXT, prep_time TIME);" );
		copyQuery = database->exec( "SELECT id,title,persons,instructions,photo,prep_time FROM recipes_copy" );
		if ( copyQuery.isActive() ) {
			while ( copyQuery.next() ) {
				QSqlQuery query(database);
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
			}
		}
		database->exec( "DROP TABLE recipes_copy" );

		database->exec( "UPDATE db_info SET ver='0.82',generated_by='Krecipes SVN (20050902)';" );

		if ( !database->commit() )
			kdDebug()<<"Update to 0.82 failed.  Maybe you should try again."<<endl;
	}
}

void PSqlRecipeDB::addColumn( const QString &new_table_sql, const QString &new_col_info, const QString &default_value, const QString &table_name, int col_index )
{
	QString command;

	command = QString(new_table_sql).arg(table_name+"_copy").arg(QString::null);
	kdDebug()<<"calling: "<<command<<endl;
	QSqlQuery query( command, database );

	command = "SELECT * FROM "+table_name+";";
	query.exec( command );
	if ( query.isActive() ) {
		while ( query.next() ) {
			QStringList dataList;
			for ( int i = 0 ;; ++i ) {
				QVariant variant = query.value(i);
				if ( variant.type() == QVariant::Invalid ) break;

				dataList << "'"+variant.toString()+"'";
			}
			command = "INSERT INTO "+table_name+"_copy VALUES("+dataList.join(",")+");";
			kdDebug()<<"calling: "<<command<<endl;
			QSqlQuery insert_query( command, database );
		}
	}
	query.exec( "DROP TABLE "+table_name+";" );
	query.exec( QString(new_table_sql).arg(table_name).arg(new_col_info+",") );
	query.exec( "SELECT * FROM "+table_name+"_copy;" );
	if ( query.isActive() ) {
		while ( query.next() ) {
			QStringList dataList;
			for ( int i = 0 ;; ++i ) {
				if ( i == col_index )
					dataList << default_value;

				QVariant variant = query.value(i);
				if ( variant.type() == QVariant::Invalid ) break;

				dataList << "'"+variant.toString()+"'";
			}
			command = "INSERT INTO "+table_name+" VALUES(" +dataList.join(",")+");";
			QSqlQuery insert_query( command, database );
			kdDebug()<<"calling: "<<command<<endl;
		}
	}
	query.exec( "DROP TABLE "+table_name+"_copy;" );
}

int PSqlRecipeDB::lastInsertID()
{
	return last_insert_id;
}

int PSqlRecipeDB::getNextInsertID( const QString &table, const QString &column )
{
	QString command = QString( "SELECT nextval('%1_%2_seq');" ).arg( table ).arg( column );
	QSqlQuery getID( command, database );

	if ( getID.isActive() && getID.first() ) {
		last_insert_id = getID.value( 0 ).toInt();
	}
	else
		last_insert_id = -1;

	return last_insert_id;
}

void PSqlRecipeDB::storePhoto( int recipeID, const QByteArray &data )
{
	QSqlQuery query( QString::null, database );

	query.prepare( "UPDATE recipes SET photo=? WHERE id=" + QString::number( recipeID ) );
	query.addBindValue( KCodecs::base64Encode( data ) );
	query.exec();
}

void PSqlRecipeDB::loadPhoto( int recipeID, QPixmap &photo )
{
	QString command = QString( "SELECT photo FROM recipes WHERE id=%1;" ).arg( recipeID );
	QSqlQuery query( command, database );

	if ( query.isActive() && query.first() ) {
		QCString decodedPic;
		QPixmap pix;
		KCodecs::base64Decode( QCString( query.value( 0 ).toString().latin1() ), decodedPic );
		int len = decodedPic.size();
		QByteArray picData( len );
		memcpy( picData.data(), decodedPic.data(), len );

		bool ok = pix.loadFromData( picData, "JPEG" );
		if ( ok )
			photo = pix;
	}
}

void PSqlRecipeDB::givePermissions( const QString & /*dbName*/, const QString &username, const QString &password, const QString & /*clientHost*/ )
{
	QStringList tables;
	tables << "ingredient_info" << "ingredient_list" << "ingredient_properties" << "ingredients" << "recipes" << "unit_list" << "units" << "units_conversion" << "categories" << "category_list" << "authors" << "author_list" << "prep_methods" << "db_info" << "ingredient_groups" << "yield_types_id_seq";

	//we also have to grant permissions on the sequences created
	tables << "authors_id_seq" << "categories_id_seq" << "ingredient_properties_id_seq" << "ingredients_id_seq" << "prep_methods_id_seq" << "recipes_id_seq" << "units_id_seq" << "ingredient_groups_id_seq" << "yield_types_id_seq";

	QString command;

	kdDebug() << "I'm doing the query to create the new user" << endl;
	command = "CREATE USER " + username;
	if ( !password.isEmpty() )
		command.append( "WITH PASSWORD '" + password + "'" );
	command.append( ";" );
	QSqlQuery permissionsToSet( command, database );

	kdDebug() << "I'm doing the query to setup permissions\n";
	command = QString( "GRANT ALL ON %1 TO %2;" ).arg( tables.join( "," ) ).arg( username );
	permissionsToSet.exec( command );
}

void PSqlRecipeDB::empty( void )
{
	QSqlRecipeDB::empty();

	QStringList tables;
	tables << "authors_id_seq" << "categories_id_seq" << "ingredient_properties_id_seq" << "ingredients_id_seq" << "prep_methods_id_seq" << "recipes_id_seq" << "units_id_seq" << "ingredient_groups_id_seq" << "yield_types_id_seq";

	QSqlQuery tablesToEmpty( QString::null, database );
	for ( QStringList::Iterator it = tables.begin(); it != tables.end(); ++it ) {
		QString command = QString( "DELETE FROM %1;" ).arg( *it );
		tablesToEmpty.exec( command );
	}
}

#include "psqlrecipedb.moc"
