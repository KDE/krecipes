/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "mmfexporter.h"

#include <qregexp.h>

#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kapplication.h>

#include "mixednumber.h"
#include "mmdata.h"

MMFExporter::MMFExporter( RecipeDB *db, const QString& filename, const QString format ) :
  BaseExporter( db, filename, format )
{
}


MMFExporter::~MMFExporter()
{
}

QString MMFExporter::createContent( const QPtrList<Recipe>& recipes )
{
	QString content;

	QPtrListIterator<Recipe> recipes_it( recipes );
	Recipe *recipe;
	while ( (recipe = recipes_it.current()) != 0 )
	{
		++recipes_it;

		writeMMFHeader( content, recipe ); content += "\n";
		writeMMFIngredients( content, recipe ); content += "\n";
		writeMMFDirections( content, recipe ); content += "\n";

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
void MMFExporter::writeMMFHeader( QString &content, Recipe *recipe )
{
	content += QString("----- Exported by Krecipes v%1 [Meal-Master Export Format] -----\n\n").arg(krecipes_version());

	QString title = recipe->title; title.truncate(60);
	content += "      Title: "+title+"\n";

	QPtrListIterator<Element> cat_it( recipe->categoryList );
	Element *cat; int i = 0; QStringList categories;
	while ( (cat = cat_it.current()) != 0 )
	{
		++cat_it; i++;

		if ( i == 6 ) break; //maximum of 5 categories

		categories << cat->name;
	}
	QString cat_str =  " Categories: "+categories.join(", "); cat_str.truncate(67);
	content += cat_str+"\n";

	content += "   Servings: "+QString::number(QMIN(9999,recipe->persons))+"\n";
}

/* Ingredient lines:
 * Positions 1-7 contains a numeric quantity
 * Positions 9-10 Meal-Master unit of measure codes
 * Positions 12-39 contain text for an ingredient name, or a "-"
 *   in position 12 and text in positions 13-39 (the latter is a
 *   "continuation" line for the previous ingredient name)
 */
void MMFExporter::writeMMFIngredients( QString &content, Recipe *recipe )
{
	KConfig *config = kapp->config();
	MixedNumber::Format number_format = (config->readBoolEntry("Fraction")) ? MixedNumber::MixedNumberFormat : MixedNumber::DecimalFormat;

	QPtrListIterator<Ingredient> ing_it( recipe->ingList );
	Ingredient *ing;
	while ( (ing = ing_it.current()) != 0 )
	{
		++ing_it;

		//columns 1-7
		if ( ing->amount > 0 )
			content += MixedNumber(ing->amount).toString( number_format ).rightJustify(7,' ',true)+" ";
		else
			content += "        ";

		//columns 9-10
		bool found_short_form = false;
		for ( int i = 0; unit_info[i].short_form; i++ )
		{
			if ( unit_info[i].expanded_form == ing->units ||
			     unit_info[i].plural_expanded_form == ing->units ||
			     unit_info[i].short_form == ing->units )
			{
				found_short_form = true;
				content += QString(unit_info[i].short_form).leftJustify(2)+" ";
				break;
			}
		}
		if ( !found_short_form )
		{
			kdDebug()<<"Warning: unable to find Meal-Master abbreviation for: "<<ing->units<<endl;
			kdDebug()<<"         This ingredient ("<<ing->name<<") will be exported without a unit"<<endl;
			content += "   ";
		}

		//columns 12-39
		QString ing_name = ing->name; ing_name.truncate(28);
		content += ing_name+"\n";
		for ( unsigned int i = 0; i < (ing->name.length()-1) / 28; i++ ) //if longer than 28 chars, continue on next line(s)
			content += "           -"+ing->name.mid(28*(i+1),28)+"\n";
	}
}

void MMFExporter::writeMMFDirections( QString &content, Recipe *recipe )
{
	content += wrapText(recipe->instructions,80).join("\n")+"\n";
}

QStringList MMFExporter::wrapText( const QString& str, int at) const
{
	QStringList ret;
	QString copy(str);
	bool stop = false;
	while (!stop)
	{
		QString line( copy.left( at));
		if (line.length() >= copy.length()) stop = true;
		QRegExp rxp( "(\\s\\S*)$", false); // last word in the new line
		rxp.setMinimal( true);    // one word, only one word, please
		line = line.replace( rxp, ""); // remove last word
		copy = copy.remove( 0, line.length());
		line = line.simplifyWhiteSpace();
		line.prepend( " ");       // indent line by one char
		ret << line; // output of current line
	}

	return ret;
}
