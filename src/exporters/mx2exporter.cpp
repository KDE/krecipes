/***************************************************************************
*   Copyright © 2003-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "mx2exporter.h"

#include <kdebug.h>
#include <klocale.h>

#include "backends/recipedb.h"

MX2Exporter::MX2Exporter( const QString& filename, const QString& format ) :
		BaseExporter( filename, format )
{}

MX2Exporter::~MX2Exporter()
{}

int MX2Exporter::supportedItems() const
{
	//return RecipeDB::All ^ RecipeDB::Photo ^ RecipeDB::Ratings;
	return RecipeDB::All ^ RecipeDB::Photo;
}

QString MX2Exporter::createHeader( const RecipeList& )
{
	QString xml = "<?xml version=\"1.0\" standalone=\"yes\" encoding=\"UTF-8\" ?>\n";
	xml += "<!DOCTYPE mx2 SYSTEM \"mx2.dtd\">\n";
	xml += "<mx2 source=\"krecipes version "+krecipes_version()+"\">\n";
	return xml;
}

QString MX2Exporter::createFooter()
{
	return "</mx2>";
}

QString MX2Exporter::createContent( const RecipeList& recipes )
{
	QDomDocument doc;

	RecipeList::const_iterator recipe_it;
	// First thing to do is riffle through the recipes and create a summary of names of the recipes to export.
	// <Summ><Nam>#text</Nam> ... <Nam>#text</Nam></Summ>
	QDomElement summ_tag = doc.createElement("Summ");
	for ( recipe_it = recipes.begin(); recipe_it != recipes.end(); ++recipe_it ) {
		QDomElement nam_tag = doc.createElement("Nam");
		// The only reason the recipe title has leading and trailing newlines is that the importer in MC6 wants it that way.
		nam_tag.appendChild( doc.createTextNode( "\n" + ( *recipe_it ).title + "\n" ) );
		summ_tag.appendChild(nam_tag);
	} // for recipes - summ of nam
	doc.appendChild(summ_tag);

	// Now the hard part. We have to crawl through each recipe and convert it to mx2
	for ( recipe_it = recipes.begin(); recipe_it != recipes.end(); ++recipe_it ) {
		// The outer element is a capsule for a recipe, "RcpE" in mx2
		QDomElement recipe_tag = doc.createElement( "RcpE" );
		// The RcpE can have two attributes, "name" and "author"
		// "name" is easy, just the recipe title
		recipe_tag.setAttribute( "name", ( *recipe_it ).title );
		// "author" is only slightly more difficult. Krecipes allows multiple authors but MasterCook does not.
		// So, we will concatenate multiple krecipes authors with ", " into one author for MasterCook
		QString qauthors = (QString) "";
		int author_count = 0;
		for ( ElementList::const_iterator author_it = ( *recipe_it ).authorList.begin(); author_it != ( *recipe_it ).authorList.end(); ++author_it ) {
			author_count++;
			qauthors = qauthors + (author_count == 1 ? (QString) "" : (QString) ", ") + ( *author_it ).name;			
		} // for authors
		if (qauthors != (QString) "") recipe_tag.setAttribute("author", qauthors);
		// Next we do servings "Serv" as an element of RcpE. It can have an attribute "qty" which is the number of servings
		// This is a bit odd in krecipes which doesn't have explicit servings but instead has "yield.amount" and "yield.type",
		// where ".type" might be servings or might be some other units like liters or cups. So we test if the yield type is 
		// "servings" and it it is, use it.
		QDomElement serv_tag = doc.createElement("Serv");
		if (( *recipe_it ).yield.type() == "servings") 
			serv_tag.setAttribute("qty", ( *recipe_it ).yield.amount());
		else serv_tag.setAttribute("qty", 0);
		recipe_tag.appendChild(serv_tag);
		// Now preparation time, "PrpT". MasterCook distinguishes prep time from Total time; krecipes does not.
		// So we just use the same total time in krecipes for both values in MasterCook.
		QDomElement prepTime_tag = doc.createElement("PrpT");
		prepTime_tag.setAttribute("elapsed", ( *recipe_it ).prepTime.toString((QString) "hh:mm"));
		recipe_tag.appendChild(prepTime_tag);
		// Catagories are next. In mx2 format this is <CatS><CatT>#text</CatT>...<CatT>#text</CatT></CatS>
		QDomElement cats_tag = doc.createElement("CatS");
 		for ( ElementList::const_iterator cat_it = ( *recipe_it ).categoryList.begin(); cat_it != ( *recipe_it ).categoryList.end(); ++cat_it ) {
 			QDomElement catt_tag = doc.createElement( "CatT" );
 			catt_tag.appendChild( doc.createTextNode( ( *cat_it ).name ) );
			cats_tag.appendChild( catt_tag );
		} // for catagories
		recipe_tag.appendChild(cats_tag);
		//
		// Here is the most difficult part -- the ingredients list.
		// Both MasterCook and krecipes have ingredient-groups/subtitles in the recipe
		// list. They are analogous. The difference is that while krecipes has
		// ingredient-groups as parent elements to ingredients, MasterCook does not
		// distinguish.
		// In MasterCook both are ingredients except that an ingredient that is really a
		// group/subtitle header has an attribute "code" with the value "S".
		// After that, the big difference is that MasterCook treats name, units, and
		// quantity as atributes of an ingredient while krecipes treats them as separate
		// elements.
		//
		// So we will crawl through the groups and ingredients...
		// We will handle the cases of whether there are ingredient-groups or not as
		// separate cases.
		// First we create a copy of the entire ingredient list for this recipe
		IngredientList list_copy = ( *recipe_it ).ingList;
		// and now loop through all of the groups
		for ( IngredientList group_list = list_copy.firstGroup(); group_list.count() != 0; group_list = list_copy.nextGroup() ) {
			// get the name of the group. I'm borrowing this code from elsewhere because I don't understand the group structure
			QString group = group_list[ 0 ].group; 		//just use the first's name... they're all the same
			// if the group has a name then write a <IngR> tags with code = "S"		  
			if (!group.isEmpty()) {
				QDomElement subtitle_tag = doc.createElement("IngR");
				subtitle_tag.setAttribute("name", group);
				subtitle_tag.setAttribute("code", "S");
				recipe_tag.appendChild(subtitle_tag);
			} // if groups
			// Now we loop through all of the ingredients in this group
			for ( IngredientList::const_iterator ing_it = group_list.begin(); ing_it != group_list.end(); ++ing_it ) {
				// create the basic root for an ingredient, "IngR"
				QDomElement ingr_tag = doc.createElement("IngR");
				// "IngR" takes three attributes for name, units, and quantity.
				// "IngR" also accepts the "code" attribute which would be "I" for an ingredient (not a subtitle)
				ingr_tag.setAttribute("name", (*ing_it).name);
				ingr_tag.setAttribute("unit", (( (*ing_it).amount > 1 ) ? (*ing_it).units.plural() : (*ing_it).units.name() ));
				// MasterCook doesn't seem to know about ranges, so we'll ignore amount range here
				ingr_tag.setAttribute("qty", (*ing_it).amount);
				// The "Ingr" tag also accepts a subelement "IPrp" for initial preparation
				if (!(*ing_it).prepMethodList.join(",").isEmpty() ) {
					QDomElement prep_tag = doc.createElement( "IPrp" );
					prep_tag.appendChild( doc.createTextNode( (*ing_it).prepMethodList.join(",") ) );
					ingr_tag.appendChild( prep_tag );
				} // if prep method
				recipe_tag.appendChild(ingr_tag);
				// The mx2 format doesn't seem to know about ingredient substitutions so we'll let that sleeping dog lie.
			} // for all ingredients in the group
		} // for all groups
		// Now for directions... MasterCook has multiple sections for directions whereas krecipes has only one. So we just use the one.
		QDomElement directions_tag = doc.createElement( "DirS" );
		QDomElement step_tag = doc.createElement( "DirT" ); //we've just got everything in one step
		step_tag.appendChild( doc.createTextNode( ( *recipe_it ).instructions ) );
		directions_tag.appendChild( step_tag );
		recipe_tag.appendChild( directions_tag );
		// Now the recipe yield...
		QDomElement yield_tag = doc.createElement( "Yield" );
		// once again MasterCook doesn't know about ranges so we're ignoring them
		yield_tag.setAttribute( "unit", ( *recipe_it ).yield.type());
		yield_tag.setAttribute("qty", ( *recipe_it ).yield.amount());
		recipe_tag.appendChild(yield_tag);
		// Ratings... The problem here being that MasterCook doesn't seem to recognize more than one set of ratings. So, we'll
		// just use the first one...
		if ( (*recipe_it).ratingList.count() > 0 )  {	// if there are any ratings...
			QDomElement rats_tag = doc.createElement("RatS");
			RatingList::const_iterator rating_it = (*recipe_it).ratingList.begin();
			foreach( RatingCriteria rc, (*rating_it).ratingCriterias()) {
				QDomElement rate_tag = doc.createElement("RatE");
				rate_tag.setAttribute("name", rc.name());
				rate_tag.setAttribute("value", 2.0 * rc.stars()); // 2X because MasterCook uses 1 - 10 scale
				rats_tag.appendChild(rate_tag);
			} // for elements of the rating
			recipe_tag.appendChild(rats_tag);
			// If the rating had a note, include it...
			if ( !( *rating_it ).comment().isEmpty()) {
				QDomElement note_tag = doc.createElement("Note");
				note_tag.appendChild( doc.createTextNode(( *rating_it ).comment()));
				recipe_tag.appendChild(note_tag);
			} // if there was a comment
		}  // if ratings
		doc.appendChild(recipe_tag);
	}
	return doc.toString();
}
