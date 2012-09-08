/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "plaintextexporter.h"

#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>

#include "backends/recipedb.h"

PlainTextExporter::PlainTextExporter( const QString& filename, const QString& format ) :
		BaseExporter( filename, format )
{}


PlainTextExporter::~PlainTextExporter()
{}

int PlainTextExporter::supportedItems() const
{
	return RecipeDB::All ^ RecipeDB::Photo;
}

QString PlainTextExporter::generateIngredient( const IngredientData &ing, MixedNumber::Format number_format )
{
	KConfigGroup config(KGlobal::config(),"Formatting");

	QString content;

	QString amount_str = MixedNumber( ing.amount ).toString( number_format );

	if ( ing.amount_offset > 0 )
		amount_str += '-'+MixedNumber( ing.amount + ing.amount_offset ).toString( number_format );
	else if ( ing.amount <= 1e-10 )
		amount_str = "";

	content += amount_str;
	if ( !amount_str.isEmpty() )
		content += ' ';

	QString unit_str = ing.units.determineName( ing.amount + ing.amount_offset, config.readEntry("AbbreviateUnits", false) );

	content += unit_str;
	if ( !unit_str.isEmpty() )
		content += ' ';

	content += ing.name;

	if ( ing.prepMethodList.count() > 0 )
		content += "; "+ing.prepMethodList.join(", ");

	return content;
}


QString PlainTextExporter::createContent( const RecipeList& recipes )
{
	KConfigGroup config = KGlobal::config()->group( "Formatting" );

	MixedNumber::Format number_format = ( config.readEntry( "Fraction", false ) ) ? MixedNumber::MixedNumberFormat : MixedNumber::DecimalFormat;

	QString content;

	RecipeList::const_iterator recipe_it;
	for ( recipe_it = recipes.begin(); recipe_it != recipes.end(); ++recipe_it ) {
		content += ( *recipe_it ).title + "\n\n";

		if ( ( *recipe_it ).authorList.count() > 0 ) {
			content += i18n("Authors: %1", ( *recipe_it ).authorList.join(", "));
			content += '\n';
		}

		if ( ( *recipe_it ).categoryList.count() > 0 ) {
			content += i18n("Categories: %1", ( *recipe_it ).categoryList.join(", "));
			content += '\n';
		}

		if ( ( *recipe_it ).yield.amount() > 0 ) {
			content += i18n("Yields: %1", ( *recipe_it ).yield.toString());
			content += '\n';
		}

		if ( !( *recipe_it ).prepTime.isNull() ) {
			content += i18n("Preparation Time: %1", ( *recipe_it ).prepTime.toString( "hh:mm" ));
			content += '\n';
		}

		content += '\n';

		IngredientList list_copy = ( *recipe_it ).ingList;
		for ( IngredientList group_list = list_copy.firstGroup(); group_list.count() != 0; group_list = list_copy.nextGroup() ) {
			QString group = group_list[ 0 ].group; //just use the first's name... they're all the same
			if ( !group.isEmpty() )
				content += group + ":\n";

			for ( IngredientList::const_iterator ing_it = group_list.begin(); ing_it != group_list.end(); ++ing_it ) {
				if ( !group.isEmpty() )
					content += "  ";

				content += generateIngredient(*ing_it,number_format);

				if ( (*ing_it).substitutes.count() > 0 )
					content += ", "+i18n("or");
				content += '\n';
				
				for ( Ingredient::SubstitutesList::const_iterator sub_it = (*ing_it).substitutes.begin(); sub_it != (*ing_it).substitutes.end(); ) {
					if ( !group.isEmpty() )
						content += "  ";

					content += generateIngredient(*sub_it,number_format);
					sub_it++;
					if ( sub_it != (*ing_it).substitutes.end() )
						content += ", "+i18n("or");
					content += '\n';
				}
			}
		}

		content += '\n';

		/// @todo add ingredient properties

		content += ( *recipe_it ).instructions;

		content += "\n\n";

		if ( (*recipe_it).ratingList.count() > 0 )
			content += "----------"+i18n("Ratings")+"----------\n";

		for ( RatingList::const_iterator rating_it = (*recipe_it).ratingList.begin(); rating_it != (*recipe_it).ratingList.end(); ++rating_it ) {
			if ( !( *rating_it ).rater().isEmpty() )
				content += "  "+( *rating_it ).rater()+'\n';

			if ( (*rating_it).hasRatingCriterias() )
				content += '\n';

			foreach ( RatingCriteria rc, (*rating_it).ratingCriterias() ) {
				//FIXME: This is an ugly hack, but I don't know how else to be i18n friendly (if this is even that)
				// and still be able to display the amount as a fraction
				//KDE$ port
				QString starsTrans = i18np("1 star","%1 stars",qRound(rc.stars()));
				starsTrans.replace(QString::number(qRound(rc.stars())),MixedNumber(rc.stars()).toString());

				content +=  "  "+rc.name()+": "+starsTrans+'\n';
			}

			if ( (*rating_it).hasRatingCriterias() )
				content += '\n';

			if ( !( *rating_it ).comment().isEmpty() )
				content += "  "+( *rating_it ).comment()+'\n';

			content += '\n';
		}

		if ( (*recipe_it).ratingList.size() > 0 )
			content += '\n';

		content += "-----\n\n"; //end of recipe indicator
	}

	return content;
}

