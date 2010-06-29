/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "rezkonvexporter.h"

#include <QRegExp>

#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kapplication.h>
#include <kglobal.h>

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
                                          {"ml", "ml", "ml", "ml"},
                                          {"l", "l", "Ltr.", "Ltr."},
                                          {"kg", "kg", "kg", "kg"},
                                          {"mg", "mg", "mg", "mg"},
                                          {"teaspoon", "teaspoons", "Teel.", "Teel."},
                                          {"t.", "t.", "Teel.", "Teel."},
                                          {"t", "t", "Teel.", "Teel."},
                                          {"tsp.", "tsp.", "Teel.", "Teel."},
                                          {"tsp", "tsp", "Teel.", "Teel."},
                                          {"tablespoon", "tablespoons", "Essl.", "Essl."},
                                          {"tbsp.", "tbsp.", "Essl.", "Essl."},
                                          {"tbsp", "tbsp", "Essl.", "Essl."},
                                          {"T", "T", "Essl.", "Essl."},
                                          {"T.", "T.", "Essl.", "Essl."},
                                          {"cup", "cups", "Tasse", "Tassen"},
                                          {"c.", "c.", "Tasse", "Tassen"},
                                          {"can", "cans", "Dose", "Dosen"},
                                          {"drop", "drops", "Tropfen", "Tropfen"},
                                          {"large", "large", "groß", "groß"},
                                          {"medium", "medium", "mittl.", "mittl."},
                                          {"small", "small", "klein.", "klein."},
                                          {"pinch", "pinches", "Prise", "Prisen"},
                                          {"package", "packages", "Pack.", "Pack."},
                                          {"bunch", "bunches", "Bund.", "Bunde"},
                                          {"stem", "stems", "Strange", "Strangen"},
                                          {"twig", "twigs", "Zweig", "Zweige"},
                                          {"tip of a knife", "tips of a knife", "Messersp.", "Messersp."},
                                          {"sheet", "sheets", "Blatt", "Blätter"},
                                          {"handful", "handfuls", "Handvoll", "Handvoll"},
                                          {"head", "heads", "Kopf", "Köpfe"},
                                          {"slice", "slices", "Scheibe", "Sheiben"},
                                          {"some", "some", "Einige", "Einige"},
                                          {"a little", "a little", "Etwas", "Etwas"},
                                          {"little can", "little cans", "Döschen", "Döschen"},
                                          {"glass", "glasses", "Glas", "Gläser"},
                                          {"piece", "pieces", "Stück", "Stück"},
                                          {"pot", "pots", "Topf", "Topf"},
                                          {"generous", "generous", "Reichlich", "Reichlich"},
                                          {"dash", "dashes", "Spritzer", "Spritzer"},
                                          {"clove", "cloves", "Zehe", "Zehen"},
                                          {"slice", "slices", "Platte", "Platten"},
                                          {"shot", "shots", "Schuss", "Schuss"},
                                          {"peduncle", "peduncles", "Stiel", "Stiele"},
                                          {"heaping teaspoon", "heaping teaspoons", "geh. TL", "geh. TL"},
                                          {"heaping tsp.", "heaping tsp.", "geh. TL", "geh. TL"},
                                          {"heaping tsp.", "heaping tsp.", "geh. TL", "geh. TL"},
                                          {"heaping tablespoon", "heaping tablespoon", "geh. EL", "geh. EL"},
                                          {"heaping tbsp.", "heaping tbsp.", "geh. EL", "geh. EL"},
#if 0
                                          {"pound", "pounds", "", ""},
                                          {"lb.", "lbs.", "", ""},
                                          {"ounce", "ounces", "", ""},
                                          {"oz.", "oz.", "", ""},
#endif
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
		content += '\n';
		writeIngredients( content, *recipe_it );
		writeDirections( content, *recipe_it );
		content += '\n';

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
	content += QString("Titel: ").rightJustified( 13, ' ', true) + title + '\n';

	int i = 0;
	QStringList categories;
	for ( ElementList::const_iterator cat_it = recipe.categoryList.begin(); cat_it != recipe.categoryList.end(); ++cat_it ) {
		i++;

		if ( i == 6 )
			break; //maximum of 5 categories
		categories << ( *cat_it ).name;
	}
	QString cat_str = QString("Kategorien: ").rightJustified( 13, ' ', true) + categories.join( ", " );
	cat_str.truncate( 67 );
	content += cat_str + '\n';

	content += QString("Menge: ").rightJustified( 13, ' ', true) + recipe.yield.toString() + '\n';
}

/* Ingredient lines:
 * Positions 1-7 contains a numeric quantity
 * Positions 9-10 Meal-Master unit of measure codes
 * Positions 12-39 contain text for an ingredient name, or a '-'
 *   in position 12 and text in positions 13-39 (the latter is a
 *   "continuation" line for the previous ingredient name)
 */
