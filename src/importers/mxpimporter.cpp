/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "mxpimporter.h"

#include <qfile.h>
#include <q3textstream.h>
#include <qstringlist.h>
#include <qdatetime.h>

#include <kapplication.h>
#include <klocale.h>
#include <kdebug.h>

#include "datablocks/mixednumber.h"
#include "datablocks/recipe.h"

MXPImporter::MXPImporter() : BaseImporter()
{}

void MXPImporter::parseFile( const QString &file )
{
	QFile input( file );

	if ( input.open( QIODevice::ReadOnly ) ) {
		Q3TextStream stream( &input );
		stream.skipWhiteSpace();

		QString line;
		while ( !stream.atEnd() ) {
			line = stream.readLine().trimmed();

			if ( line.simplified().contains( "Exported from MasterCook" ) ) {
				importMXP( stream );
			}
			else if ( line == "{ Exported from MasterCook Mac }" ) {
				importMac( stream );
			}
			else if ( line == "@@@@@" ) {
				importGeneric( stream );
			}

			stream.skipWhiteSpace();
		}

		if ( fileRecipeCount() == 0 )
			addWarningMsg( i18n( "No recipes found in this file." ) );
	}
	else
		setErrorMsg( i18n( "Unable to open file." ) );
}

MXPImporter::~MXPImporter()
{}

void MXPImporter::importMXP( Q3TextStream &stream )
{
	Recipe recipe;

	kapp->processEvents(); //don't want the user to think its frozen... especially for files with thousands of recipes

	//kDebug()<<"Found recipe MXP format: * Exported from MasterCook *"<<endl;
	QString current;

	// title
	stream.skipWhiteSpace();
	recipe.title = stream.readLine().trimmed();
	//kDebug()<<"Found title: "<<m_title<<endl;

	//author
	stream.skipWhiteSpace();
	current = stream.readLine().trimmed();
	if ( current.mid( 0, current.find( ":" ) ).simplified().toLower() == "recipe by" ) {
		Element new_author( current.mid( current.find( ":" ) + 1, current.length() ).trimmed() );
		recipe.authorList.append( new_author );
		//kDebug()<<"Found author: "<<new_author.name<<endl;
	}
	else {
		addWarningMsg( QString( i18n( "While loading recipe \"%1\" "
		                              "the field \"Recipe By:\" is either missing or could not be detected." ) ).arg( recipe.title ) );
	}

	//servings
	stream.skipWhiteSpace();
	current = stream.readLine().trimmed();
	if ( current.mid( 0, current.find( ":" ) ).simplified().toLower() == "serving size" ) {
		//allows serving size to be loaded even if preparation time is missing
		int end_index;
		if ( current.contains( "preparation time", FALSE ) )
			end_index = current.find( "preparation time", 0, FALSE ) - 15;
		else
			end_index = current.length();

		recipe.yield.amount = current.mid( current.find( ":" ) + 1, end_index ).trimmed().toInt();
		recipe.yield.type = i18n("servings");
		//kDebug()<<"Found serving size: "<<recipe.yield.amount<<endl;
	}
	else {
		addWarningMsg( QString( i18n( "While loading recipe \"%1\" "
		                              "the field \"Serving Size:\" is either missing or could not be detected." ) ).arg( recipe.title ) );
	}

	if ( current.contains( "preparation time", FALSE ) ) {
		QString prep_time = current.mid( current.find( ":", current.find( "preparation time", 0, FALSE ) ) + 1,
		                                 current.length() ).trimmed();
		recipe.prepTime = QTime( prep_time.section( ':', 0, 0 ).toInt(), prep_time.section( ':', 1, 1 ).toInt() );
		kDebug() << "Found preparation time: " << prep_time << endl;
	}
	else {
		addWarningMsg( QString( i18n( "While loading recipe \"%1\" "
		                              "the field \"Preparation Time:\" is either missing or could not be detected." ) ).arg( recipe.title ) );
	}

	loadCategories( stream, recipe );
	loadIngredients( stream, recipe );
	loadInstructions( stream, recipe );
	loadOptionalFields( stream, recipe );

	add
		( recipe );

	if ( !stream.atEnd() ) {
		importMXP( stream );
		return ;
	}
}

void MXPImporter::loadCategories( Q3TextStream &stream, Recipe &recipe )
{
	//====================categories====================//
	stream.skipWhiteSpace();
	QString current = stream.readLine().trimmed();
	if ( current.mid( 0, current.find( ":" ) ).simplified().toLower() == "categories" ) {
		QString tmp_str = current.mid( current.find( ":" ) + 1, current.length() ).trimmed();

		while ( current.trimmed() != "Amount  Measure       Ingredient -- Preparation Method" && !stream.atEnd() ) {
			if ( !tmp_str.isEmpty() ) {
				QStringList categories = QStringList::split( "  ", tmp_str );
				for ( QStringList::const_iterator it = categories.begin(); it != categories.end(); ++it ) {
					Element new_cat( ( *it ).trimmed() );
					recipe.categoryList.append( new_cat );

					//kDebug()<<"Found category: "<<new_cat.name<<endl;
				}
			}

			current = stream.readLine();
			tmp_str = current;
		}
		//else
		//	kDebug()<<"No categories found."<<endl;
	}
	else {
		addWarningMsg( QString( i18n( "While loading recipe \"%1\" "
		                              "the field \"Categories:\" is either missing or could not be detected." ) ).arg( recipe.title ) );

		//the ingredient loaded will expect the last thing to have been read to be this header line
		while ( current.trimmed() != "Amount  Measure       Ingredient -- Preparation Method" && !stream.atEnd() )
			current = stream.readLine();
	}
}

