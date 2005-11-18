/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "rezkonvexporter.h"

#include <qregexp.h>

#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kapplication.h>

#include "backends/recipedb.h"
#include "datablocks/mixednumber.h"

struct translate_unit_info
{
	const char *english;
	const char *english_plural;
	const char *german;
	const char *german_plural;
};

static translate_unit_info translate_units[] = {
                                          {"g", "g", "Gramm", "Gramms" },
                                          {"", "", "", ""},
                                          { 0, 0, 0, 0 }
                                      };

RezkonvExporter::RezkonvExporter( const QString& filename, const QString& format ) :
		BaseExporter( filename, format )
{}


RezkonvExporter::~RezkonvExporter()
{}

int RezkonvExporter::supportedItems() const
{
	return RecipeDB::All ^ RecipeDB::Photo ^ RecipeDB::Ratings;
}

QString RezkonvExporter::createContent( const RecipeList& recipes )
{
	QString content;

	RecipeList::const_iterator recipe_it;
	for ( recipe_it = recipes.begin(); recipe_it != recipes.end(); ++recipe_it ) {
		writeHeader( content, *recipe_it );
		content += "\n";
		writeIngredients( content, *recipe_it );
		content += "\n";
		writeDirections( content, *recipe_it );
		content += "\n";

		content += "=====\n\n"; //end of recipe indicator
	}

	return content;
}

/* Header:
 * Line 1 - contains five hyphens and "Meal-Master" somewhere in the line
 * Line 2 - "Title:" followed by a blank space; maximum of 60 char
 * Line 3 - "Categories:" followed by a blank space; Maximum of 5
 * Line 4 - Numeric quantity representing the # of servings (1-9999)
 */
void RezkonvExporter::writeHeader( QString &content, const Recipe &recipe )
{
	content += QString( "===== Exported by Krecipes v%1 [REZKONV Export Format] =====\n\n" ).arg( krecipes_version() );

	QString title = recipe.title;
	title.truncate( 60 );
	content += "      Titel: " + title + "\n";

	int i = 0;
	QStringList categories;
	for ( ElementList::const_iterator cat_it = recipe.categoryList.begin(); cat_it != recipe.categoryList.end(); ++cat_it ) {
		i++;

		if ( i == 6 )
			break; //maximum of 5 categories
		categories << ( *cat_it ).name;
	}
	QString cat_str = " Kategorien: " + categories.join( ", " );
	cat_str.truncate( 67 );
	content += cat_str + "\n";

	//todo ranges and yield type
	content += "   Menge: " + recipe.yield.toString() + "\n";
}

/* Ingredient lines:
 * Positions 1-7 contains a numeric quantity
 * Positions 9-10 Meal-Master unit of measure codes
 * Positions 12-39 contain text for an ingredient name, or a "-"
 *   in position 12 and text in positions 13-39 (the latter is a
 *   "continuation" line for the previous ingredient name)
 */
void RezkonvExporter::writeIngredients( QString &content, const Recipe &recipe )
{
	//this format requires ingredients without a group to be written first
	for ( IngredientList::const_iterator ing_it = recipe.ingList.begin(); ing_it != recipe.ingList.end(); ++ing_it ) {
		if ( ( *ing_it ).groupID == -1 )
			writeSingleIngredient( content, *ing_it );
	}

	IngredientList list_copy = recipe.ingList;
	for ( IngredientList group_list = list_copy.firstGroup(); group_list.count() != 0; group_list = list_copy.nextGroup() ) {
		if ( group_list[ 0 ].groupID == -1 )  //we already handled this group
			continue;

		QString group = group_list[ 0 ].group.left( 76 ); //just use the first's name... they're all the same
		if ( !group.isEmpty() ) {
			int length = group.length();
			QString filler_lt = QString().fill( '=', ( 76 - length ) / 2 );
			QString filler_rt = ( length % 2 ) ? QString().fill( '=', ( 76 - length ) / 2 + 1 ) : filler_lt;
			content += filler_lt + group + filler_rt + "\n";
		}

		for ( IngredientList::const_iterator ing_it = group_list.begin(); ing_it != group_list.end(); ++ing_it ) {
			writeSingleIngredient( content, *ing_it );
		}
	}

	QString authorLines;
	if ( recipe.authorList.count() > 0 ) {
		content += "============================== QUELLE ==============================\n";
		authorLines = "                   "+(*recipe.authorList.begin()).name+"\n";
	}
	for ( ElementList::const_iterator author_it = ++recipe.authorList.begin(); author_it != recipe.authorList.end(); ++author_it ) {
		authorLines += "                   -- ";
		authorLines += (*author_it).name + "\n";
	}
	if ( !authorLines.isEmpty() )
		authorLines += "\n";
	content += authorLines;
}

