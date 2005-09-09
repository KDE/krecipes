/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "nycgenericimporter.h"

#include <kapplication.h>
#include <klocale.h>
#include <kdebug.h>

#include <qfile.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include <qregexp.h>

#include "datablocks/mixednumber.h"
#include "datablocks/recipe.h"

NYCGenericImporter::NYCGenericImporter() : BaseImporter()
{}

void NYCGenericImporter::parseFile( const QString &file )
{
	first = true;

	resetVars();

	QFile input( file );
	if ( input.open( IO_ReadOnly ) ) {
		QTextStream stream( &input );
		stream.skipWhiteSpace();

		if ( !stream.atEnd() && stream.readLine().startsWith( "@@@@@" ) )
			importNYCGeneric( stream );
		else {
			setErrorMsg( i18n( "File does not appear to be a valid NYC export." ) );
			return ;
		}
	}
	else
		setErrorMsg( i18n( "Unable to open file." ) );
}

NYCGenericImporter::~NYCGenericImporter()
{}

void NYCGenericImporter::importNYCGeneric( QTextStream &stream )
{
	kapp->processEvents(); //don't want the user to think its frozen... especially for files with thousands of recipes

	QString current;

	stream.skipWhiteSpace();

	//title
	while ( !( current = stream.readLine() ).isEmpty() && !stream.atEnd() )
		m_title = current;
	kdDebug() << "Found title: " << m_title << endl;

	//categories
	while ( !( current = stream.readLine() ).isEmpty() && !stream.atEnd() ) {
		if ( current[ 0 ].isNumber() ) {
			loadIngredientLine( current );
			break;
		} //oops, this is really an ingredient line (there was no category line)

		QStringList categories = QStringList::split( ',', current );

		if ( categories.count() > 0 && categories[ 0 ].upper() == "none" )  //there are no categories
			break;

		for ( QStringList::const_iterator it = categories.begin(); it != categories.end(); ++it ) {
			Element new_cat( QString( *it ).stripWhiteSpace() );
			kdDebug() << "Found category: " << new_cat.name << endl;
			m_categories.append( new_cat );
		}
	}

	//ingredients
	while ( !( current = stream.readLine() ).isEmpty() && !stream.atEnd() )
		loadIngredientLine( current );

	//everything else is the instructions with optional "contributor", "prep time" and "yield"
	bool found_next;
	while ( !( found_next = ( current = stream.readLine() ).startsWith( "@@@@@" ) ) && !stream.atEnd() ) {
		if ( current.startsWith( "Contributor:" ) ) {
			Element new_author( current.mid( current.find( ':' ) + 1, current.length() ).stripWhiteSpace() );
			kdDebug() << "Found author: " << new_author.name << endl;
			m_authors.append( new_author );
		}
		else if ( current.startsWith( "Preparation Time:" ) ) {
			m_preptime = QTime::fromString( current.mid( current.find( ':' ), current.length() ) );
			kdDebug() << "Found preptime: " << m_preptime.toString( "hh:mm" ) << endl;
		}
		else if ( current.startsWith( "Yield:" ) ) {
			m_servings = current.mid( current.find( ':' ), current.length() ).toInt();
			kdDebug() << "Found servings: " << m_servings << endl;
		}
		else {
			kdDebug() << "Found instruction line: " << current << endl;
			m_instructions += current + "\n";
		}
	}

	m_instructions = m_instructions.stripWhiteSpace();
	putDataInRecipe();

	if ( found_next )
		importNYCGeneric( stream );
}

void NYCGenericImporter::putDataInRecipe()
{
	//create the recipe
	Recipe new_recipe;
	new_recipe.yield.amount = m_servings;
	new_recipe.yield.type = i18n("servings");
	new_recipe.title = m_title;
	new_recipe.instructions = m_instructions;
	new_recipe.ingList = m_ingredients;
	new_recipe.categoryList = m_categories;
	new_recipe.authorList = m_authors;
	new_recipe.prepTime = m_preptime;
	new_recipe.recipeID = -1;

	//put it in the recipe list
	add
		( new_recipe );

	//reset for the next recipe to use these variables
	resetVars();
}

void NYCGenericImporter::resetVars()
{
	m_ingredients.empty();
	m_authors.clear();
	m_categories.clear();

	m_servings = 0;
	m_preptime = QTime( 0, 0 );

	m_title = QString::null;
	m_instructions = QString::null;

	current_header = QString::null;
}

void NYCGenericImporter::loadIngredientLine( const QString &line )
{
	QString current = line;

	if ( current.contains( "-----" ) ) {
		current_header = current.stripWhiteSpace();
		kdDebug() << "Found ingredient header: " << current_header << endl;
		return ;
	}

	MixedNumber amount( 0, 0, 1 );
	QString unit;
	QString name;
	QString prep;

	QStringList ingredient_line = QStringList::split( ' ', current );

	bool found_amount = false;

	if ( !ingredient_line.empty() )  //probably an unnecessary check... but to be safe
	{
		bool ok;
		MixedNumber test_amount = MixedNumber::fromString( ingredient_line[ 0 ], &ok );
		if ( ok )
		{
			amount = amount + test_amount;
			ingredient_line.pop_front();
			found_amount = true;
		}
	}
	if ( !ingredient_line.empty() )  //probably an unnecessary check... but to be safe
	{
		bool ok;
		MixedNumber test_amount = MixedNumber::fromString( ingredient_line[ 0 ], &ok );
		if ( ok )
		{
			amount = amount + test_amount;
			ingredient_line.pop_front();
			found_amount = true;
		}
	}

	if ( found_amount ) {
		unit = ingredient_line[ 0 ];
		ingredient_line.pop_front();
	}

	//now join each separate part of ingredient (name, unit, amount)
	name = ingredient_line.join( " " );

	int prep_sep_index = name.find( QRegExp( "[,;]" ) );
	name = name.left( prep_sep_index );
	prep = name.mid( prep_sep_index, name.length() );

	Ingredient new_ingredient( name, amount.toDouble(), Unit( unit, amount.toDouble() ), -1 );
	new_ingredient.group = current_header;
	new_ingredient.prepMethodList.append( Element(prep) );
	m_ingredients.append( new_ingredient );

}

