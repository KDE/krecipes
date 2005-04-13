/***************************************************************************
*   Copyright (C) 2003-2005 by                                            *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "recipedb.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kprogress.h>
#include <kglobal.h>
#include <klocale.h>
#include <kaboutdata.h>

#include <qfile.h>
#include <qstringlist.h>

#include <map>

#include "importers/kreimporter.h"

#if HAVE_POSTGRESQL
#include "DBBackend/PostgreSQL/psqlrecipedb.h"
#endif

#if HAVE_MYSQL
#include "DBBackend/MySQL/mysqlrecipedb.h"
#endif

#if HAVE_SQLITE || HAVE_SQLITE3
#include "DBBackend/SQLite/literecipedb.h"
#endif

#include "datablocks/categorytree.h"
#include "ingredientpropertylist.h"
#include "usda_property_data.h"
#include "usda_ingredient_data.h"

struct ingredient_nutrient_data
{
	QString name;
	QValueList<double> data;
};

double RecipeDB::latestDBVersion() const
{
	return 0.7;
}

QString RecipeDB::krecipes_version() const
{
	KInstance * this_instance = KGlobal::instance();
	if ( this_instance && this_instance->aboutData() )
		return this_instance->aboutData() ->version();

	return QString::null; //Oh, well.  We couldn't get the version (shouldn't happen).
}

RecipeDB* RecipeDB::createDatabase( const QString &dbType, const QString &file )
{
	KConfig * config = kapp->config();
	config->setGroup( "Server" );
	QString host = config->readEntry( "Host", "localhost" );
	QString user = config->readEntry( "Username", QString::null );
	QString pass = config->readEntry( "Password", QString::null );
	QString dbname = config->readEntry( "DBName", DEFAULT_DB_NAME );

	return createDatabase( dbType, host, user, pass, dbname, file );
}

RecipeDB* RecipeDB::createDatabase( const QString &dbType, const QString &host, const QString &user, const QString &pass, const QString &dbname, const QString &file )
{
	RecipeDB * database = 0;

	if ( 0 )
		; //we need some condition here
#if HAVE_SQLITE || HAVE_SQLITE3

	else if ( dbType == "SQLite" ) {
		database = new LiteRecipeDB( file );
	}
#endif //HAVE_SQLITE || HAVE_SQLITE3
	#if HAVE_MYSQL
	else if ( dbType == "MySQL" ) {
		database = new MySQLRecipeDB( host, user, pass, dbname );
	}
#endif //HAVE_MYSQL
	#if HAVE_POSTGRESQL
	else if ( dbType == "PostgreSQL" ) {
		database = new PSqlRecipeDB( host, user, pass, dbname );
	}
#endif //HAVE_POSTGRESQL
	else {
		kdDebug() << "No database support included (or available) for the " << dbType << " database." << endl;
	}

	return database;
}

void RecipeDB::loadRecipes( RecipeList *recipes, const QValueList<int>& ids, KProgressDialog *progress_dlg )
{
	if ( progress_dlg )
		progress_dlg->progressBar() ->setTotalSteps( ids.count() );

	recipes->empty();

	QValueList<int>::const_iterator end = ids.end();
	for ( QValueList<int>::const_iterator it = ids.begin(); it != end; ++it ) {
		Recipe recipe;
		loadRecipe( &recipe, *it );
		recipes->append( recipe );

		if ( progress_dlg ) {
			progress_dlg->progressBar() ->advance( 1 );
			kapp->processEvents();
			if ( progress_dlg->wasCancelled() )
				break;
		}
	}
}

int RecipeDB::categoryCount()
{
	return getCount("categories");
}

int RecipeDB::authorCount()
{
	return getCount("authors");
}

int RecipeDB::ingredientCount()
{
	return getCount("ingredients");
}

int RecipeDB::prepMethodCount()
{
	return getCount("prep_methods");
}

int RecipeDB::unitCount()
{
	return getCount("units");
}

void RecipeDB::importSamples()
{
	QString sample_recipes = locate( "appdata", "data/samples-" + KGlobal::locale() ->language() + ".kreml" );
	if ( sample_recipes.isEmpty() ) {
		//TODO: Make this a KMessageBox??
		kdDebug() << "NOTICE: Samples recipes for the language \"" << KGlobal::locale() ->language() << "\" are not available.  However, if you would like samples recipes for this language in future releases of Krecipes, we invite you to submit your own.  Just save your favorite recipes in the kreml format and e-mail them to mizunoami44@users.sf.net.  Then we will have them available to everyone in the very next release." << endl;

		sample_recipes = locate( "appdata", "data/samples-en_US.kreml" ); //default to English
	}
	if ( !sample_recipes.isEmpty() ) {
		KreImporter importer;

		QStringList file;
		file << sample_recipes;
		importer.parseFiles( file );

		importer.import( this, true );
	}
	else
		kdDebug() << "Unable to find samples recipe file (samples-en_US.kreml)" << endl;
}

void RecipeDB::getIDList( const CategoryTree *categoryTree, QStringList &ids )
{
	for ( CategoryTree * child_it = categoryTree->firstChild(); child_it; child_it = child_it->nextSibling() ) {
		ids << QString::number(child_it->category.id);
		getIDList(child_it,ids );
	}
}

//These are helper functions solely for use by the USDA data importer
void getIngredientNameAndID( std::multimap<int, QString> * );
int createUnit( const QString &name, RecipeDB* );
int createIngredient( const QString &name, int unit_g_id, int unit_mg_id, RecipeDB*, bool do_checks );
void create_properties( RecipeDB* );

void RecipeDB::importUSDADatabase( KProgressDialog *progress_dlg )
{
	KProgress * progress = 0;
	if ( progress_dlg )
		progress = progress_dlg->progressBar();

	//check if the data file even exists before we do anything
	QString abbrev_file = locate( "appdata", "data/abbrev.txt" );
	if ( abbrev_file.isEmpty() ) {
		kdDebug() << "Unable to find abbrev.txt data file." << endl;
		return ;
	}

	QFile file( abbrev_file );
	if ( !file.open( IO_ReadOnly ) ) {
		kdDebug() << "Unable to open data file: " << abbrev_file << endl;
		return ;
	}

	create_properties( this );

	std::multimap<int, QString> *ings_and_ids = new std::multimap<int, QString>;
	getIngredientNameAndID( ings_and_ids );

	QTextStream stream( &file );
	QValueList<ingredient_nutrient_data> *data = new QValueList<ingredient_nutrient_data>;

	kdDebug() << "Parsing abbrev.txt" << endl;
	while ( !stream.atEnd() ) {
		QStringList fields = QStringList::split( "^", stream.readLine(), true );

		int id = fields[ 0 ].mid( 1, fields[ 0 ].length() - 2 ).toInt();

		std::multimap<int, QString>::iterator current_pair;
		while ( ( current_pair = ings_and_ids->find( id ) ) != ings_and_ids->end() )  //there may be more than one ingredients with the same id
		{
			ingredient_nutrient_data current_ing;
			current_ing.name = ( *current_pair ).second.latin1();

			for ( int i = 2; i < TOTAL_USDA_PROPERTIES + 2; i++ )  //properties start at the third field (index 2)
				current_ing.data << fields[ i ].toDouble();

			data->append( current_ing );

			ings_and_ids->erase( current_pair );
		}
	}

	delete ings_and_ids;

	if ( progress ) {
		progress->setTotalSteps( data->count() );
	}

	//if there is no data in the database, we can really speed this up with this
	bool do_checks = true;
	{
		ElementList ing_list;
		loadIngredients( &ing_list );

		if ( ing_list.count() == 0 ) {
			kdDebug()<<"Found an empty database... enabling fast nutrient import"<<endl;
			do_checks = false;
		}
	}

	//since there are only two units used, lets just create them and store their id for speed
	int unit_g_id = createUnit( "g", this );
	int unit_mg_id = createUnit( "mg", this );

	QValueList<ingredient_nutrient_data>::const_iterator it;
	QValueList<ingredient_nutrient_data>::const_iterator data_end = data->constEnd();
	const int total = data->count();
	int counter = 0;
	for ( it = data->constBegin(); it != data_end; ++it ) {
		counter++;
		kdDebug() << "Inserting (" << counter << " of " << total << "): " << ( *it ).name << endl;
		if ( progress ) {
			progress->advance( 1 );
			kapp->processEvents();

			if ( progress_dlg->wasCancelled() )
				break;
		}

		int assigned_id = createIngredient( ( *it ).name, unit_g_id, unit_mg_id, this, do_checks );

		//for now, only check if there is any info on the ingredient to see whether or not we will import this data,
		//because checking to see that each property exists is quite slow
		IngredientPropertyList ing_properties;
		if ( do_checks ) loadProperties( &ing_properties, assigned_id );
		if ( ing_properties.count() == 0 )  //ingredient doesn't already have any properties
		{
			QValueList<double>::const_iterator property_it;
			QValueList<double>::const_iterator property_end = ( *it ).data.constEnd();
			int i = 0;
			for ( property_it = ( *it ).data.constBegin(); property_it != property_end; ++property_it, ++i )
				addPropertyToIngredient( assigned_id, property_data_list[ i ].id, ( *property_it ) / 100.0, unit_g_id );
		}
	}

	delete data;

	kdDebug() << "USDA data import successful" << endl;
}

void getIngredientNameAndID( std::multimap<int, QString> *data )
{
	for ( int i = 0; !ingredient_data_list[ i ].name.isEmpty(); i++ )
		data->insert( std::make_pair( ingredient_data_list[ i ].usda_id, ingredient_data_list[ i ].name ) );
}

int createIngredient( const QString &name, int unit_g_id, int unit_mg_id, RecipeDB *database, bool do_checks )
{
	int assigned_id = -1;
	if ( do_checks )
		assigned_id = database->findExistingIngredientByName( name );

	if ( assigned_id == -1 ) {
		database->createNewIngredient( name );
		assigned_id = database->lastInsertID();
	}

	if ( !do_checks || !database->ingredientContainsUnit( assigned_id, unit_g_id ) )
		database->addUnitToIngredient( assigned_id, unit_g_id );

	if ( !do_checks || !database->ingredientContainsUnit( assigned_id, unit_mg_id ) )
		database->addUnitToIngredient( assigned_id, unit_mg_id );

	return assigned_id;
}

int createUnit( const QString &name, RecipeDB *database )
{
	int assigned_id = database->findExistingUnitByName( name );

	if ( assigned_id == -1 )  //create unit since it doesn't exist
	{
		database->createNewUnit( name, name );
		assigned_id = database->lastInsertID();
	}

	return assigned_id;
}

void create_properties( RecipeDB *database )
{
	IngredientPropertyList property_list;
	database->loadProperties( &property_list );

	for ( int i = 0; !property_data_list[ i ].name.isEmpty(); i++ ) {
		property_data_list[ i ].id = property_list.findByName( property_data_list[ i ].name );
		if ( property_data_list[ i ].id == -1 ) //doesn't exist, so insert it and set property_data_list[i].id
		{
			database->addProperty( property_data_list[ i ].name, property_data_list[ i ].unit );
			property_data_list[ i ].id = database->lastInsertID();
		}
	}
}

#include "recipedb.moc"
