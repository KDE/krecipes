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

#include "datablocks/mixednumber.h"
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

				QString amount_str = MixedNumber( ( *ing_it ).amount ).toString( number_format );

				if ( (*ing_it).amount_offset > 0 )
					amount_str += "-"+MixedNumber( ( *ing_it ).amount + ( *ing_it ).amount_offset ).toString( number_format );
				else if ( ( *ing_it ).amount <= 1e-10 )
					amount_str = "";

				content += amount_str;
				if ( !amount_str.isEmpty() )
					content += " ";

				QString unit_str = ( ( *ing_it ).amount+( *ing_it ).amount_offset > 1 ) ? ( *ing_it ).units.plural : ( *ing_it ).units.name;

				content += unit_str;
				if ( !unit_str.isEmpty() )
					content += " ";

				content += ( *ing_it ).name;

				if ( ( *ing_it ).prepMethodList.count() > 0 )
					content += "; "+( *ing_it ).prepMethodList.join(", ");

				content += "\n";
			}
		}

		content += "\n";

		/// @todo add ingredient properties

		content += ( *recipe_it ).instructions;

		content += "\n\n";

		if ( (*recipe_it).ratingList.count() > 0 )
			content += "Ratings:\n";

		for ( RatingList::const_iterator rating_it = (*recipe_it).ratingList.begin(); rating_it != (*recipe_it).ratingList.end(); ++rating_it ) {
			if ( !( *rating_it ).rater.isEmpty() )
				content += "  "+( *rating_it ).rater+"\n";
			if ( !( *rating_it ).comment.isEmpty() )
				content += "  "+( *rating_it ).comment+"\n";

			for ( RatingCriteriaList::const_iterator rc_it = (*rating_it).ratingCriteriaList.begin(); rc_it != (*rating_it).ratingCriteriaList.end(); ++rc_it ) {
				//FIXME-0.9
				content +=  "  "+(*rc_it).name+": "+QString(i18n("%n star","%n stars",(*rc_it).stars)).arg((*rc_it).stars)+"\n";
			}
			content += "\n";
		}

		content += "\n";

		content += "-----\n\n"; //end of recipe indicator
	}

	return content;
}

