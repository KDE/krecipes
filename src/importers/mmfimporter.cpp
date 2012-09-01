/***************************************************************************
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
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

#include <QFile>
#include <QRegExp>
#include <QStringList>
#include <QTextStream>

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

	if ( input.open( QIODevice::ReadOnly ) ) {
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
	m_title = current.mid( current.indexOf( ":" ) + 1, current.length() ).trimmed();
	kDebug() << "Found title: " << m_title ;

	//categories
	stream.skipWhiteSpace();
	current = stream.readLine().trimmed();
	const char separator = ( version == FromDatabase ) ? ' ' : ',';
	QStringList categories;
	if ( current.mid( current.indexOf( ":" ) + 1, current.length() ).isEmpty() )
		categories = QStringList();
	else
		categories = current.mid( current.indexOf( ":" ) + 1, current.length() ).split( separator, QString::SkipEmptyParts);

	for ( QStringList::const_iterator it = categories.constBegin(); it != categories.constEnd(); ++it ) {
		Element new_cat;
		new_cat.name = QString( *it ).trimmed();
		kDebug() << "Found category: " << new_cat.name ;
		m_categories.append( new_cat );
	}

	//servings
	stream.skipWhiteSpace();
	current = stream.readLine().trimmed();
	if ( current.startsWith( "Yield:" ) ) {
		//get the number between the ":" and the next space after it
		m_servings = current.mid( current.indexOf( ":" ) + 1,
		                          current.indexOf( " ", current.indexOf( ":" ) + 2 ) - current.indexOf( ":" ) ).toInt();
		kDebug() << "Found yield: " << m_servings ;
	}
	else if ( current.startsWith( "Servings:" ) )  //from database version
	{
		m_servings = current.mid( current.indexOf( ":" , 0, Qt::CaseSensitive ) + 1, current.length() ).toInt();
		kDebug() << "Found servings: " << m_servings ;
	}

	//=======================VARIABLE FORMAT===================//
	//read lines until ending is found
	//each line is either an ingredient, ingredient header, or instruction
	bool instruction_found = false;
	bool is_sub = false;

	( void ) stream.readLine();
	current = stream.readLine();
	while ( current.trimmed() != "MMMMM" &&
	        current.trimmed() != "-----" &&
	        current.trimmed() != "-----------------------------------------------------------------------------" &&
	        !stream.atEnd() ) {
		bool col_one_used = loadIngredientLine( current.left( 41 ), m_left_col_ing, is_sub );
		if ( col_one_used )  //only check for second column if there is an ingredient in the first column
			loadIngredientLine( current.mid( 41, current.length() ), m_right_col_ing, is_sub );

		if ( instruction_found && col_one_used ) {
			addWarningMsg( i18n( "While loading recipe <b>%1</b> "
			                              "an ingredient line was found after the directions. "
			                              "While this is valid, it most commonly indicates an incorrectly "
			                              "formatted recipe.", m_title ) );
		}

		if ( !col_one_used &&
		        !loadIngredientHeader( current.trimmed() ) ) {
			if ( !current.trimmed().isEmpty() )
				instruction_found = true;
			m_instructions += current.trimmed() + '\n';
			//kDebug()<<"Found instruction line: "<<current.trimmed();
		}

		current = stream.readLine();
	}
	m_instructions = m_instructions.trimmed();
	//kDebug()<<"Found instructions: "<<m_instructions;

	putDataInRecipe();
}

bool MMFImporter::loadIngredientLine( const QString &string, IngredientList &list, bool &is_sub )
{
	//just ignore an empty line
	if ( string.trimmed().isEmpty() )
		return false;

	Ingredient new_ingredient;
	new_ingredient.amount = 0; //amount not required, so give default of 0

	if ( string.at( 11 ) == '-' && string.mid( 0, 11 ).trimmed().isEmpty() && !list.isEmpty() )  //continuation of previous ingredient
	{
		//kDebug()<<"Appending to last ingredient in column: "<<string.trimmed().mid(1,string.length());
		list.last().name += ' ' + string.trimmed().mid( 1, string.length() );
		QString name = list.last().name;

		if ( name.endsWith(", or") ) {
			list.last().name = name.left(name.length()-4);
			is_sub = true;
		}
		else
			is_sub = false;

		return true;
	}

	//amount
	if ( !string.mid( 0, 7 ).trimmed().isEmpty() ) {
		MixedNumber amount;
		QValidator::State state;
		state = MixedNumber::fromString( string.mid( 0, 7 ).trimmed(), amount, false );
		if ( state != QValidator::Acceptable )
			return false;
		else
			new_ingredient.amount = amount.toDouble();
	}

	//amount/unit separator
	if ( string[ 7 ] != ' ' )
		return false;

	//unit
	if ( !string.mid( 8, 2 ).trimmed().isEmpty() ) {
		bool is_unit = false;
		QString unit( string.mid( 8, 2 ).trimmed() );
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
						  "then you can safely ignore this warning, and the recipe will be correctly imported.")).arg(unit).arg(string.trimmed()) );*/
			return false;
		}

		if ( int(new_ingredient.amount) > 1 )
			new_ingredient.units.setPlural(unit);
		else
			new_ingredient.units.setName(unit);
	}

	//unit/name separator
	if ( string[ 10 ] != ' ' || string[ 11 ] == ' ' )
		return false;

	//name and preparation method
	new_ingredient.name = string.mid( 11, 41 ).trimmed();

	//put in the header... it there is no header, current_header will be QString()
	new_ingredient.group = current_header;

	bool last_is_sub = is_sub;
	if ( new_ingredient.name.endsWith(", or") ) {
		new_ingredient.name = new_ingredient.name.left(new_ingredient.name.length()-4);
		is_sub = true;
	}
	else
		is_sub = false;

	if ( last_is_sub )
		list.last().substitutes.append(new_ingredient);
	else
		list.append( new_ingredient );

	//if we made it this far it is an ingredient line
	return true;
}