void RezkonvExporter::writeSingleIngredient( QString &content, const Ingredient &ing )
{
	KConfig * config = kapp->config();
	config->setGroup( "Formatting" );
	MixedNumber::Format number_format = ( config->readBoolEntry( "Fraction" ) ) ? MixedNumber::MixedNumberFormat : MixedNumber::DecimalFormat;

	//columns 1-7
	if ( ing.amount > 1e-8 ) {
		MixedNumber::Format other_format = (number_format == MixedNumber::DecimalFormat) ? MixedNumber::MixedNumberFormat : MixedNumber::DecimalFormat;

		QString amount_str = MixedNumber( ing.amount ).toString( other_format, false );

		if ( ing.amount_offset > 0 )
			amount_str += "-"+MixedNumber( ing.amount + ing.amount_offset ).toString( other_format, false );

		if ( amount_str.length() > 7 ) { //too long, let's try the other formatting
			QString new_amount_str = MixedNumber( ing.amount ).toString( other_format, false );
	
			if ( ing.amount_offset > 0 )
				new_amount_str += "-"+MixedNumber( ing.amount + ing.amount_offset ).toString( other_format, false );

			if (new_amount_str.length() > 7) { //still too long, use original formatting, but truncate it
				amount_str = amount_str.left(7);
				kdDebug()<<"Warning: Amount text too long, truncating"<<endl;
			}
		}
		content += amount_str.rightJustify( 7, ' ', true ) + " ";
	}
	else
		content += QString().fill(' ',7+1);

	//columns 9-19
	bool found_translation = false;
	for ( int i = 0; translate_units[ i ].english; i++ ) {
		if ( translate_units[ i ].english == ing.units.name ||  translate_units[ i ].english_plural == ing.units.plural ||  translate_units[ i ].german == ing.units.name ||  translate_units[ i ].german_plural == ing.units.plural ) {
			found_translation = true;
			QString unit;
			if ( ing.amount > 1 )
				unit += translate_units[i].german;
			else
				unit += translate_units[i].german_plural;
			content += unit.leftJustify( 9 ) + " ";
			break;
		}
	}
	if ( !found_translation ) {
		kdDebug() << "Warning: unable to find German translation for: " << ing.units.name << endl;
		kdDebug() << "         This ingredient (" << ing.name << ") will be exported without a unit" << endl;
		content += QString().fill(' ',9+1);
	}

	//columns 21-70
	QString ing_name( ing.name );
	if ( ing.prepMethodList.count() > 0 )
		ing_name += "; " + ing.prepMethodList.join(", ");

	if ( !found_translation )
		ing_name.prepend( ( ing.amount > 1 ? ing.units.plural : ing.units.name ) + " " );

	//try and split the ingredient on a word boundry
	int split_index;
	if ( ing_name.length() > 50 ) {
		split_index = ing_name.left(50).findRev(" ")+1;
		if ( split_index == 0 )
			split_index = 50;
	}
	else
		split_index = 50;

	content += ing_name.left(split_index) + "\n";

	for ( unsigned int i = 0; i < ( ing_name.length() - 1 ) / 50; i++ )  //if longer than 50 chars, continue on next line(s)
		content += QString().fill(' ',(7+1)+(9+1)) + "-" + ing_name.mid( 50 * ( i ) + split_index, 50 ) + "\n";
}

void RezkonvExporter::writeDirections( QString &content, const Recipe &recipe )
{
	QStringList lines = QStringList::split("\n",recipe.instructions,true);
	for ( QStringList::const_iterator it = lines.begin(); it != lines.end(); ++it ) {
		content += wrapText( *it, 80 ).join( "\n" ) + "\n";
	}
}

QStringList RezkonvExporter::wrapText( const QString& str, int at ) const
{
	QStringList ret;
	QString copy( str );
	bool stop = false;
	while ( !stop ) {
		QString line( copy.left( at ) );
		if ( line.length() >= copy.length() )
			stop = true;
		else {
			QRegExp rxp( "(\\s\\S*)$", false ); // last word in the new line
			rxp.setMinimal( true );    // one word, only one word, please
			line = line.replace( rxp, "" ); // remove last word
		}
		copy = copy.remove( 0, line.length() );
		line = line.stripWhiteSpace();
		line.prepend( " " );       // indent line by one char
		ret << line; // output of current line
	}

	return ret;
}