void RezkonvExporter::writeIngredients( QString &content, const Recipe &recipe )
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
			QString filler_lt = QString().fill( '=', ( 76 - length ) / 2 );
			QString filler_rt = ( length % 2 ) ? QString().fill( '=', ( 76 - length ) / 2 + 1 ) : filler_lt;
			content += filler_lt + group + filler_rt + '\n';
		}

		for ( IngredientList::const_iterator ing_it = group_list.begin(); ing_it != group_list.end(); ++ing_it ) {
			writeSingleIngredient( content, *ing_it, (*ing_it).substitutes.count() > 0 );

			for ( Ingredient::SubstitutesList::const_iterator sub_it = (*ing_it).substitutes.begin(); sub_it != (*ing_it).substitutes.end(); ) {
				Ingredient::SubstitutesList::const_iterator save_it = sub_it;

				 ++sub_it;
				writeSingleIngredient( content, *save_it, sub_it != (*ing_it).substitutes.end() );
			}
		}
	}

	QString authorLines;
	if ( recipe.authorList.count() > 0 ) {
		content += "============================== QUELLE ==============================\n";
		authorLines = "                   "+(*recipe.authorList.begin()).name+'\n';
	}
	for ( ElementList::const_iterator author_it = ++recipe.authorList.begin(); author_it != recipe.authorList.end(); ++author_it ) {
		authorLines += "                   -- ";
		authorLines += (*author_it).name + '\n';
	}
	if ( !authorLines.isEmpty() )
		authorLines += '\n';
	content += authorLines;
}

void RezkonvExporter::writeSingleIngredient( QString &content, const IngredientData &ing, bool is_sub )
{
	KConfigGroup  config(KGlobal::config() , "Formatting" );
	MixedNumber::Format number_format = config.readEntry( "Fraction" ).isEmpty() ? MixedNumber::MixedNumberFormat : MixedNumber::DecimalFormat;

	//columns 1-7
	if ( ing.amount > 1e-8 ) {
		QString amount_str = MixedNumber( ing.amount ).toString( number_format, false );

		if ( ing.amount_offset > 0 )
			amount_str += '-'+MixedNumber( ing.amount + ing.amount_offset ).toString( number_format, false );

		if ( amount_str.length() > 7 ) { //too long, let's try the other formatting
			MixedNumber::Format other_format = (number_format == MixedNumber::DecimalFormat) ? MixedNumber::MixedNumberFormat : MixedNumber::DecimalFormat;

			QString new_amount_str = MixedNumber( ing.amount ).toString( other_format, false );
	
			if ( ing.amount_offset > 0 )
				new_amount_str += '-'+MixedNumber( ing.amount + ing.amount_offset ).toString( other_format, false );

			if (new_amount_str.length() > 7) { //still too long, use original formatting, but truncate it
				amount_str = amount_str.left(7);
				kDebug()<<"Warning: Amount text too long, truncating";
			}
		}
		content += amount_str.rightJustified( 7, ' ', true ) + ' ';
	}
	else
		content += QString().fill(' ',7+1);

	//columns 9-19
	bool found_translation = false;
	for ( int i = 0; translate_units[ i ].english; i++ ) {
		if ( translate_units[ i ].english == ing.units.name() ||  translate_units[ i ].english_plural == ing.units.plural() ||  translate_units[ i ].german == ing.units.name() ||  translate_units[ i ].german_plural == ing.units.plural() ) {
			found_translation = true;
			QString unit;
			if ( ing.amount > 1 )
				unit += translate_units[i].german;
			else
				unit += translate_units[i].german_plural;
			content += unit.leftJustified( 9 ) + ' ';
			break;
		}
	}
	if ( !found_translation ) {
		kDebug() << "Warning: unable to find German translation for: " << ing.units.name() ;
		kDebug() << "         This ingredient (" << ing.name << ") will be exported without a unit" ;
		content += QString().fill(' ',9+1);
	}

	//columns 21-70
	QString ing_name( ing.name );
	if ( ing.prepMethodList.count() > 0 )
		ing_name += ", " + ing.prepMethodList.join(", ");

	if ( is_sub )
		ing_name += ", or"; //FIXME: what's 'or' in German?

	if ( !found_translation )
		ing_name.prepend( ing.units.determineName(ing.amount, /*useAbbrev=*/false) + ' ' );

	//try and split the ingredient on a word boundary
	int split_index;
	if ( ing_name.length() > 50 ) {
		split_index = ing_name.left(50).lastIndexOf(' ')+1;
		if ( split_index == 0 )
			split_index = 50;
	}
	else
		split_index = 50;

	content += ing_name.left(split_index) + '\n';

	for ( int i = 0; i < ( ing_name.length() - 1 ) / 50; i++ )  //if longer than 50 chars, continue on next line(s)
		content += QString().fill(' ',(7+1)+(9+1)) + '-' + ing_name.mid( 50 * ( i ) + split_index, 50 ) + '\n';
}

void RezkonvExporter::writeDirections( QString &content, const Recipe &recipe )
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