void MXPImporter::loadIngredients( Q3TextStream &stream, Recipe &recipe )
{
	//============ingredients=================//
	stream.skipWhiteSpace();
	( void ) stream.readLine();
	QString current = stream.readLine();
	if ( !current.contains( "NONE" ) && !current.isEmpty() ) {
		while ( !current.isEmpty() && !stream.atEnd() ) {
			Ingredient new_ingredient;

			//amount
			QString amount_str = current.mid( 0, 9 ).simplified();
			if ( !amount_str.isEmpty() )  // case of amount_str.isEmpty() correctly handled by class default
			{
				bool ok;
				MixedNumber amount( MixedNumber::fromString( amount_str, &ok ) );
				if ( !ok )
				{
					addWarningMsg( QString( i18n( "While loading recipe \"%1\" Invalid amount \"%2\" in the line \"%3\"" ) ).arg( recipe.title ).arg( amount_str ).arg( current.trimmed() ) );
					current = stream.readLine();
					continue;
				}
				new_ingredient.amount = amount.toDouble();
			}

			//units
			QString units( current.mid( 9, 13 ) );
			new_ingredient.units = Unit( units.simplified(), new_ingredient.amount );

			//name
			int dash_index = current.find( "--" );

			int length;
			if ( dash_index == -1 || dash_index == 24 )  //ignore a dash in the first position (index 24)
				length = current.length();
			else
				length = dash_index - 22;

			QString ingredient_name( current.mid( 22, length ) );
			new_ingredient.name = ingredient_name.trimmed();

			//prep method
			if ( dash_index != -1 && dash_index != 24 )  //ignore a dash in the first position (index 24)
				new_ingredient.prepMethodList.append( Element(current.mid( dash_index + 2, current.length() ).trimmed()) );

			recipe.ingList.append( new_ingredient );
			//kDebug()<<"Found ingredient: amount="<<new_ingredient.amount
			//  <<", unit:"<<new_ingredient.units
			//  <<", name:"<<new_ingredient.name
			//  <<", prep_method:"<<prep_method<<endl;

			current = stream.readLine();
		}
	}
	//else
	//	kDebug()<<"No ingredients found."<<endl;
}

void MXPImporter::loadInstructions( Q3TextStream &stream, Recipe &recipe )
{
	//==========================instructions ( along with other optional fields... mxp format doesn't define end of ingredients and start of other fields )==============//
	stream.skipWhiteSpace();
	QString current = stream.readLine().trimmed();
	while ( !current.contains( "- - - -" ) && !stream.atEnd() ) {
		if ( current.trimmed() == "Source:" ) {
			Element new_author( getNextQuotedString( stream ) );
			recipe.authorList.append( new_author );
			//kDebug()<<"Found source: "<<new_author.name<<" (adding as author)"<<endl;
		}
		else if ( current.trimmed() == "Description:" ) {
			QString description = getNextQuotedString( stream );
			//kDebug()<<"Found description: "<<m_description<<" (adding to end of instructions)"<<endl;
			recipe.instructions += "\n\nDescription: " + description;
		}
		else if ( current.trimmed() == "S(Internet Address):" ) {
			QString internet = getNextQuotedString( stream );
			//kDebug()<<"Found internet address: "<<m_internet<<" (adding to end of instructions)"<<endl;
			recipe.instructions += "\n\nInternet address: " + internet;
		}
		else if ( current.trimmed() == "Yield:" ) {
			recipe.yield.amount = getNextQuotedString( stream ).trimmed().toInt();
			recipe.yield.type = i18n("servings");
			//kDebug()<<"Found yield: "<<recipe.yield.amount<<" (adding as servings)"<<endl;
		}
		else if ( current.trimmed() == "T(Cook Time):" ) {
			( void ) getNextQuotedString( stream ); //this would be prep time, but we don't use prep time at the moment
			//kDebug()<<"Found cook time: "<<m_prep_time<<" (adding as prep time)"<<endl;
		}
		else if ( current.trimmed() == "Cuisine:" ) {
			Element new_cat( getNextQuotedString( stream ) );
			recipe.categoryList.append( new_cat );
			//kDebug()<<"Found cuisine (adding as category): "<<new_cat.name<<endl;
		}
		else
			recipe.instructions += current + "\n";

		current = stream.readLine().trimmed();
	}
	recipe.instructions = recipe.instructions.trimmed();
	//kDebug()<<"Found instructions: "<<m_instructions<<endl;
}

