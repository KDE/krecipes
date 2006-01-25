/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "mmfimporter.h"

#include <kapplication.h>
#include <klocale.h>
#include <kdebug.h>

#include <qfile.h>
#include <qregexp.h>
#include <qtextstream.h>
#include <qstringlist.h>

#include "datablocks/mixednumber.h"
#include "datablocks/recipe.h"
#include "mmdata.h"

//TODO: pre-parse file and try to correct alignment errors in ingredients?

MMFImporter::MMFImporter() : BaseImporter()
{}

MMFImporter::~MMFImporter()
{}

void MMFImporter::parseFile( const QString &file )
{
	resetVars();

	QFile input( file );

	if ( input.open( IO_ReadOnly ) ) {
		QTextStream stream( &input );
		stream.skipWhiteSpace();

		QString line;
		while ( !stream.atEnd() ) {
			line = stream.readLine();

			if ( line.startsWith( "MMMMM" ) ) {
				version = VersionMMMMM;
				importMMF( stream );
			}
			else if ( line.contains( "Recipe Extracted from Meal-Master (tm) Database" ) ) {
				version = FromDatabase;
				importMMF( stream );
			}
			else if ( line.startsWith( "-----" ) ) {
				version = VersionNormal;
				importMMF( stream );
			}
			else if ( line.startsWith( "MM" ) ) {
				version = VersionBB;
				( void ) stream.readLine();
				importMMF( stream );
			}

			stream.skipWhiteSpace();
		}

		if ( fileRecipeCount() == 0 )
			addWarningMsg( i18n( "No recipes found in this file." ) );
	}
	else
		setErrorMsg( i18n( "Unable to open file." ) );
}

void MMFImporter::importMMF( QTextStream &stream )
{
	kapp->processEvents(); //don't want the user to think its frozen... especially for files with thousands of recipes

	QString current;

	//===============FIXED FORMAT================//
	//line 1: title
	//line 2: categories (comma or space separated)
	//line 3: yield (number followed by label)

	//title
	stream.skipWhiteSpace();
	current = stream.readLine();
	m_title = current.mid( current.find( ":" ) + 1, current.length() ).stripWhiteSpace();
	kdDebug() << "Found title: " << m_title << endl;

	//categories
	stream.skipWhiteSpace();
	current = stream.readLine().stripWhiteSpace();
	const char separator = ( version == FromDatabase ) ? ' ' : ',';
	QStringList categories = QStringList::split( separator, current.mid( current.find( ":" ) + 1, current.length() ) );
	for ( QStringList::const_iterator it = categories.begin(); it != categories.end(); ++it ) {
		Element new_cat;
		new_cat.name = QString( *it ).stripWhiteSpace();
		kdDebug() << "Found category: " << new_cat.name << endl;
		m_categories.append( new_cat );
	}

	//servings
	stream.skipWhiteSpace();
	current = stream.readLine().stripWhiteSpace();
	if ( current.startsWith( "Yield:" ) ) {
		//get the number between the ":" and the next space after it
		m_servings = current.mid( current.find( ":" ) + 1,
		                          current.find( " ", current.find( ":" ) + 2 ) - current.find( ":" ) ).toInt();
		kdDebug() << "Found yield: " << m_servings << endl;
	}
	else if ( current.startsWith( "Servings:" ) )  //from database version
	{
		m_servings = current.mid( current.find( ":" ) + 1, current.length() ).toInt();
		kdDebug() << "Found servings: " << m_servings << endl;
	}

	//=======================VARIABLE FORMAT===================//
	//read lines until ending is found
	//each line is either an ingredient, ingredient header, or instruction
	bool instruction_found = false;

	( void ) stream.readLine();
	current = stream.readLine();
	while ( current.stripWhiteSpace() != "MMMMM" &&
	        current.stripWhiteSpace() != "-----" &&
	        current.stripWhiteSpace() != "-----------------------------------------------------------------------------" &&
	        !stream.atEnd() ) {
		bool col_one_used = loadIngredientLine( current.left( 41 ), m_left_col_ing );
		if ( col_one_used )  //only check for second column if there is an ingredient in the first column
			loadIngredientLine( current.mid( 41, current.length() ), m_right_col_ing );

		if ( instruction_found && col_one_used ) {
			addWarningMsg( QString( i18n( "While loading recipe <b>%1</b> "
			                              "an ingredient line was found after the directions. "
			                              "While this is valid, it most commonly indicates an incorrectly "
			                              "formatted recipe." ) ).arg( m_title ) );
		}

		if ( !col_one_used &&
		        !loadIngredientHeader( current.stripWhiteSpace() ) ) {
			if ( !current.stripWhiteSpace().isEmpty() )
				instruction_found = true;
			m_instructions += current.stripWhiteSpace() + "\n";
			//kdDebug()<<"Found instruction line: "<<current.stripWhiteSpace()<<endl;
		}

		current = stream.readLine();
	}
	m_instructions = m_instructions.stripWhiteSpace();
	//kdDebug()<<"Found instructions: "<<m_instructions<<endl;

	putDataInRecipe();
}

