/***************************************************************************
 *   Copyright (C) 2003 by krecipes.sourceforge.net authors                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "mxpimporter.h"

#include <qfile.h>
#include <qtextstream.h>
#include <qstringlist.h>

#include <kapplication.h>
#include <klocale.h>

#include "mixednumber.h"
#include "recipe.h"

MXPImporter::MXPImporter( const QString &file ) : BaseImporter()
{
	QFile input( file );

	if ( input.open( IO_ReadOnly ) )
	{
		QTextStream stream( &input );

		QString line;
		while ( !stream.atEnd() )
		{
			line = stream.readLine().stripWhiteSpace();

			if ( line.simplifyWhiteSpace().contains("Exported from MasterCook") )
				{importMXP( stream );}
			else if ( line == "{ Exported from MasterCook Mac }" )
				{importMac( stream );}
			else if ( line == "@@@@@" )
				{importGeneric( stream );}
		}
	}
	else
		setErrorMsg(i18n("Unable to open file."));
}

MXPImporter::~MXPImporter()
{
}

void MXPImporter::importMXP( QTextStream &stream )
{
	kapp->processEvents(); //don't want the user to think its frozen... especially for files with thousands of recipes

	qDebug("Found recipe MXP format: * Exported from MasterCook *");
	QString current;

	// title
	stream.skipWhiteSpace();
	m_title = stream.readLine().stripWhiteSpace();
	qDebug("Found title: %s", m_title.latin1());

	//author
	stream.skipWhiteSpace();
	current = stream.readLine().stripWhiteSpace();
	if ( current.mid( 0, current.find(":") ).simplifyWhiteSpace().lower() == "recipe by" )
	{
		Element new_author;
		new_author.name = current.mid( current.find(":")+1, current.length() ).stripWhiteSpace();
		m_authors.add( new_author );
		qDebug("Found author: %s", new_author.name.latin1());
	}
	else
	{
		addWarningMsg(QString(i18n("While loading recipe \"%1\""
		  "the field \"Recipe By:\" is either missing or could not be detected.")).arg(m_title));
	}

	//servings
	stream.skipWhiteSpace();
	current = stream.readLine().stripWhiteSpace();
	if ( current.mid( 0, current.find(":") ).simplifyWhiteSpace().lower() == "serving size" )
	{
		//allows serving size to be loaded even if preparation time is missing
		int end_index;
		if ( current.contains("preparation time",FALSE) )
			end_index = current.find("preparation time",0,FALSE) - 15;
		else
		 	end_index = current.length();

		m_servings = current.mid( current.find(":")+1, end_index ).stripWhiteSpace().toInt();
		qDebug("Found serving size: %d",m_servings);
	}
	else
	{
		addWarningMsg(QString(i18n("While loading recipe \"%1\""
		  "the field \"Serving Size:\" is either missing or could not be detected.")).arg(m_title));
	}

	if ( current.contains("preparation time",FALSE) )
	{
		m_prep_time = current.mid( current.find(":",current.find("preparation time",0,FALSE)) + 1,
		  current.length() ).stripWhiteSpace();
		qDebug("Found preparation time: %s",m_prep_time.latin1());
	}
	else
	{
		addWarningMsg(QString(i18n("While loading recipe \"%1\""
		  "the field \"Preparation Time:\" is either missing or could not be detected.")).arg(m_title));
	}

	//====================categories====================//
	stream.skipWhiteSpace();
	current = stream.readLine().stripWhiteSpace();
	if ( current.mid( 0, current.find(":") ).simplifyWhiteSpace().lower() == "categories" )
	{
		QString tmp_str = current.mid( current.find(":")+1, current.length() ).stripWhiteSpace();
		if ( tmp_str != "" )
		{
			while ( current != "" && current.stripWhiteSpace() != "Amount  Measure       Ingredient -- Preparation Method" && !stream.atEnd() )
			{
				QStringList categories = QStringList::split( "  ", tmp_str );
				for ( QStringList::const_iterator it = categories.begin(); it != categories.end(); ++it )
				{
					Element new_cat;
					new_cat.name = (*it).stripWhiteSpace();
					m_categories.add( new_cat );

					qDebug("Found category: %s", new_cat.name.latin1());
				}

				current = stream.readLine();
				tmp_str = current;
			}
		}
		else
			qDebug("No categories found.");
	}
	else
	{
		addWarningMsg(QString(i18n("While loading recipe \"%1\""
		  "the field \"Categories:\" is either missing or could not be detected.")).arg(m_title));
	}


	//============ingredients=================//
	stream.skipWhiteSpace();
	(void)stream.readLine(); (void)stream.readLine();
	current = stream.readLine();
	if ( !current.contains("NONE") && current != "" )
	{
		while ( current != "" && !stream.atEnd() )
		{
			Ingredient new_ingredient;

			//amount
			QString amount_str = current.mid(0, 9).simplifyWhiteSpace();
			if ( amount_str == "" )
				new_ingredient.amount = 0;
			else
			{
				bool ok;
				MixedNumber amount(MixedNumber::fromString(amount_str,&ok));
				if ( !ok )
				{
					addWarningMsg( QString(i18n("While loading recipe \"%1\" Invalid amount \"%2\" in the line \"%3\"")).arg(m_title).arg(amount_str).arg(current.stripWhiteSpace()) );
					current = stream.readLine();
					continue;
				}
				new_ingredient.amount = amount.toDouble();
			}

			//units
			QString units(current.mid(9, 13));
			new_ingredient.units = units.simplifyWhiteSpace();

			//name
			int dash_index = current.find("--");

			int length;
			if ( dash_index == -1 )
				length = current.length();
			else
				length = dash_index-22;

			QString ingredient_name(current.mid(22, length));
			new_ingredient.name = ingredient_name.stripWhiteSpace();

			//preparation method (Krecipes doesn't yet use this, but have it here for the future)
			QString prep_method;
			if ( dash_index != -1 )
			{
				QString prep_method = current.mid( dash_index + 2, current.length() );
				if ( prep_method != "" )
					new_ingredient.name += " -- " + prep_method.stripWhiteSpace();
			}

			m_ingredients.add( new_ingredient );
			qDebug("Found ingredient: amount=%f, unit:%s, name:%s, prep_method:%s",
			  new_ingredient.amount,
			  new_ingredient.units.latin1(),
			  new_ingredient.name.latin1(),
			  prep_method.latin1());

			current = stream.readLine();
		}
	}
	else
		qDebug("No ingredients found.");

	//==========================instructions ( along with other optional fields... mxp format doesn't define end of ingredients and start of other fields )==============//
	stream.skipWhiteSpace();
	current = stream.readLine().stripWhiteSpace();
	while ( !current.contains("- - - -" ) && !stream.atEnd() )
	{
		if ( current.stripWhiteSpace() == "Source:" )
		{
			Element new_author;
			new_author.name = getNextQuotedString(stream);
			m_authors.add( new_author );
			qDebug("Found source: %s (adding as author)",new_author.name.latin1());
		}
		else if ( current.stripWhiteSpace() == "Description:" )
		{
			m_description = getNextQuotedString(stream);
			qDebug("Found description: %s (adding to end of instructions)",m_description.latin1());
			m_instructions += "\n\nDescription: " + m_description;
		}
		else if ( current.stripWhiteSpace() == "S(Internet Address):" )
		{
			m_internet = getNextQuotedString(stream);
			qDebug("Found internet address: %s (adding to end of instructions)",m_internet.latin1());
			m_instructions += "\n\nInternet address: " + m_internet;
		}
		else if ( current.stripWhiteSpace() == "Yield:" )
		{
			m_servings = getNextQuotedString(stream).stripWhiteSpace().toInt();
			qDebug("Found yield: %d (adding as servings)",m_servings);
		}
		else if ( current.stripWhiteSpace() == "T(Cook Time):" )
		{
			m_prep_time = getNextQuotedString(stream);
			qDebug("Found cook time: %s (adding as prep time)",m_prep_time.latin1());
		}
		else if ( current.stripWhiteSpace() == "Cuisine:" )
		{
			Element new_cat;
			new_cat.name = getNextQuotedString(stream);
			m_categories.add( new_cat );
			qDebug("Found cuisine (adding as category): %s",new_cat.name.latin1());
		}
		else
			m_instructions += current + "\n";

		current = stream.readLine().stripWhiteSpace();
	}
	m_instructions = m_instructions.stripWhiteSpace();
	qDebug("Found instructions: %s", m_instructions.latin1());

	//=================after here, fields are optional=========================//
	stream.skipWhiteSpace();
	current = stream.readLine().stripWhiteSpace();

	while ( !current.simplifyWhiteSpace().contains("Exported from MasterCook") && !stream.atEnd() )
	{
		//suggested wine
		if ( current.mid( 0, current.find(":") ).simplifyWhiteSpace().lower() == "suggested wine" )
		{
			m_wine = current.mid( current.find(":")+1, current.length() ).stripWhiteSpace();
			qDebug("Found suggested wine: %s (adding to end of instructions)", m_wine.latin1());

			m_instructions += "\n\nSuggested wine: " + m_wine;
		}
		//Nutr. Assoc.
		if ( current.mid( 0, current.find(":") ).simplifyWhiteSpace().lower() == "nutr. assoc." )
		{
			QString nutr_assoc = current.mid( current.find(":")+1, current.length() ).stripWhiteSpace();
			qDebug("Found nutrient association: %s (adding to end of instructions)", nutr_assoc.latin1());

			m_instructions += "\n\nNutrient Association: " + nutr_assoc;
		}
		else if ( current.mid( 0, current.find(":") ).simplifyWhiteSpace().lower() == "per serving (excluding unknown items)" )
		{ //per serving... maybe we can do something with this info later
			QString per_serving_info = current.mid( current.find(":")+1, current.length() ).stripWhiteSpace();
			qDebug("Found per serving (excluding unknown items): %s (adding to end of instructions)", per_serving_info.latin1());

			m_instructions += "\n\nPer Serving (excluding unknown items): " + per_serving_info;
		}
		else if ( current.mid( 0, current.find(":") ).simplifyWhiteSpace().lower() == "per serving" )
		{ //per serving... maybe we can do something with this info later
			QString per_serving_info = current.mid( current.find(":")+1, current.length() ).stripWhiteSpace();
			qDebug("Found per serving: %s (adding to end of instructions)", per_serving_info.latin1());

			m_instructions += "\n\nPer Serving: " + per_serving_info;
		}
		else if ( current.mid( 0, current.find(":") ).simplifyWhiteSpace().lower() == "food exchanges" )
		{ //food exchanges... maybe we can do something with this info later
			QString food_exchange_info = current.mid( current.find(":")+1, current.length() ).stripWhiteSpace();
			qDebug("Found food exchanges: %s (adding to end of instructions)", food_exchange_info.latin1());

			m_instructions += "\n\nFood Exchanges: " + food_exchange_info;
		}
		else if ( current.mid( 0, current.find(":") ).simplifyWhiteSpace().lower() == "serving ideas" )
		{ //serving ideas
			m_serving_ideas = current.mid( current.find(":")+1, current.length() ).stripWhiteSpace();
			qDebug("Found serving ideas: %s (adding to end of instructions)", m_serving_ideas.latin1());

			m_instructions += "\n\nServing ideas: " + m_serving_ideas;
		}
		else if ( current.mid( 0, current.find(":") ).simplifyWhiteSpace().lower() == "notes" ) //notes
			m_notes = current.mid( current.find(":")+1, current.length() ).stripWhiteSpace();
		else if ( current != "" && current != "_____" ) //if it doesn't belong to any other field, assume it a part of a multi-line notes field
			m_notes += "\n" + current;

		current = stream.readLine().stripWhiteSpace();
	}

	/*possible fields to implement later:

          Nutr. Assoc. : 0 0 0 0 0

Ratings       : Cholesterol Rating 5            Complete Meal 3
                Cost 3                          Depth 3
                Difficulty 2                    Fanciness 7
                Fat Content 5                   Good For Crowds 10
                Intensity 5                     Intricacy 2
                Kid Appeal 3                    Looks 5
                Portability 3                   Richness 7
                Serving Temperature 8           Spicy Hotness 2
                Tartness 7

	*/
	if ( !m_notes.isNull() )
	{
		qDebug("Found notes: %s (adding to end of instructions)", m_notes.latin1());
		m_instructions += "\n\nNotes: " + m_notes.stripWhiteSpace();
	}

	putDataInRecipe();

	if ( !stream.atEnd() )
	{
		importMXP( stream );
		return;
	}
}

