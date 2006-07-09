/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
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
	KConfig *config = KGlobal::config();

	QString content;

	QString amount_str = MixedNumber( ing.amount ).toString( number_format );

	if ( ing.amount_offset > 0 )
		amount_str += "-"+MixedNumber( ing.amount + ing.amount_offset ).toString( number_format );
	else if ( ing.amount <= 1e-10 )
		amount_str = "";

	content += amount_str;
	if ( !amount_str.isEmpty() )
		content += " ";

	QString unit_str = ing.units.determineName( ing.amount + ing.amount_offset, config->readBoolEntry("AbbreviateUnits") );

	content += unit_str;
	if ( !unit_str.isEmpty() )
		content += " ";

	content += ing.name;

	if ( ing.prepMethodList.count() > 0 )
		content += "; "+ing.prepMethodList.join(", ");

	return content;
}


QString PlainTextExporter::createContent( const RecipeList& recipes )
{
	KConfig *config = KGlobal::config();
	config->setGroup( "Formatting" );

	MixedNumber::Format number_format = ( config->readBoolEntry( "Fraction" ) ) ? MixedNumber::MixedNumberFormat : MixedNumber::DecimalFormat;

	QString content;

	RecipeList::const_iterator recipe_it;
	for ( recipe_it = recipes.begin(); recipe_it != recipes.end(); ++recipe_it ) {
		content += ( *recipe_it ).title + "\n\n";

		if ( ( *recipe_it ).authorList.count() > 0 ) {
			content += QString("%1: ").arg(i18n("Authors"));
			content += ( *recipe_it ).authorList.join(", ");
			content += "\n";
		}

		if ( ( *recipe_it ).categoryList.count() > 0 ) {
			content += QString("%1: ").arg(i18n("Categories"));
			content += ( *recipe_it ).categoryList.join(", ");
			content += "\n";
		}

		if ( ( *recipe_it ).yield.amount > 0 ) {
			content += QString("%1: ").arg(i18n("Yields"));
			content += ( *recipe_it ).yield.toString();
			content += "\n";
		}

		if ( !( *recipe_it ).prepTime.isNull() ) {
			content += QString("%1: ").arg(i18n("Preparation Time"));
			content += ( *recipe_it ).prepTime.toString( "hh:mm" );
			content += "\n";
		}

		content += "\n";

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
				
				for ( QValueList<IngredientData>::const_iterator sub_it = (*ing_it).substitutes.begin(); sub_it != (*ing_it).substitutes.end(); ) {
					content += "  "+generateIngredient(*sub_it,number_format);
					sub_it++;
					if ( sub_it != (*ing_it).substitutes.end() )
						content += ", "+i18n("or");
					content += "\n";
				}

				content += "\n";
			}
		}

		content += "\n";

		/// @todo add ingredient properties

		content += ( *recipe_it ).instructions;

		content += "\n\n";

		if ( (*recipe_it).ratingList.count() > 0 )
			content += "----------"+i18n("Ratings")+"----------\n";

		for ( RatingList::const_iterator rating_it = (*recipe_it).ratingList.begin(); rating_it != (*recipe_it).ratingList.end(); ++rating_it ) {
			if ( !( *rating_it ).rater.isEmpty() )
				content += "  "+( *rating_it ).rater+"\n";

			if ( (*rating_it).ratingCriteriaList.size() > 0 )
				content += "\n";

			for ( RatingCriteriaList::const_iterator rc_it = (*rating_it).ratingCriteriaList.begin(); rc_it != (*rating_it).ratingCriteriaList.end(); ++rc_it ) {
				//FIXME: This is an ugly hack, but I don't know how else to be i18n friendly (if this is even that)
				// and still be able to display the amount as a fraction
				QString starsTrans = i18n("1 star","%n stars",qRound((*rc_it).stars));
				starsTrans.replace(QString::number(qRound((*rc_it).stars)),MixedNumber((*rc_it).stars).toString());

				content +=  "  "+(*rc_it).name+": "+starsTrans+"\n";
			}

			if ( (*rating_it).ratingCriteriaList.size() > 0 )
				content += "\n";

			if ( !( *rating_it ).comment.isEmpty() )
				content += "  "+( *rating_it ).comment+"\n";

			content += "\n";
		}

		if ( (*recipe_it).ratingList.size() > 0 )
			content += "\n";

		content += "-----\n\n"; //end of recipe indicator
	}

	return content;
}

