/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
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
#include <kdebug.h>

#include "mixednumber.h"
#include "recipe.h"

MXPImporter::MXPImporter( const QString &file ) : BaseImporter()
{
	QFile input( file );

	if ( input.open( IO_ReadOnly ) )
	{
		QTextStream stream( &input );
		stream.skipWhiteSpace();

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
			else
			{
				setErrorMsg( i18n("File does not appear to be a valid MasterCook Export.") );
				return;
			}

			stream.skipWhiteSpace();
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
	Recipe recipe;

	kapp->processEvents(); //don't want the user to think its frozen... especially for files with thousands of recipes

	//kdDebug()<<"Found recipe MXP format: * Exported from MasterCook *"<<endl;
	QString current;

	// title
	stream.skipWhiteSpace();
	recipe.title = stream.readLine().stripWhiteSpace();
	//kdDebug()<<"Found title: "<<m_title<<endl;

	//author
	stream.skipWhiteSpace();
	current = stream.readLine().stripWhiteSpace();
	if ( current.mid( 0, current.find(":") ).simplifyWhiteSpace().lower() == "recipe by" )
	{
		Element new_author( current.mid( current.find(":")+1, current.length() ).stripWhiteSpace() );
		recipe.authorList.append( new_author );
		//kdDebug()<<"Found author: "<<new_author.name<<endl;
	}
	else
	{
		addWarningMsg(QString(i18n("While loading recipe \"%1\" "
		  "the field \"Recipe By:\" is either missing or could not be detected.")).arg(recipe.title));
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

		recipe.persons = current.mid( current.find(":")+1, end_index ).stripWhiteSpace().toInt();
		//kdDebug()<<"Found serving size: "<<recipe.persons<<endl;
	}
	else
	{
		addWarningMsg(QString(i18n("While loading recipe \"%1\" "
		  "the field \"Serving Size:\" is either missing or could not be detected.")).arg(recipe.title));
	}

	if ( current.contains("preparation time",FALSE) )
	{
		//QString prep_time = current.mid( current.find(":",current.find("preparation time",0,FALSE)) + 1,
		//  current.length() ).stripWhiteSpace();
		//kdDebug()<<"Found preparation time: "<<prep_time<<endl;
	}
	else
	{
		addWarningMsg(QString(i18n("While loading recipe \"%1\" "
		  "the field \"Preparation Time:\" is either missing or could not be detected.")).arg(recipe.title));
	}

	loadCategories( stream, recipe );
	loadIngredients( stream, recipe );
	loadInstructions( stream, recipe );
	loadOptionalFields( stream, recipe );
	
	add( recipe );
	
	if ( !stream.atEnd() )
	{
		importMXP( stream );
		return;
	}
}

void MXPImporter::loadCategories( QTextStream &stream, Recipe &recipe )
{
	//====================categories====================//
	stream.skipWhiteSpace();
	QString current = stream.readLine().stripWhiteSpace();
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
					Element new_cat( (*it).stripWhiteSpace() );
					recipe.categoryList.append( new_cat );

					//kdDebug()<<"Found category: "<<new_cat.name<<endl;
				}

				current = stream.readLine();
				tmp_str = current;
			}
		}
		//else
		//	kdDebug()<<"No categories found."<<endl;
	}
	else
	{
		addWarningMsg(QString(i18n("While loading recipe \"%1\" "
		  "the field \"Categories:\" is either missing or could not be detected.")).arg(recipe.title));
	}
}

void MXPImporter::loadIngredients( QTextStream &stream, Recipe &recipe )
{
	//============ingredients=================//
	stream.skipWhiteSpace();
	(void)stream.readLine(); (void)stream.readLine();
	QString current = stream.readLine();
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
					addWarningMsg( QString(i18n("While loading recipe \"%1\" Invalid amount \"%2\" in the line \"%3\"")).arg(recipe.title).arg(amount_str).arg(current.stripWhiteSpace()) );
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

			recipe.ingList.append( new_ingredient );
			//kdDebug()<<"Found ingredient: amount="<<new_ingredient.amount
			//  <<", unit:"<<new_ingredient.units
			//  <<", name:"<<new_ingredient.name
			//  <<", prep_method:"<<prep_method<<endl;

			current = stream.readLine();
		}
	}
	//else
	//	kdDebug()<<"No ingredients found."<<endl;
}

void MXPImporter::loadInstructions( QTextStream &stream, Recipe &recipe )
{
	//==========================instructions ( along with other optional fields... mxp format doesn't define end of ingredients and start of other fields )==============//
	stream.skipWhiteSpace();
	QString current = stream.readLine().stripWhiteSpace();
	while ( !current.contains("- - - -" ) && !stream.atEnd() )
	{
		if ( current.stripWhiteSpace() == "Source:" )
		{
			Element new_author( getNextQuotedString(stream) );
			recipe.authorList.append( new_author );
			//kdDebug()<<"Found source: "<<new_author.name<<" (adding as author)"<<endl;
		}
		else if ( current.stripWhiteSpace() == "Description:" )
		{
			QString description = getNextQuotedString(stream);
			//kdDebug()<<"Found description: "<<m_description<<" (adding to end of instructions)"<<endl;
			recipe.instructions += "\n\nDescription: " + description;
		}
		else if ( current.stripWhiteSpace() == "S(Internet Address):" )
		{
			QString internet = getNextQuotedString(stream);
			//kdDebug()<<"Found internet address: "<<m_internet<<" (adding to end of instructions)"<<endl;
			recipe.instructions += "\n\nInternet address: " + internet;
		}
		else if ( current.stripWhiteSpace() == "Yield:" )
		{
			recipe.persons = getNextQuotedString(stream).stripWhiteSpace().toInt();
			//kdDebug()<<"Found yield: "<<m_servings<<" (adding as servings)"<<endl;
		}
		else if ( current.stripWhiteSpace() == "T(Cook Time):" )
		{
			(void)getNextQuotedString(stream); //this would be prep time, but we don't use prep time at the moment
			//kdDebug()<<"Found cook time: "<<m_prep_time<<" (adding as prep time)"<<endl;
		}
		else if ( current.stripWhiteSpace() == "Cuisine:" )
		{
			Element new_cat( getNextQuotedString(stream) );
			recipe.categoryList.append( new_cat );
			//kdDebug()<<"Found cuisine (adding as category): "<<new_cat.name<<endl;
		}
		else
			recipe.instructions += current + "\n";

		current = stream.readLine().stripWhiteSpace();
	}
	recipe.instructions = recipe.instructions.stripWhiteSpace();
	//kdDebug()<<"Found instructions: "<<m_instructions<<endl;
}

