/***************************************************************************
 *   Copyright (C) 2003 by krecipes.sourceforge.net authors                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "mmfimporter.h"

#include <kapplication.h>
#include <klocale.h>

#include <qfile.h>
#include <qtextstream.h>
#include <qstringlist.h>

#include "mixednumber.h"
#include "recipe.h"

static expand_unit_info unit_info[] = {
  {"bn",I18N_NOOP("bunch")},
  {"c" ,I18N_NOOP("cup")},
  {"cc",I18N_NOOP("cubic cm")},
  {"cg",I18N_NOOP("centigram")},
  {"cl",I18N_NOOP("centiliter")},
  {"cn",I18N_NOOP("can")},
  {"ct",I18N_NOOP("carton")},
  {"dg",I18N_NOOP("decigram")},
  {"dl",I18N_NOOP("deciliter")},
  {"dr",I18N_NOOP("drop")},
  {"ds",I18N_NOOP("dash")},
  {"ea",I18N_NOOP("each")},
  {"kg",I18N_NOOP("kilogram")},
  {"fl",I18N_NOOP("fluid ounce")},
  {"g" ,I18N_NOOP("gram")},
  {"ga",I18N_NOOP("gallon")},
  {"l" ,I18N_NOOP("liter")},
  {"lb",I18N_NOOP("pound")},
  {"lg",I18N_NOOP("large")},
  {"md",I18N_NOOP("medium")},
  {"mg",I18N_NOOP("milligram")},
  {"ml",I18N_NOOP("milliliter")},
  {"pg",I18N_NOOP("package")},
  {"pk",I18N_NOOP("package")},
  {"pn",I18N_NOOP("pinch")},
  {"pt",I18N_NOOP("pint")},
  {"oz",I18N_NOOP("ounce")},
  {"qt",I18N_NOOP("quart")},
  {"sl",I18N_NOOP("slice")},
  {"sm",I18N_NOOP("small")},
  {"t" ,I18N_NOOP("teaspoon")},
  {"tb",I18N_NOOP("tablespoon")},
  {"ts",I18N_NOOP("teaspoon")},
  {"T" ,I18N_NOOP("tablespoon")},
  {"x" ,I18N_NOOP("per serving")},
  { 0, 0 }
};

//TODO: pre-parse file and try to correct alignment errors in ingredients

MMFImporter::MMFImporter( const QString &file ) : BaseImporter()
{
	resetVars();

	QFile input( file );
	qDebug("loading file: %s",file.latin1());
	if ( input.open( IO_ReadOnly ) )
	{
		QTextStream stream( &input );
		qDebug("file opened");
		QString line;
		while ( !stream.atEnd() )
		{
			line = stream.readLine();

			if ( line.startsWith("MMMMM") )
			{
				version = VersionMMMMM;
				importMMF( stream );
				error_code = 0;
			}
			else if ( line.contains("Recipe Extracted from Meal-Master (tm) Database") )
			{
				version = FromDatabase;
				importMMF( stream );
				error_code = 0;
			}
			else if ( line.startsWith("-----") )
			{
				version = VersionNormal;
				importMMF( stream );
				error_code = 0;
			}
			else if ( line.startsWith("MM") )
			{
				version = VersionBB;
				(void)stream.readLine();
				importMMF( stream );
				error_code = 0;
			}
		}
	}
	else
		error_code = FileOpenError;
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
	qDebug("Found title: %s",m_title.latin1());

	//categories
	stream.skipWhiteSpace();
	current = stream.readLine().stripWhiteSpace();
	const char separator = ( version == FromDatabase ) ? ' ' : ',';
	QStringList categories = QStringList::split( separator, current.mid(current.find(":")+1,current.length()) );
	for ( QStringList::const_iterator it = categories.begin(); it != categories.end(); ++it )
	{
		Element new_cat;
		new_cat.name = QString(*it).stripWhiteSpace();
		qDebug("Found category: %s", new_cat.name.latin1() );
		m_categories.add( new_cat );
	}

	//servings
	stream.skipWhiteSpace();
	current = stream.readLine().stripWhiteSpace();
	if ( current.startsWith("Yield:") )
	{
		//get the number between the ":" and the next space after it
		m_servings = current.mid( current.find(":")+1,
		  current.find(" ",current.find(":")+2) - current.find(":") ).toInt();
		qDebug("Found yield: %d",m_servings);
	}
	else if ( current.startsWith("Servings:") ) //from database version
	{
		m_servings = current.mid( current.find(":")+1, current.length() ).toInt();
		qDebug("Found servings: %d",m_servings);
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
			qDebug("Found instruction line: %s",current.stripWhiteSpace().latin1());
		}

		current = stream.readLine();
	}
	m_instructions = m_instructions.stripWhiteSpace();
	//qDebug("Found instructions: %s",m_instructions.latin1());

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
		qDebug("Appending to last ingredient in column: %s",string.stripWhiteSpace().mid(1,string.length()).latin1());
		if ( list.at(list.count()-1) )
			list.at(list.count()-1)->name += " "+string.stripWhiteSpace().mid(1,string.length());

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
				unit = unit_info[i].expanded_form;
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
	list.add( new_ingredient );
	qDebug("Found ingredient: amount=%f, unit:%s, name:%s",
	  new_ingredient.amount,
	  new_ingredient.units.latin1(),
	  new_ingredient.name.latin1());

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
		qDebug("found ingredient header: %s",header.latin1());

		for (Ingredient *ing=m_left_col_ing.getFirst(); ing; ing=m_left_col_ing.getNext())
			m_all_ing.add( *ing );
		m_left_col_ing.empty();

		for (Ingredient *ing=m_right_col_ing.getFirst(); ing; ing=m_right_col_ing.getNext())
			m_all_ing.add( *ing );
		m_right_col_ing.empty();

		Ingredient title;
		title.name = header;
		title.units = ""; title.amount = 0;
		m_all_ing.add( title );
		return true;
	}
	else
		return false;
}

void MMFImporter::putDataInRecipe()
{
	for (Ingredient *ing=m_left_col_ing.getFirst(); ing; ing=m_left_col_ing.getNext())
		m_all_ing.add( *ing );
	for (Ingredient *ing=m_right_col_ing.getFirst(); ing; ing=m_right_col_ing.getNext())
		m_all_ing.add( *ing );

	//create the recipe
	Recipe *new_recipe = new Recipe;
	new_recipe->persons = m_servings;
	new_recipe->title = m_title;
	new_recipe->instructions = m_instructions;
	new_recipe->ingList = m_all_ing;
	new_recipe->categoryList = m_categories;
	new_recipe->authorList = m_authors;
	new_recipe->recipeID = -1;

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

