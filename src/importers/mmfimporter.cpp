/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
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
#include <qtextstream.h>
#include <qstringlist.h>

#include "mixednumber.h"
#include "recipe.h"
#include "mmdata.h"

//TODO: pre-parse file and try to correct alignment errors in ingredients?

MMFImporter::MMFImporter( const QString &file ) : BaseImporter()
{
	resetVars();

	QFile input( file );

	if ( input.open( IO_ReadOnly ) )
	{
		QTextStream stream( &input );
		stream.skipWhiteSpace();

		QString line;
		while ( !stream.atEnd() )
		{
			line = stream.readLine();

			if ( line.startsWith("MMMMM") )
			{
				version = VersionMMMMM;
				importMMF( stream );
			}
			else if ( line.contains("Recipe Extracted from Meal-Master (tm) Database") )
			{
				version = FromDatabase;
				importMMF( stream );
			}
			else if ( line.startsWith("-----") )
			{
				version = VersionNormal;
				importMMF( stream );
			}
			else if ( line.startsWith("MM") )
			{
				version = VersionBB;
				(void)stream.readLine();
				importMMF( stream );
			}
			else
			{
				setErrorMsg( i18n("File does not appear to be a valid Meal-Master export.") );
				return;
			}

			stream.skipWhiteSpace();
		}
	}
	else
		setErrorMsg(i18n("Unable to open file."));
}