void MXPImporter::loadOptionalFields( QTextStream &stream, Recipe &recipe )
{
	//=================after here, fields are optional=========================//
	stream.skipWhiteSpace();
	QString current = stream.readLine().stripWhiteSpace();

	QString notes;

	//Note: we simplifyWhiteSpace() because some versions of MasterCook have "Exported from MasterCook" and others have "Exported  from MasterCook".
	//      This also could work around a typo or such.
	while ( !current.simplifyWhiteSpace().contains("Exported from MasterCook") && !stream.atEnd() )
	{
		//suggested wine
		if ( current.mid( 0, current.find(":") ).simplifyWhiteSpace().lower() == "suggested wine" )
		{
			QString wine = current.mid( current.find(":")+1, current.length() ).stripWhiteSpace();
			//kdDebug()<<"Found suggested wine: "<<m_wine<<" (adding to end of instructions)"<<endl;

			recipe.instructions += "\n\nSuggested wine: " + wine;
		}
		//Nutr. Assoc.
		if ( current.mid( 0, current.find(":") ).simplifyWhiteSpace().lower() == "nutr. assoc." )
		{
			QString nutr_assoc = current.mid( current.find(":")+1, current.length() ).stripWhiteSpace();
			//kdDebug()<<"Found nutrient association: "<<nutr_assoc<<" (adding to end of instructions)"<<endl;

			recipe.instructions += "\n\nNutrient Association: " + nutr_assoc;
		}
		else if ( current.mid( 0, current.find(":") ).simplifyWhiteSpace().lower() == "per serving (excluding unknown items)" )
		{ //per serving... maybe we can do something with this info later
			QString per_serving_info = current.mid( current.find(":")+1, current.length() ).stripWhiteSpace();
			//kdDebug()<<"Found per serving (excluding unknown items): "<<per_serving_info<<" (adding to end of instructions)"<<endl;

			recipe.instructions += "\n\nPer Serving (excluding unknown items): " + per_serving_info;
		}
		else if ( current.mid( 0, current.find(":") ).simplifyWhiteSpace().lower() == "per serving" )
		{ //per serving... maybe we can do something with this info later
			QString per_serving_info = current.mid( current.find(":")+1, current.length() ).stripWhiteSpace();
			//kdDebug()<<"Found per serving: "<<per_serving_info<<" (adding to end of instructions)"<<endl;

			recipe.instructions += "\n\nPer Serving: " + per_serving_info;
		}
		else if ( current.mid( 0, current.find(":") ).simplifyWhiteSpace().lower() == "food exchanges" )
		{ //food exchanges... maybe we can do something with this info later
			QString food_exchange_info = current.mid( current.find(":")+1, current.length() ).stripWhiteSpace();
			//kdDebug()<<"Found food exchanges: "<<food_exchange_info<<" (adding to end of instructions)"<<endl;

			recipe.instructions += "\n\nFood Exchanges: " + food_exchange_info;
		}
		else if ( current.mid( 0, current.find(":") ).simplifyWhiteSpace().lower() == "serving ideas" )
		{ //serving ideas
			QString serving_ideas = current.mid( current.find(":")+1, current.length() ).stripWhiteSpace();
			//kdDebug()<<"Found serving ideas: "<<m_serving_ideas<<" (adding to end of instructions)"<<endl;

			recipe.instructions += "\n\nServing ideas: " + serving_ideas;
		}
		else if ( current.mid( 0, current.find(":") ).simplifyWhiteSpace().lower() == "notes" ) //notes
			notes = current.mid( current.find(":")+1, current.length() ).stripWhiteSpace();
		else if ( current != "" && current != "_____" ) //if it doesn't belong to any other field, assume it a part of a multi-line notes field
			notes += "\n" + current;

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
	if ( !notes.isNull() )
	{
		//kdDebug()<<QString("Found notes: %s (adding to end of instructions)").arg(m_notes)<<endl;
		recipe.instructions += "\n\nNotes: " + notes.stripWhiteSpace();
	}
}

void MXPImporter::importGeneric( QTextStream &/*stream*/ )
{
	setErrorMsg( i18n("MasterCook's Generic Export format is currently not supported.  Please write to mizunoami44@users.sourceforge.net to request support for this format.") );
//not even sure it this is worth writing... its rather obsolete
}

void MXPImporter::importMac( QTextStream &/*stream*/ )
{
	setErrorMsg( i18n("MasterCook Mac's Export format is currently not supported.  Please write to mizunoami44@users.sourceforge.net to request support for this format.") );
//not even sure it this is worth writing... its rather obsolete
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
