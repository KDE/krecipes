 /***************************************************************************
 *   Copyright (C) 2003 by krecipes.sourceforge.net authors                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "recipedb.h"

#include <kdebug.h>
#include <kstandarddirs.h>

#include <qfile.h>
#include <qstringlist.h>

#include <map>

#include "ingredientpropertylist.h"
#include "usda_property_data.h"
#include "usda_ingredient_data.h"

struct ingredient_nutrient_data
{
	QString name;
	QValueList<double> data;
};

void RecipeDB::loadRecipes( RecipeList *recipes, const QValueList<int>& ids )
{
	recipes->empty();

	for ( QValueList<int>::const_iterator it = ids.begin(); it != ids.end(); ++it )
	{
		Recipe recipe;
		loadRecipe( &recipe, *it );
		recipes->append( recipe );
	}
}


//These are helper functions solely for use by the USDA data importer
void getIngredientNameAndID( std::multimap<int,QString> * );
int createUnit(const QString &name, RecipeDB*);
int createIngredient(const QString &name, int unit_g_id, int unit_mg_id, RecipeDB*);
void create_properties( RecipeDB* );

void RecipeDB::importUSDADatabase()
{
	//check if the data file even exists before we do anything
	QString abbrev_file = locate("appdata","data/abbrev.txt");
	if ( abbrev_file.isEmpty() )
	{
		kdDebug()<<"Unable to find abbrev.txt data file."<<endl;
		return;
	}

	QFile file( abbrev_file );
	if ( !file.open( IO_ReadOnly ) )
	{
		kdDebug()<<"Unable to open data file: "<<abbrev_file<<endl;
		return;
	}

	create_properties(this);

	std::multimap<int,QString> *ings_and_ids = new std::multimap<int,QString>;
	getIngredientNameAndID( ings_and_ids );

	QTextStream stream( &file );
	QValueList<ingredient_nutrient_data> *data = new QValueList<ingredient_nutrient_data>;

	kdDebug()<<"Parsing abbrev.txt"<<endl;
	while ( !stream.atEnd() )
	{
		QStringList fields = QStringList::split( "^", stream.readLine(), true );

		int id = fields[0].mid(1,fields[0].length()-2).toInt();

		std::multimap<int,QString>::iterator current_pair;
		while ( (current_pair = ings_and_ids->find(id)) != ings_and_ids->end() ) //there may be more than one ingredients with the same id
		{
			ingredient_nutrient_data current_ing;
			current_ing.name = (*current_pair).second.latin1();

			for ( int i = 2; i < TOTAL_USDA_PROPERTIES + 2; i++ ) //properties start at the third field (index 2)
				current_ing.data << fields[i].toDouble();

			data->append( current_ing );

			ings_and_ids->erase( current_pair );
		}
	}

	delete ings_and_ids;

	//since there are only two units used, lets just create them and store their id for speed
	int unit_g_id = createUnit("g",this);
	int unit_mg_id = createUnit("mg",this);

	QValueList<ingredient_nutrient_data>::iterator it; const int total = data->count(); int counter = 0;
	for ( it = data->begin(); it != data->end(); ++it )
	{
		counter++;
		kdDebug()<<"Inserting ("<<counter<<" of "<<total<<"): "<<(*it).name<<endl;

		int assigned_id = createIngredient((*it).name,unit_g_id,unit_mg_id,this);

		//for now, only check if there is any info on the ingredient to see whether or not we will import this data,
		//because checking to see that each property exists is quite slow
		IngredientPropertyList ing_properties; loadProperties( &ing_properties, assigned_id );
		if ( ing_properties.count() == 0 ) //ingredient doesn't already have any properties
		{
			QValueList<double>::iterator property_it; int i = 0;
			for ( property_it = (*it).data.begin(); property_it != (*it).data.end(); ++property_it, ++i )
				addPropertyToIngredient(assigned_id,property_data_list[i].id,(*property_it)/100.0,unit_g_id);
		}
	}

	delete data;

	kdDebug()<<"USDA SR 16 data import successful"<<endl;
}

void getIngredientNameAndID( std::multimap<int,QString> *data )
{
	for ( int i = 0; ingredient_data_list[i].name; i++ )
		data->insert( std::make_pair(ingredient_data_list[i].usda_id, ingredient_data_list[i].name) );
}

int createIngredient(const QString &name, int unit_g_id, int unit_mg_id, RecipeDB *database)
{
	int assigned_id = database->findExistingIngredientByName( name );

	if ( assigned_id == -1 )
	{
		database->createNewIngredient( name );
		assigned_id = database->lastInsertID();
	}

	if ( !database->ingredientContainsUnit( assigned_id, unit_g_id ) )
		database->addUnitToIngredient( assigned_id, unit_g_id );

	if ( !database->ingredientContainsUnit( assigned_id, unit_mg_id ) )
		database->addUnitToIngredient( assigned_id, unit_mg_id );

	return assigned_id;
}

int createUnit(const QString &name, RecipeDB *database)
{
	int assigned_id = database->findExistingUnitByName( name );

	if ( assigned_id == -1 ) //create unit since it doesn't exist
	{
		database->createNewUnit( name );
		assigned_id = database->lastInsertID();
	}

	return assigned_id;
}

void create_properties( RecipeDB *database )
{
	IngredientPropertyList property_list; database->loadProperties( &property_list );

	for ( int i = 0; property_data_list[i].name; i++ )
	{
		property_data_list[i].id = property_list.findByName( property_data_list[i].name );
		if ( property_data_list[i].id == -1 )//doesn't exist, so insert it and set property_data_list[i].id
		{
			database->addProperty( property_data_list[i].name, property_data_list[i].unit );
			property_data_list[i].id = database->lastInsertID();
		}
	}
}

#include "recipedb.moc"
