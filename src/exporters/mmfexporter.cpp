/***************************************************************************
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "mmfexporter.h"

#include <QRegExp>

#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kapplication.h>
#include <kglobal.h>

#include "backends/recipedb.h"
#include "datablocks/mixednumber.h"
#include "mmdata.h"

MMFExporter::MMFExporter( const QString& filename, const QString& format ) :
		BaseExporter( filename, format )
{}


MMFExporter::~MMFExporter()
{}

int MMFExporter::supportedItems() const
{
	return RecipeDB::All ^ RecipeDB::Photo ^ RecipeDB::Ratings ^ RecipeDB::Authors;
}

QString MMFExporter::createContent( const RecipeList& recipes )
{
	QString content;

	RecipeList::const_iterator recipe_it;
	for ( recipe_it = recipes.begin(); recipe_it != recipes.end(); ++recipe_it ) {
		writeMMFHeader( content, *recipe_it );
		content += '\n';
		writeMMFIngredients( content, *recipe_it );
		content += '\n';
		writeMMFDirections( content, *recipe_it );
		content += '\n';

		content += "-----\n"; //end of recipe indicator
	}

	return content;
}

/* Header:
 * Line 1 - contains five hyphens and "Meal-Master" somewhere in the line
 * Line 2 - "Title:" followed by a blank space; maximum of 60 char
 * Line 3 - "Categories:" followed by a blank space; Maximum of 5
 * Line 4 - Numeric quantity representing the # of servings (1-9999)
 */
void MMFExporter::writeMMFHeader( QString &content, const Recipe &recipe )
{
	content += QString( "----- Exported by Krecipes v%1 [Meal-Master Export Format] -----\n\n" ).arg( krecipes_version() );

	QString title = recipe.title;
	title.truncate( 60 );
	content += "      Title: " + title + '\n';

	int i = 0;
	QStringList categories;
	for ( ElementList::const_iterator cat_it = recipe.categoryList.begin(); cat_it != recipe.categoryList.end(); ++cat_it ) {
		i++;

		if ( i == 6 )
			break; //maximum of 5 categories
		categories << ( *cat_it ).name;
	}
	QString cat_str = " Categories: " + categories.join( ", " );
	cat_str.truncate( 67 );
	content += cat_str + '\n';

	content += "   Servings: " + QString::number( qMin( 9999.0, recipe.yield.amount() ) ) + '\n'; //some yield info is lost here, but hey, that's the MM format
}

/* Ingredient lines:
 * Positions 1-7 contains a numeric quantity
 * Positions 9-10 Meal-Master unit of measure codes
 * Positions 12-39 contain text for an ingredient name, or a "-"
 *   in position 12 and text in positions 13-39 (the latter is a
 *   "continuation" line for the previous ingredient name)
 */
void MMFExporter::writeMMFIngredients( QString &content, const Recipe &recipe )
{
	//this format requires ingredients without a group to be written first
	for ( IngredientList::const_iterator ing_it = recipe.ingList.begin(); ing_it != recipe.ingList.end(); ++ing_it ) {
		if ( ( *ing_it ).groupID == -1 ) {
			writeSingleIngredient( content, *ing_it, (*ing_it).substitutes.count() > 0 );

			for ( Ingredient::SubstitutesList::const_iterator sub_it = (*ing_it).substitutes.begin(); sub_it != (*ing_it).substitutes.end(); ) {
				Ingredient::SubstitutesList::const_iterator save_it = sub_it;

				 ++sub_it;
				writeSingleIngredient( content, *save_it, sub_it != (*ing_it).substitutes.end() );
			}
		}
	}

	IngredientList list_copy = recipe.ingList;
	for ( IngredientList group_list = list_copy.firstGroup(); group_list.count() != 0; group_list = list_copy.nextGroup() ) {
		if ( group_list[ 0 ].groupID == -1 )  //we already handled this group
			continue;

		QString group = group_list[ 0 ].group.left( 76 ); //just use the first's name... they're all the same
		if ( !group.isEmpty() ) {
			int length = group.length();
			QString filler_lt = QString().fill( '-', ( 76 - length ) / 2 );
			QString filler_rt = ( length % 2 ) ? QString().fill( '-', ( 76 - length ) / 2 + 1 ) : filler_lt;
			content += filler_lt + group + filler_rt + '\n';
		}

		for ( IngredientList::const_iterator ing_it = group_list.begin(); ing_it != group_list.end(); ++ing_it ) {
			writeSingleIngredient( content, *ing_it, (*ing_it).substitutes.count() > 0  );

			for ( Ingredient::SubstitutesList::const_iterator sub_it = (*ing_it).substitutes.begin(); sub_it != (*ing_it).substitutes.end(); ) {
				Ingredient::SubstitutesList::const_iterator save_it = sub_it;

				 ++sub_it;
				writeSingleIngredient( content, *save_it, sub_it != (*ing_it).substitutes.end() );
			}
		}
	}
}