bool MMFImporter::loadIngredientHeader( const QString &string )
{
	if ( ( string.startsWith( "-----" ) || string.startsWith( "MMMMM" ) ) &&
	        string.length() >= 40 &&
	        ( ( string.at( string.length() / 2 ) != '-' ) ||
	          ( string.at( string.length() / 2 + 1 ) != '-' ) ||
	          ( string.at( string.length() / 2 - 1 ) != '-' ) ) ) {
		QString header( string.trimmed() );

		//get only the header name
		header.remove( QRegExp( "^MMMMM" ) );
		header.remove( QRegExp( "^-*" ) ).remove( QRegExp( "-*$" ) );

		kDebug() << "found ingredient header: " << header ;

		//merge all columns before appending to full ingredient list to maintain the ingredient order
		for ( IngredientList::iterator ing_it = m_left_col_ing.begin(); ing_it != m_left_col_ing.end(); ++ing_it ) {
			m_all_ing.append( *ing_it );
		}
		m_left_col_ing.clear();

		for ( IngredientList::iterator ing_it = m_right_col_ing.begin(); ing_it != m_right_col_ing.end(); ++ing_it ) {
			m_all_ing.append( *ing_it );
		}
		m_right_col_ing.clear();

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
		int separator_index = name_and_prep.indexOf( QRegExp( "(;|,)" ) );
		if ( separator_index != -1 ) {
			( *ing_it ).name = name_and_prep.mid( 0, separator_index ).trimmed();
			( *ing_it ).prepMethodList = ElementList::split(",",name_and_prep.mid( separator_index + 1, name_and_prep.length() ).trimmed() );
		}
	}

	//create the recipe
	Recipe new_recipe;
	new_recipe.yield.setAmount(m_servings);
	new_recipe.yield.setType(i18n("servings"));
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
	m_left_col_ing.clear();
	m_right_col_ing.clear();
	m_all_ing.clear();
	m_authors.clear();
	m_categories.clear();

	m_servings = 0;

	m_title.clear();
	m_instructions.clear();

	current_header.clear();
}