void MXPImporter::loadOptionalFields( Q3TextStream &stream, Recipe &recipe )
{
	//=================after here, fields are optional=========================//
	stream.skipWhiteSpace();
	QString current = stream.readLine().trimmed();

	QString notes;

	//Note: we simplified() because some versions of MasterCook have "Exported from MasterCook" and others have "Exported  from MasterCook".
	//      This also could work around a typo or such.
	while ( !current.simplified().contains( "Exported from MasterCook" ) && !stream.atEnd() ) {
		//suggested wine
		if ( current.mid( 0, current.find( ":" ) ).simplified().toLower() == "suggested wine" ) {
			QString wine = current.mid( current.find( ":" ) + 1, current.length() ).trimmed();
			//kDebug()<<"Found suggested wine: "<<m_wine<<" (adding to end of instructions)"<<endl;

			recipe.instructions += "\n\nSuggested wine: " + wine;
		}
		//Nutr. Assoc.
		if ( current.mid( 0, current.find( ":" ) ).simplified().toLower() == "nutr. assoc." ) {
			QString nutr_assoc = current.mid( current.find( ":" ) + 1, current.length() ).trimmed();
			//kDebug()<<"Found nutrient association: "<<nutr_assoc<<" (adding to end of instructions)"<<endl;

			recipe.instructions += "\n\nNutrient Association: " + nutr_assoc;
		}
		else if ( current.mid( 0, current.find( ":" ) ).simplified().toLower() == "per serving (excluding unknown items)" ) { //per serving... maybe we can do something with this info later
			QString per_serving_info = current.mid( current.find( ":" ) + 1, current.length() ).trimmed();
			//kDebug()<<"Found per serving (excluding unknown items): "<<per_serving_info<<" (adding to end of instructions)"<<endl;

			recipe.instructions += "\n\nPer Serving (excluding unknown items): " + per_serving_info;
		}
		else if ( current.mid( 0, current.find( ":" ) ).simplified().toLower() == "per serving" ) { //per serving... maybe we can do something with this info later
			QString per_serving_info = current.mid( current.find( ":" ) + 1, current.length() ).trimmed();
			//kDebug()<<"Found per serving: "<<per_serving_info<<" (adding to end of instructions)"<<endl;

			recipe.instructions += "\n\nPer Serving: " + per_serving_info;
		}
		else if ( current.mid( 0, current.find( ":" ) ).simplified().toLower() == "food exchanges" ) { //food exchanges... maybe we can do something with this info later
			QString food_exchange_info = current.mid( current.find( ":" ) + 1, current.length() ).trimmed();
			//kDebug()<<"Found food exchanges: "<<food_exchange_info<<" (adding to end of instructions)"<<endl;

			recipe.instructions += "\n\nFood Exchanges: " + food_exchange_info;
		}
		else if ( current.mid( 0, current.find( ":" ) ).simplified().toLower() == "serving ideas" ) { //serving ideas
			QString serving_ideas = current.mid( current.find( ":" ) + 1, current.length() ).trimmed();
			//kDebug()<<"Found serving ideas: "<<m_serving_ideas<<" (adding to end of instructions)"<<endl;

			recipe.instructions += "\n\nServing ideas: " + serving_ideas;
		}
		else if ( current.mid( 0, current.find( ":" ) ).simplified().toLower() == "notes" )  //notes
			notes = current.mid( current.find( ":" ) + 1, current.length() ).trimmed();
		else if ( !current.isEmpty() && current != "_____" )  //if it doesn't belong to any other field, assume it a part of a multi-line notes field
			notes += "\n" + current;

		current = stream.readLine().trimmed();
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
	if ( !notes.isNull() ) {
		//kDebug()<<QString("Found notes: %s (adding to end of instructions)").arg(m_notes)<<endl;
		recipe.instructions += "\n\nNotes: " + notes.trimmed();
	}
}

void MXPImporter::importGeneric( Q3TextStream & /*stream*/ )
{
	setErrorMsg( i18n( "MasterCook's Generic Export format is currently not supported.  Please write to jkivlighn@gmail.com to request support for this format." ) );
	//not even sure it this is worth writing... its rather obsolete
}

void MXPImporter::importMac( Q3TextStream & /*stream*/ )
{
	setErrorMsg( i18n( "MasterCook Mac's Export format is currently not supported.  Please write to jkivlighn@gmail.com to request support for this format." ) );
	//not even sure it this is worth writing... its rather obsolete
}

QString MXPImporter::getNextQuotedString( Q3TextStream &stream )
{
	stream.skipWhiteSpace();
	QString current = stream.readLine().trimmed();
	QString return_str;

	if ( current.left( 1 ) == "\"" )
		return_str = current.mid( 1, current.length() - 1 );
	else
		return current;

	while ( current.right( 1 ) != "\"" && !stream.atEnd() ) {
		current = stream.readLine().trimmed();
		return_str += "\n" + current;
	}

	//take off quote at end
	return_str = return_str.mid( 0, return_str.length() - 1 );

	return return_str.trimmed();
}