void MXPImporter::importGeneric( QTextStream &stream )
{
//not even sure it this is worth writing... its rather obsolete
}

void MXPImporter::importMac( QTextStream &stream )
{
//not even sure it this is worth writing... its rather obsolete
}

void MXPImporter::putDataInRecipe()
{
	//create the recipe
	Recipe *new_recipe = new Recipe;
	new_recipe->persons = m_servings;
	new_recipe->title = m_title;
	new_recipe->instructions = m_instructions;
	new_recipe->ingList = m_ingredients;
	new_recipe->categoryList = m_categories;
	new_recipe->authorList = m_authors;
	new_recipe->recipeID = -1;

	//put it in the recipe list
	add( new_recipe );

	//reset for the next recipe to use these variables
	m_ingredients.empty();
	m_authors.clear();
	m_categories.clear();

	m_servings = 0;

	m_description = QString::null;
	m_instructions = QString::null;
	m_internet = QString::null;
	m_notes = QString::null;
	m_prep_time = QString::null;
	m_serving_ideas = QString::null;
	m_source = QString::null;
	m_title = QString::null;
	m_wine = QString::null;
}

QString MXPImporter::getNextQuotedString( QTextStream &stream )
{
	stream.skipWhiteSpace();
	QString current = stream.readLine().stripWhiteSpace();
	QString return_str;

	if ( current.left( 1 ) == "\"" )
		return_str = current.mid( 1, current.length()-1 );
	else
		return current;

	while ( current.right( 1 ) != "\"" && !stream.atEnd() )
	{
		current = stream.readLine().stripWhiteSpace();
		return_str += "\n"+current;
	}

	//take off quote at end
	return_str = return_str.mid( 0, return_str.length() - 1);

	return return_str.stripWhiteSpace();
}
