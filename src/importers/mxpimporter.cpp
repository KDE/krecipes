/***************************************************************************
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "mxpimporter.h"

#include <QFile>
#include <QStringList>
#include <QTextStream>

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
		QTextStream stream( &input );
		stream.skipWhiteSpace();

		QString line;
		while ( !stream.atEnd() ) {
			line = stream.readLine().trimmed();

			if ( line == "{ Exported from MasterCook Mac }" ) {
				kDebug() << "detected Mac file";
				importMac( stream );
			}
			else if ( line == "@@@@@" ) {
				kDebug() << "detected generic file";
				importGeneric( stream );
			}
			else if ( line.simplified().contains( "Exported from MasterCook" ) ) {
				kDebug() << "detected MasterCook normal file";
				importMXP( stream );
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

void MXPImporter::importMXP( QTextStream &stream )
{
	Recipe recipe;

	kapp->processEvents(); //don't want the user to think its frozen... especially for files with thousands of recipes

	//kDebug()<<"Found recipe MXP format: * Exported from MasterCook *";
	QString current;

	// title
	stream.skipWhiteSpace();
	recipe.title = stream.readLine().trimmed();
	//kDebug()<<"Found title: "<<m_title;

	//author
	stream.skipWhiteSpace();
	current = stream.readLine().trimmed();
	if ( current.mid( 0, current.indexOf( ":" ) ).simplified().toLower() == "recipe by" ) {
		Element new_author( current.mid( current.indexOf( ":" ) + 1, current.length() ).trimmed() );
		recipe.authorList.append( new_author );
		//kDebug()<<"Found author: "<<new_author.name;
	}
	else {
		addWarningMsg( i18n( "While loading recipe \"%1\" "
		                              "the field \"Recipe By:\" is either missing or could not be detected.", recipe.title ) );
	}

	//servings
	stream.skipWhiteSpace();
	current = stream.readLine().trimmed();
	if ( current.mid( 0, current.indexOf( ":" ) ).simplified().toLower() == "serving size" ) {
		//allows serving size to be loaded even if preparation time is missing
		int end_index;
		if ( current.contains( "preparation time", Qt::CaseInsensitive ) )
			end_index = current.indexOf( "preparation time", 0, Qt::CaseInsensitive ) - 15;
		else
			end_index = current.length();

		recipe.yield.setAmount(current.mid( current.indexOf( ":" ) + 1, end_index ).trimmed().toInt());
		recipe.yield.setType(i18n("servings"));
		//kDebug()<<"Found serving size: "<<recipe.yield.amount;
	}
	else {
		addWarningMsg( i18n( "While loading recipe \"%1\" "
		                              "the field \"Serving Size:\" is either missing or could not be detected." , recipe.title ) );
	}

	if ( current.contains( "preparation time", Qt::CaseInsensitive ) ) {
		QString prep_time = current.mid( current.indexOf( ":", current.indexOf( "preparation time", 0, Qt::CaseInsensitive ) ) + 1,
		                                 current.length() ).trimmed();
		recipe.prepTime = QTime( prep_time.section( ':', 0, 0 ).toInt(), prep_time.section( ':', 1, 1 ).toInt() );
		kDebug() << "Found preparation time: " << prep_time ;
	}
	else {
		addWarningMsg( i18n( "While loading recipe \"%1\" "
			"the field \"Preparation Time:\" is either missing or could not be detected." , recipe.title ) );
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

void MXPImporter::loadCategories( QTextStream &stream, Recipe &recipe )
{
	//====================categories====================//
	stream.skipWhiteSpace();
	QString current = stream.readLine().trimmed();
	if ( current.mid( 0, current.indexOf( ":" ) ).simplified().toLower() == "categories" ) {
		QString tmp_str = current.mid( current.indexOf( ":" ) + 1, current.length() ).trimmed();

		while ( current.trimmed() != "Amount  Measure       Ingredient -- Preparation Method" && !stream.atEnd() ) {
			if ( !tmp_str.isEmpty() ) {
				const QStringList categories = tmp_str.split( QRegExp("\t|  "), QString::SkipEmptyParts );
				for ( QStringList::const_iterator it = categories.constBegin(); it != categories.constEnd(); ++it ) {
					Element new_cat( ( *it ).trimmed() );
					recipe.categoryList.append( new_cat );

					kDebug()<<"Found category: "<<new_cat.name;
				}
			}

			current = stream.readLine();
			tmp_str = current;
		}
		//else
		//	kDebug()<<"No categories found.";
	}
	else {
		addWarningMsg(i18n( "While loading recipe \"%1\" "
			"the field \"Categories:\" is either missing or could not be detected.", recipe.title ) );

		//the ingredient loaded will expect the last thing to have been read to be this header line
		while ( current.trimmed() != "Amount  Measure       Ingredient -- Preparation Method" && !stream.atEnd() )
			current = stream.readLine();
	}
}

void MXPImporter::loadIngredients( QTextStream &stream, Recipe &recipe )
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
					addWarningMsg( i18n( "While loading recipe \"%1\" Invalid amount \"%2\" in the line \"%3\"" , recipe.title, amount_str , current.trimmed() ) );
					current = stream.readLine();
					continue;
				}
				new_ingredient.amount = amount.toDouble();
			}

			//units
			QString units( current.mid( 9, 13 ) );
			new_ingredient.units = Unit( units.simplified(), new_ingredient.amount );

			//name
			int dash_index = current.indexOf( "--" );

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
			//  <<", prep_method:"<<prep_method;

			current = stream.readLine();
		}
	}
	//else
	//	kDebug()<<"No ingredients found.";
}

void MXPImporter::loadInstructions( QTextStream &stream, Recipe &recipe )
{
	//==========================instructions ( along with other optional fields... mxp format doesn't define end of ingredients and start of other fields )==============//
	stream.skipWhiteSpace();
	QString current = stream.readLine().trimmed();
	while ( !current.contains( "- - - -" ) && !stream.atEnd() ) {
		if ( current.trimmed() == "Source:" ) {
			Element new_author( getNextQuotedString( stream ) );
			recipe.authorList.append( new_author );
			//kDebug()<<"Found source: "<<new_author.name<<" (adding as author)";
		}
		else if ( current.trimmed() == "Description:" ) {
			QString description = getNextQuotedString( stream );
			//kDebug()<<"Found description: "<<m_description<<" (adding to end of instructions)";
			recipe.instructions += "\n\nDescription: " + description;
		}
		else if ( current.trimmed() == "S(Internet Address):" ) {
			QString internet = getNextQuotedString( stream );
			//kDebug()<<"Found internet address: "<<m_internet<<" (adding to end of instructions)";
			recipe.instructions += "\n\nInternet address: " + internet;
		}
		else if ( current.trimmed() == "Yield:" ) {
			recipe.yield.setAmount(getNextQuotedString( stream ).trimmed().toInt());
			recipe.yield.setType(i18n("servings"));
			//kDebug()<<"Found yield: "<<recipe.yield.amount<<" (adding as servings)";
		}
		else if ( current.trimmed() == "T(Cook Time):" ) {
			( void ) getNextQuotedString( stream ); //this would be prep time, but we don't use prep time at the moment
			//kDebug()<<"Found cook time: "<<m_prep_time<<" (adding as prep time)";
		}
		else if ( current.trimmed() == "Cuisine:" ) {
			Element new_cat( getNextQuotedString( stream ) );
			recipe.categoryList.append( new_cat );
			//kDebug()<<"Found cuisine (adding as category): "<<new_cat.name;
		}
		else
			recipe.instructions += current + '\n';

		current = stream.readLine().trimmed();
	}
	recipe.instructions = recipe.instructions.trimmed();
	//kDebug()<<"Found instructions: "<<m_instructions;
}

void MXPImporter::loadOptionalFields( QTextStream &stream, Recipe &recipe )
{
	//=================after here, fields are optional=========================//
	stream.skipWhiteSpace();
	QString current = stream.readLine().trimmed();

	QString notes;

	//Note: we simplified() because some versions of MasterCook have "Exported from MasterCook" and others have "Exported  from MasterCook".
	//      This also could work around a typo or such.
	while ( !current.simplified().contains( "Exported from MasterCook" ) && !stream.atEnd() ) {
		//suggested wine
		if ( current.mid( 0, current.indexOf( ":" ) ).simplified().toLower() == "suggested wine" ) {
			QString wine = current.mid( current.indexOf( ":" ) + 1, current.length() ).trimmed();
			//kDebug()<<"Found suggested wine: "<<m_wine<<" (adding to end of instructions)";

			recipe.instructions += "\n\nSuggested wine: " + wine;
		}
		//Nutr. Assoc.
		if ( current.mid( 0, current.indexOf( ":" ) ).simplified().toLower() == "nutr. assoc." ) {
			QString nutr_assoc = current.mid( current.indexOf( ":" ) + 1, current.length() ).trimmed();
			//kDebug()<<"Found nutrient association: "<<nutr_assoc<<" (adding to end of instructions)";

			recipe.instructions += "\n\nNutrient Association: " + nutr_assoc;
		}
		else if ( current.mid( 0, current.indexOf( ":" ) ).simplified().toLower() == "per serving (excluding unknown items)" ) { //per serving... maybe we can do something with this info later
			QString per_serving_info = current.mid( current.indexOf( ":" ) + 1, current.length() ).trimmed();
			//kDebug()<<"Found per serving (excluding unknown items): "<<per_serving_info<<" (adding to end of instructions)";

			recipe.instructions += "\n\nPer Serving (excluding unknown items): " + per_serving_info;
		}
		else if ( current.mid( 0, current.indexOf( ":" ) ).simplified().toLower() == "per serving" ) { //per serving... maybe we can do something with this info later
			QString per_serving_info = current.mid( current.indexOf( ":" ) + 1, current.length() ).trimmed();
			//kDebug()<<"Found per serving: "<<per_serving_info<<" (adding to end of instructions)";

			recipe.instructions += "\n\nPer Serving: " + per_serving_info;
		}
		else if ( current.mid( 0, current.indexOf( ":" ) ).simplified().toLower() == "food exchanges" ) { //food exchanges... maybe we can do something with this info later
			QString food_exchange_info = current.mid( current.indexOf( ":" ) + 1, current.length() ).trimmed();
			//kDebug()<<"Found food exchanges: "<<food_exchange_info<<" (adding to end of instructions)";

			recipe.instructions += "\n\nFood Exchanges: " + food_exchange_info;
		}
		else if ( current.mid( 0, current.indexOf( ":" ) ).simplified().toLower() == "serving ideas" ) { //serving ideas
			QString serving_ideas = current.mid( current.indexOf( ":" ) + 1, current.length() ).trimmed();
			//kDebug()<<"Found serving ideas: "<<m_serving_ideas<<" (adding to end of instructions)";

			recipe.instructions += "\n\nServing ideas: " + serving_ideas;
		}
		else if ( current.mid( 0, current.indexOf( ":" ) ).simplified().toLower() == "notes" )  //notes
			notes = current.mid( current.indexOf( ":" ) + 1, current.length() ).trimmed();
		else if ( !current.isEmpty() && current != "_____" )  //if it doesn't belong to any other field, assume it a part of a multi-line notes field
			notes += '\n' + current;

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
	if ( !notes.isEmpty() ) {
		//kDebug()<<QString("Found notes: %s (adding to end of instructions)").arg(m_notes);
		recipe.instructions += "\n\nNotes: " + notes.trimmed();
	}
}

void MXPImporter::importGeneric( QTextStream & /*stream*/ )
{
	setErrorMsg( i18n( "MasterCook's Generic Export format is currently not supported.  Please write to jkivlighn@gmail.com to request support for this format." ) );
	//not even sure it this is worth writing... its rather obsolete
}

void MXPImporter::importMac( QTextStream & /*stream*/ )
{
	setErrorMsg( i18n( "MasterCook Mac's Export format is currently not supported.  Please write to jkivlighn@gmail.com to request support for this format." ) );
	//not even sure it this is worth writing... its rather obsolete
}

QString MXPImporter::getNextQuotedString( QTextStream &stream )
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
		return_str += '\n' + current;
	}

	//take off quote at end
	return_str = return_str.mid( 0, return_str.length() - 1 );

	return return_str.trimmed();
}
