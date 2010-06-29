/***************************************************************************
*   Copyright © 2003-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "recipemlexporter.h"

#include <kdebug.h>
#include <klocale.h>

#include "backends/recipedb.h"

RecipeMLExporter::RecipeMLExporter( const QString& filename, const QString& format ) :
		BaseExporter( filename, format )
{}


RecipeMLExporter::~RecipeMLExporter()
{}

int RecipeMLExporter::supportedItems() const
{
	return RecipeDB::All ^ RecipeDB::Photo ^ RecipeDB::Ratings;
}

QString RecipeMLExporter::createHeader( const RecipeList& )
{
	QString xml = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
	xml += "<!DOCTYPE recipeml PUBLIC \"-//FormatData//DTD RecipeML 0.5//EN\" \
	  \"http://www.formatdata.com/recipeml/recipeml.dtd\">";
	xml += "<recipeml version=\"0.5\" generator=\"Krecipes v"+krecipes_version()+"\">\n";
	return xml;
}

QString RecipeMLExporter::createFooter()
{
	return "</recipeml>";
}

void RecipeMLExporter::createIngredient( QDomElement &ing_tag, const IngredientData &ing, QDomDocument &doc )
{
	QDomElement amt_tag = doc.createElement( "amt" );
	ing_tag.appendChild( amt_tag );

	QDomElement qty_tag = doc.createElement( "qty" );
	amt_tag.appendChild( qty_tag );
	if ( ing.amount_offset < 1e-10 )
		qty_tag.appendChild( doc.createTextNode( QString::number( ing.amount ) ) );
	else {
		QDomElement range_tag = doc.createElement( "range" );
		qty_tag.appendChild(range_tag);
		
		QDomElement q1_tag = doc.createElement( "q1" );
		q1_tag.appendChild( doc.createTextNode( QString::number( ing.amount ) ) );
		QDomElement q2_tag = doc.createElement( "q2" );
		q2_tag.appendChild( doc.createTextNode( QString::number( ing.amount + ing.amount_offset ) ) );

		range_tag.appendChild(q1_tag);
		range_tag.appendChild(q2_tag);
	}

	QDomElement unit_tag = doc.createElement( "unit" );
	amt_tag.appendChild( unit_tag );
	unit_tag.appendChild( doc.createTextNode( ing.units.determineName(ing.amount, /*useAbbrev=*/false ) ) );

	QDomElement item_tag = doc.createElement( "item" );
	item_tag.appendChild( doc.createTextNode( ing.name ) );
	ing_tag.appendChild( item_tag );

	QDomElement prep_tag = doc.createElement( "prep" );
	prep_tag.appendChild( doc.createTextNode( ing.prepMethodList.join(",") ) );
	ing_tag.appendChild( prep_tag );
}

