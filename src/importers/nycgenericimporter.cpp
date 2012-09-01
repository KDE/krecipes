/***************************************************************************
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
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

#include <QFile>
#include <QStringList>
#include <QRegExp>
#include <QTextStream>

#include "datablocks/mixednumber.h"
#include "datablocks/recipe.h"

NYCGenericImporter::NYCGenericImporter() : BaseImporter()
{}

void NYCGenericImporter::parseFile( const QString &file )
{
	first = true;

	m_recipe.empty();

	QFile input( file );
	if ( input.open( QIODevice::ReadOnly ) ) {
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
		m_recipe.title = current;

	//categories
	while ( !( current = stream.readLine() ).isEmpty() && !stream.atEnd() ) {
		if ( current[ 0 ].isNumber() ) {
			loadIngredientLine( current );
			break;
		} //oops, this is really an ingredient line (there was no category line)

		QStringList categories = current.split( ',', QString::SkipEmptyParts );

		if ( categories.count() > 0 && categories[ 0 ].toUpper() == "none" )  //there are no categories
			break;

		for ( QStringList::const_iterator it = categories.constBegin(); it != categories.constEnd(); ++it ) {
			Element new_cat( QString( *it ).trimmed() );
			kDebug() << "Found category: " << new_cat.name ;
			m_recipe.categoryList.append( new_cat );
		}
	}

	//ingredients
	while ( !( current = stream.readLine() ).isEmpty() && !stream.atEnd() )
		loadIngredientLine( current );

	//everything else is the instructions with optional "contributor", "prep time" and "yield"
	bool found_next;
	while ( !( found_next = ( current = stream.readLine() ).startsWith( "@@@@@" ) ) && !stream.atEnd() ) {
		if ( current.startsWith( "Contributor:" ) ) {
			Element new_author( current.mid( current.indexOf( ':' ) + 1, current.length() ).trimmed() );
			kDebug() << "Found author: " << new_author.name ;
			m_recipe.authorList.append( new_author );
		}
		else if ( current.startsWith( "Preparation Time:" ) ) {
			m_recipe.prepTime = QTime::fromString( current.mid( current.indexOf( ':' ), current.length() ) );
		}
		else if ( current.startsWith( "Yield:" ) ) {
			int colon_index = current.indexOf( ':' );
			int amount_type_sep_index = current.indexOf(" ",colon_index+1);

			m_recipe.yield.setAmount(current.mid( colon_index+2, amount_type_sep_index-colon_index ).toDouble());
			m_recipe.yield.setType(current.mid( amount_type_sep_index+3, current.length() ));
		}
		else if ( current.startsWith( "NYC Nutrition Analysis (per serving or yield unit):" ) ) {
			//m_recipe.instructions += current + '\n';
		}
		else if ( current.startsWith( "NYC Nutrilink:" ) ) {
			//m_recipe.instructions += current + '\n';
		}
		else if ( !current.trimmed().isEmpty() && !current.startsWith("** Exported from Now You're Cooking!") ) {
			m_recipe.instructions += current + '\n';
		}
	}

	m_recipe.instructions = m_recipe.instructions.trimmed();
	putDataInRecipe();

	if ( found_next )
		importNYCGeneric( stream );
}

void NYCGenericImporter::putDataInRecipe()
{
	//put it in the recipe list
	add( m_recipe );

	//reset for the next recipe
	m_recipe.empty();
}

void NYCGenericImporter::loadIngredientLine( const QString &line )
{
	QString current = line;

	if ( current.contains( "-----" ) ) {
		current_header = current.trimmed();
		kDebug() << "Found ingredient header: " << current_header ;
		return ;
	}

	MixedNumber amount( 0, 0, 1 );
	QString unit;
	QString name;
	QString prep;

	QStringList ingredient_line;
	if (current.isEmpty())
		ingredient_line = QStringList();
	else
		ingredient_line = current.split( ' ', QString::SkipEmptyParts);

	bool found_amount = false;

	if ( !ingredient_line.empty() )  //probably an unnecessary check... but to be safe
	{
		MixedNumber test_amount;
		QValidator::State state;
		state = MixedNumber::fromString( ingredient_line[ 0 ], test_amount, false );
		if ( state != QValidator::Acceptable )
		{
			amount = amount + test_amount;
			ingredient_line.pop_front();
			found_amount = true;
		}
	}
	if ( !ingredient_line.empty() )  //probably an unnecessary check... but to be safe
	{
		MixedNumber test_amount;
		QValidator::State state;
		state = MixedNumber::fromString( ingredient_line[ 0 ], test_amount, false );
		if ( state == QValidator::Acceptable )
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

	int prep_sep_index = name.indexOf( QRegExp( "(--|,;;)" ) );
	if ( prep_sep_index == -1 )
		prep_sep_index = name.length();

	name = name.left( prep_sep_index ).trimmed();
	prep = name.mid( prep_sep_index+1, name.length() ).trimmed();

	Ingredient new_ingredient( name, amount.toDouble(), Unit( unit, amount.toDouble() ) );
	new_ingredient.group = current_header;
	new_ingredient.prepMethodList = ElementList::split(",",prep);
	m_recipe.ingList.append( new_ingredient );

}