void MMFExporter::writeSingleIngredient( QString &content, const Ingredient &ing, bool is_sub )
{
	KConfigGroup  config = KConfigGroup(KGlobal::config(), "Formatting" );
	MixedNumber::Format number_format = ( config.readEntry( "Fraction" ) ).isEmpty() ? MixedNumber::MixedNumberFormat : MixedNumber::DecimalFormat;

	//columns 1-7
	if ( ing.amount > 0 )
		content += MixedNumber( ing.amount ).toString( number_format, false ).rightJustified( 7, ' ', true ) + ' ';
	else
		content += "        ";

	//columns 9-10
	bool found_short_form = false;
	for ( int i = 0; unit_info[ i ].short_form; i++ ) {
		if ( unit_info[ i ].expanded_form == ing.units.name() ||
		        unit_info[ i ].plural_expanded_form == ing.units.plural() ||
		        unit_info[ i ].short_form == ing.units.name() ) {
			found_short_form = true;
			content += QString( unit_info[ i ].short_form ).leftJustified( 2 ) + ' ';
			break;
		}
	}
	if ( !found_short_form ) {
		kDebug() << "Warning: unable to find Meal-Master abbreviation for: " << ing.units.name() ;
		kDebug() << "         This ingredient (" << ing.name << ") will be exported without a unit" ;
		content += "   ";
	}

	//columns 12-39
	QString ing_name( ing.name );
	if ( ing.prepMethodList.count() > 0 )
		ing_name += "; " + ing.prepMethodList.join(", ");

	if ( is_sub )
		ing_name += ", or";

	if ( !found_short_form )
		ing_name.prepend( ing.units.determineName(ing.amount, /*useAbbrev=*/false) + ' ' );

	//try and split the ingredient on a word boundary
	int split_index;
	if ( ing_name.length() > 28 ) {
		split_index = ing_name.left(28).lastIndexOf(" ")+1;
		if ( split_index == 0 )
			split_index = 28;
	}
	else
		split_index = 28;

	content += ing_name.left(split_index) + '\n';

	for ( int i = 0; i < ( ing_name.length() - 1 ) / 28; i++ )  //if longer than 28 chars, continue on next line(s)
		content += "           -" + ing_name.mid( 28 * ( i ) + split_index, 28 ) + '\n';
}

void MMFExporter::writeMMFDirections( QString &content, const Recipe &recipe )
{
	QStringList lines;
   if (recipe.instructions.isEmpty())
       lines = QStringList();
   else
      lines = recipe.instructions.split( '\n', QString::KeepEmptyParts);
	
   for ( QStringList::const_iterator it = lines.constBegin(); it != lines.constEnd(); ++it ) {
		content += wrapText( *it, 80 ).join( "\n" ) + '\n';
	}
}

QStringList MMFExporter::wrapText( const QString& str, int at ) const
{
	QStringList ret;
	QString copy( str );
	bool stop = false;
	while ( !stop ) {
		QString line( copy.left( at ) );
		if ( line.length() >= copy.length() )
			stop = true;
		else {
			QRegExp rxp( "(\\s\\S*)$", Qt::CaseInsensitive ); // last word in the new line
			rxp.setMinimal( true );    // one word, only one word, please
			line = line.remove( rxp ); // remove last word
		}
		copy = copy.remove( 0, line.length() );
		line = line.trimmed();
		line.prepend( ' ' );       // indent line by one char
		ret << line; // output of current line
	}

	return ret;
}