QString RecipeMLExporter::createContent( const RecipeList& recipes )
{
	QDomDocument doc;

	RecipeList::const_iterator recipe_it;
	for ( recipe_it = recipes.begin(); recipe_it != recipes.end(); ++recipe_it ) {
		QDomElement recipe_tag = doc.createElement( "recipe" );

		doc.appendChild(recipe_tag);
		//recipe_root.appendChild( recipe_tag ); //will append to either <menu> if exists or else <recipeml>

		QDomElement head_tag = doc.createElement( "head" );
		recipe_tag.appendChild( head_tag );

		QDomElement title_tag = doc.createElement( "title" );
		title_tag.appendChild( doc.createTextNode( ( *recipe_it ).title ) );
		head_tag.appendChild( title_tag );

		QDomElement source_tag = doc.createElement( "source" );
		for ( ElementList::const_iterator author_it = ( *recipe_it ).authorList.begin(); author_it != ( *recipe_it ).authorList.end(); ++author_it ) {
			QDomElement srcitem_tag = doc.createElement( "srcitem" );
			srcitem_tag.appendChild( doc.createTextNode( ( *author_it ).name ) );
			source_tag.appendChild( srcitem_tag );
		}
		head_tag.appendChild( source_tag );

		QDomElement categories_tag = doc.createElement( "categories" );
		for ( ElementList::const_iterator cat_it = ( *recipe_it ).categoryList.begin(); cat_it != ( *recipe_it ).categoryList.end(); ++cat_it ) {
			QDomElement cat_tag = doc.createElement( "cat" );
			cat_tag.appendChild( doc.createTextNode( ( *cat_it ).name ) );
			categories_tag.appendChild( cat_tag );
		}
		head_tag.appendChild( categories_tag );

		QDomElement yield_tag = doc.createElement( "yield" );
		if ( ( *recipe_it ).yield.amountOffset() < 1e-10 )
			yield_tag.appendChild( doc.createTextNode( QString::number( ( *recipe_it ).yield.amount() ) ) );
		else {
			QDomElement range_tag = doc.createElement( "range" );
			yield_tag.appendChild(range_tag);
			
			QDomElement q1_tag = doc.createElement( "q1" );
			q1_tag.appendChild( doc.createTextNode( QString::number(( *recipe_it ).yield.amount() ) ) );
			QDomElement q2_tag = doc.createElement( "q2" );
			q2_tag.appendChild( doc.createTextNode( QString::number( ( *recipe_it ).yield.amount() + ( *recipe_it ).yield.amountOffset() ) ) );

			range_tag.appendChild(q1_tag);
			range_tag.appendChild(q2_tag);
		}
		if ( !( *recipe_it ).yield.type().isEmpty() ) {
			QDomElement yield_unit_tag = doc.createElement( "unit" );
			yield_unit_tag.appendChild( doc.createTextNode(( *recipe_it ).yield.type()) );
			yield_tag.appendChild( yield_unit_tag );
		}

		head_tag.appendChild( yield_tag );

		if ( !( *recipe_it ).prepTime.isNull() ) {
			QDomElement preptime_tag = doc.createElement( "preptime" );
			head_tag.appendChild( preptime_tag );
			preptime_tag.setAttribute( "type", i18nc( "Total preparation time", "Total" ) );

			QDomElement preptime_time_tag = doc.createElement( "time" );
			preptime_tag.appendChild( preptime_time_tag );

			QDomElement preptime_min_qty_tag = doc.createElement( "qty" );
			preptime_time_tag.appendChild( preptime_min_qty_tag );
			preptime_min_qty_tag.appendChild( doc.createTextNode( QString::number( ( *recipe_it ).prepTime.minute() + ( *recipe_it ).prepTime.hour() * 60 ) ) );

			QDomElement preptime_min_unit_tag = doc.createElement( "timeunit" );
			preptime_time_tag.appendChild( preptime_min_unit_tag );
			preptime_min_unit_tag.appendChild( doc.createTextNode( "minutes" ) );
		}

		QDomElement ingredients_tag = doc.createElement( "ingredients" );
		IngredientList list_copy = ( *recipe_it ).ingList;
		for ( IngredientList group_list = list_copy.firstGroup(); group_list.count() != 0; group_list = list_copy.nextGroup() ) {
			QDomElement ing_root;

			QString group = group_list[ 0 ].group; //just use the first's name... they're all the same
			if ( !group.isEmpty() ) {
				QDomElement ingdiv_tag = doc.createElement( "ing-div" );
				QDomElement title_tag = doc.createElement( "title" );
				title_tag.appendChild( doc.createTextNode( group ) );
				ingdiv_tag.appendChild( title_tag );
				ingredients_tag.appendChild( ingdiv_tag );
				ing_root = ingdiv_tag;
			}
			else
				ing_root = ingredients_tag;

			for ( IngredientList::const_iterator ing_it = group_list.begin(); ing_it != group_list.end(); ++ing_it ) {
				QDomElement ing_tag = doc.createElement( "ing" );
				ing_root.appendChild( ing_tag );

				createIngredient( ing_tag, *ing_it, doc );

				for ( Ingredient::SubstitutesList::const_iterator sub_it = (*ing_it).substitutes.begin(); sub_it != (*ing_it).substitutes.end(); ++sub_it ) {
					QDomElement alt_ing_tag = doc.createElement( "alt-ing" );
					ing_tag.appendChild( alt_ing_tag );
					createIngredient( alt_ing_tag, *sub_it, doc );
				}
			}
		}
		recipe_tag.appendChild( ingredients_tag );

		QDomElement directions_tag = doc.createElement( "directions" );
		recipe_tag.appendChild( directions_tag );

		QDomElement step_tag = doc.createElement( "step" ); //we've just got everything in one step
		directions_tag.appendChild( step_tag );
		step_tag.appendChild( doc.createTextNode( ( *recipe_it ).instructions ) );
	}

	return doc.toString();
}