MMFImporter::~MMFImporter()
{
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
	m_title = current.mid( current.find(":")+1, current.length() ).stripWhiteSpace();
	kdDebug()<<"Found title: "<<m_title<<endl;

	//categories
	stream.skipWhiteSpace();
	current = stream.readLine().stripWhiteSpace();
	const char separator = ( version == FromDatabase ) ? ' ' : ',';
	QStringList categories = QStringList::split( separator, current.mid(current.find(":")+1,current.length()) );
	for ( QStringList::const_iterator it = categories.begin(); it != categories.end(); ++it )
	{
		Element new_cat;
		new_cat.name = QString(*it).stripWhiteSpace();
		kdDebug()<<"Found category: "<<new_cat.name<<endl;
		m_categories.append( new_cat );
	}

	//servings
	stream.skipWhiteSpace();
	current = stream.readLine().stripWhiteSpace();
	if ( current.startsWith("Yield:") )
	{
		//get the number between the ":" and the next space after it
		m_servings = current.mid( current.find(":")+1,
		  current.find(" ",current.find(":")+2) - current.find(":") ).toInt();
		kdDebug()<<"Found yield: "<<m_servings<<endl;
	}
	else if ( current.startsWith("Servings:") ) //from database version
	{
		m_servings = current.mid( current.find(":")+1, current.length() ).toInt();
		kdDebug()<<"Found servings: "<<m_servings<<endl;
	}

	//=======================VARIABLE FORMAT===================//
	//read lines until ending is found
	//each line is either an ingredient, ingredient header, or instruction
	bool instruction_found = false;

	(void)stream.readLine();
	current = stream.readLine();
	while ( current.stripWhiteSpace() != "MMMMM" &&
	        current.stripWhiteSpace() != "-----" &&
		current.stripWhiteSpace() != "-----------------------------------------------------------------------------" &&
		!stream.atEnd() )
	{
		bool col_one_used = loadIngredientLine( current.left(41), m_left_col_ing );
		if ( col_one_used ) //only check for second column if there is an ingredient in the first column
			loadIngredientLine( current.mid( 41, current.length() ), m_right_col_ing );

		if ( instruction_found && col_one_used )
		{
			addWarningMsg( QString(i18n("While loading recipe \"%1\" "
			  "an ingredient line was found after the directions. "
			  "While this is valid, it most commonly indicates an incorrectly "
			  "formatted recipe.")).arg(m_title) );
		}

		if ( !col_one_used &&
		     !loadIngredientHeader( current.stripWhiteSpace() ) )
		{
			if ( current.stripWhiteSpace() != "" )
				instruction_found = true;
			m_instructions += current.stripWhiteSpace() + "\n";
			kdDebug()<<"Found instruction line: "<<current.stripWhiteSpace()<<endl;
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
	if ( string.stripWhiteSpace() == "" )
		return false;

	Ingredient new_ingredient;
	new_ingredient.amount = 0; //amount not required, so give default of 0

	if ( string.mid( 11, 1 ) == "-" && (string.mid( 0, 11 ).stripWhiteSpace() == "") ) //continuation of previous ingredient
	{
		kdDebug()<<"Appending to last ingredient in column: "<<string.stripWhiteSpace().mid(1,string.length())<<endl;
		if ( !list.isEmpty() )
			(*list.at(list.count()-1)).name += " "+string.stripWhiteSpace().mid(1,string.length());

		return true;
	}

	if ( string.mid(0,7).stripWhiteSpace() != "" )
	{
		bool ok;
		MixedNumber amount = MixedNumber::fromString(string.mid(0,7).stripWhiteSpace(),&ok);
		if ( !ok )
			return false;
		else
			new_ingredient.amount = amount.toDouble();
	}

	if ( string[7] != ' ' )
		return false;

	if ( string.mid( 8, 2 ).stripWhiteSpace() != "" )
	{
		bool is_unit = false;
		QString unit( string.mid( 8, 2 ).stripWhiteSpace() );
		for ( int i = 0; unit_info[i].short_form; i++ )
		{
			if ( unit_info[i].short_form == unit )
			{
				is_unit = true;
				if ( new_ingredient.amount <= 1 )
					unit = unit_info[i].expanded_form;
				else
					unit = unit_info[i].plural_expanded_form;

				break;
			}
		}
		if ( !is_unit )
		{/*This gives too many false warnings...
			addWarningMsg( QString(i18n("Unit \"%1\" not recognized. "
			  "Used in the context of \"%2\".  If this shouldn't be an ingredient line (i.e. is part of the instructions), "
			  "then you can safely ignore this warning, and the recipe will be correctly imported.")).arg(unit).arg(string.stripWhiteSpace()) );*/
			return false;
		}

		new_ingredient.units = unit;
	}

	if ( string[10] != ' ' || string[11] == ' ' )
		return false;

	new_ingredient.name = string.mid( 11, 32 ).stripWhiteSpace();

	//if we made it this far it is an ingredient line
	list.append( new_ingredient );
	kdDebug()<<"Found ingredient: amount="<<new_ingredient.amount
	  <<", unit:"<<new_ingredient.units
	  <<", name:"<<new_ingredient.name<<endl;

	return true;
}

//for now, make header an ingredient
bool MMFImporter::loadIngredientHeader( const QString &string )
{
	if ( (string.startsWith("-----") || string.startsWith("MMMMM") ) &&
	     string.length() >= 40 &&
	     (  (string.at( string.length()/2 ) != "-") ||
	        (string.at( string.length()/2 + 1 ) != "-") ||
		(string.at( string.length()/2 - 1 ) != "-") ) )
	{
		QString header(string.stripWhiteSpace());
		header = header.mid( 10, header.length() - 20 );
		kdDebug()<<"found ingredient header: "<<header<<endl;

		for ( IngredientList::const_iterator ing_it = m_left_col_ing.begin(); ing_it != m_left_col_ing.end(); ++ing_it )
			m_all_ing.append( *ing_it );
		m_left_col_ing.empty();

		for ( IngredientList::const_iterator ing_it = m_right_col_ing.begin(); ing_it != m_right_col_ing.end(); ++ing_it )
			m_all_ing.append( *ing_it );
		m_right_col_ing.empty();

		Ingredient title;
		title.name = header;
		title.units = ""; title.amount = 0;
		m_all_ing.append( title );
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

	//create the recipe
	Recipe new_recipe;
	new_recipe.persons = m_servings;
	new_recipe.title = m_title;
	new_recipe.instructions = m_instructions;
	new_recipe.ingList = m_all_ing;
	new_recipe.categoryList = m_categories;
	new_recipe.authorList = m_authors;
	new_recipe.recipeID = -1;

	//put it in the recipe list
	add( new_recipe );

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
}