bool MMFImporter::loadIngredientLine( const QString &string, IngredientList &list )
{
	//just ignore an empty line
	if ( string.stripWhiteSpace().isEmpty() )
		return false;

	Ingredient new_ingredient;
	new_ingredient.amount = 0; //amount not required, so give default of 0

	if ( string.at( 11 ) == "-" && string.mid( 0, 11 ).stripWhiteSpace().isEmpty() )  //continuation of previous ingredient
	{
		//kdDebug()<<"Appending to last ingredient in column: "<<string.stripWhiteSpace().mid(1,string.length())<<endl;
		if ( !list.isEmpty() )  //so it doesn't crash when the first ingredient appears to be a continuation of another
			( *list.at( list.count() - 1 ) ).name += " " + string.stripWhiteSpace().mid( 1, string.length() );

		return true;
	}

	//amount
	if ( !string.mid( 0, 7 ).stripWhiteSpace().isEmpty() ) {
		bool ok;
		MixedNumber amount = MixedNumber::fromString( string.mid( 0, 7 ).stripWhiteSpace(), &ok, false );
		if ( !ok )
			return false;
		else
			new_ingredient.amount = amount.toDouble();
	}

	//amount/unit separator
	if ( string[ 7 ] != ' ' )
		return false;

	//unit
	if ( !string.mid( 8, 2 ).stripWhiteSpace().isEmpty() ) {
		bool is_unit = false;
		QString unit( string.mid( 8, 2 ).stripWhiteSpace() );
		for ( int i = 0; unit_info[ i ].short_form; i++ ) {
			if ( unit_info[ i ].short_form == unit ) {
				is_unit = true;
				if ( new_ingredient.amount <= 1 )
					unit = unit_info[ i ].expanded_form;
				else
					unit = unit_info[ i ].plural_expanded_form;

				break;
			}
		}
		if ( !is_unit ) { /*This gives too many false warnings...
						addWarningMsg( QString(i18n("Unit \"%1\" not recognized. "
						  "Used in the context of \"%2\".  If this shouldn't be an ingredient line (i.e. is part of the instructions), "
						  "then you can safely ignore this warning, and the recipe will be correctly imported.")).arg(unit).arg(string.stripWhiteSpace()) );*/ 
			return false;
		}

		if ( int(new_ingredient.amount) > 1 )
			new_ingredient.units.plural = unit;
		else
			new_ingredient.units.name = unit;
	}

	//unit/name separator
	if ( string[ 10 ] != ' ' || string[ 11 ] == ' ' )
		return false;

	//name and preparation method
	new_ingredient.name = string.mid( 11, 41 ).stripWhiteSpace();

	//put in the header... it there is no header, current_header will be QString::null
	new_ingredient.group = current_header;

	//if we made it this far it is an ingredient line
	list.append( new_ingredient );

	return true;
}

bool MMFImporter::loadIngredientHeader( const QString &string )
{
	if ( ( string.startsWith( "-----" ) || string.startsWith( "MMMMM" ) ) &&
	        string.length() >= 40 &&
	        ( ( string.at( string.length() / 2 ) != "-" ) ||
	          ( string.at( string.length() / 2 + 1 ) != "-" ) ||
	          ( string.at( string.length() / 2 - 1 ) != "-" ) ) ) {
		QString header( string.stripWhiteSpace() );

		//get only the header name
		header.remove( QRegExp( "^MMMMM" ) );
		header.remove( QRegExp( "^-*" ) ).remove( QRegExp( "-*$" ) );

		kdDebug() << "found ingredient header: " << header << endl;

		//merge all columns before appending to full ingredient list to maintain the ingredient order
		for ( IngredientList::iterator ing_it = m_left_col_ing.begin(); ing_it != m_left_col_ing.end(); ++ing_it ) {
			m_all_ing.append( *ing_it );
		}
		m_left_col_ing.empty();

		for ( IngredientList::iterator ing_it = m_right_col_ing.begin(); ing_it != m_right_col_ing.end(); ++ing_it ) {
			m_all_ing.append( *ing_it );
		}
		m_right_col_ing.empty();

		current_header = header;
		return true;
	}
	else
		return false;
}

void MMFImporter::putDataInRecipe()
{
	for ( IngredientList::const_iterator ing_it = m_left_col_ing.begin(); ing_it != m_left_col_ing.end(); ++ing_it )
		m_all_ing.append( *ing_it );
	for ( IngredientList::const_iterator ing_it = m_right_col_ing.begin(); ing_it != m_right_col_ing.end(); ++ing_it )
		m_all_ing.append( *ing_it );

	for ( IngredientList::iterator ing_it = m_all_ing.begin(); ing_it != m_all_ing.end(); ++ing_it ) {
		QString name_and_prep = ( *ing_it ).name;
		int separator_index = name_and_prep.find( QRegExp( "(;|,)" ) );
		if ( separator_index != -1 ) {
			( *ing_it ).name = name_and_prep.mid( 0, separator_index ).stripWhiteSpace();
			( *ing_it ).prepMethodList = ElementList::split(",",name_and_prep.mid( separator_index + 1, name_and_prep.length() ).stripWhiteSpace() );
		}
	}

	//create the recipe
	Recipe new_recipe;
	new_recipe.yield.amount = m_servings;
	new_recipe.yield.type = i18n("servings");
	new_recipe.title = m_title;
	new_recipe.instructions = m_instructions;
	new_recipe.ingList = m_all_ing;
	new_recipe.categoryList = m_categories;
	new_recipe.authorList = m_authors;
	new_recipe.recipeID = -1;

	//put it in the recipe list
	add
		( new_recipe );

	//reset for the next recipe to use these variables
	resetVars();
}

void MMFImporter::resetVars()
{
	m_left_col_ing.empty();
	m_right_col_ing.empty();
	m_all_ing.empty();
	m_authors.clear();
	m_categories.clear();

	m_servings = 0;

	m_title = QString::null;
	m_instructions = QString::null;

	current_header = QString::null;
}

