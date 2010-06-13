/***************************************************************************
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "rezkonvimporter.h"

#include <kapplication.h>
#include <klocale.h>
#include <kdebug.h>

#include <QFile>
#include <QRegExp>
#include <QTextStream>

#include "datablocks/mixednumber.h"

RezkonvImporter::RezkonvImporter() : BaseImporter()
{}

RezkonvImporter::~RezkonvImporter()
{}

void RezkonvImporter::parseFile( const QString &filename )
{
	QFile input( filename );

	if ( input.open( QIODevice::ReadOnly ) ) {
		QTextStream stream( &input );
		stream.skipWhiteSpace();

		QString line;

		while ( !stream.atEnd() ) {
			line = stream.readLine();

			if ( line.contains( QRegExp( "^=====.*REZKONV.*", Qt::CaseInsensitive ) ) ) {
				QStringList raw_recipe;
				while ( !( line = stream.readLine() ).contains( QRegExp( "^=====\\s*$" ) ) && !stream.atEnd() )
					raw_recipe << line;

				readRecipe( raw_recipe );
			}
		}

		if ( fileRecipeCount() == 0 )
			setErrorMsg( i18n( "No recipes found in this file." ) );
	}
	else
		setErrorMsg( i18n( "Unable to open file." ) );
}

void RezkonvImporter::readRecipe( const QStringList &raw_recipe )
{
	kapp->processEvents(); //don't want the user to think its frozen... especially for files with thousands of recipes

	Recipe recipe;

	QStringList::const_iterator text_it = raw_recipe.begin();
	m_end_it = raw_recipe.end();

	//title (Titel)
	text_it++;
	recipe.title = ( *text_it ).mid( ( *text_it ).indexOf( ":" ) + 1, ( *text_it ).length() ).trimmed();
	kDebug() << "Found title: " << recipe.title ;

	//categories (Kategorien):
	text_it++;
	QStringList categories;
	if ( ( *text_it ).mid( ( *text_it ).indexOf( ":" ) + 1, ( *text_it ).length() ).isEmpty() )
		categories = QStringList();
	else
		categories = ( *text_it ).mid( ( *text_it ).indexOf( ":" ) + 1, ( *text_it ).length() ).split( ',', QString::SkipEmptyParts );

	for ( QStringList::const_iterator it = categories.constBegin(); it != categories.constEnd(); ++it ) {
		Element new_cat;
		new_cat.name = QString( *it ).trimmed();
		kDebug() << "Found category: " << new_cat.name ;
		recipe.categoryList.append( new_cat );
	}

	//yield (Menge)
	text_it++;
	//get the number between the ":" and the next space after it
	QString yield_str = ( *text_it ).trimmed();
	yield_str.remove( QRegExp( "^Menge:\\s*" ) );
	int sep_index = yield_str.indexOf( ' ' );
	if ( sep_index != -1 )
		recipe.yield.setType(yield_str.mid( sep_index+1 ));
	double amount = 0.0, amountOffset = 0.0;
	readRange( yield_str.mid( 0, sep_index ), amount, amountOffset );
	recipe.yield.setAmount(amount);
	recipe.yield.setAmountOffset(amountOffset);
	kDebug() << "Found yield: " << recipe.yield.amount();

	bool is_sub = false;
	bool last_line_empty = false;
	text_it++;
	while ( text_it != raw_recipe.end() ) {
		if ( ( *text_it ).isEmpty() ) {
			last_line_empty = true;
			text_it++;
			continue;
		}

		if ( ( *text_it ).contains( QRegExp( "^=====.*=$" ) ) )  //is a header
		{
			if ( ( *text_it ).contains( "quelle", Qt::CaseInsensitive ) )
			{
				loadReferences( text_it, recipe );
				break; //reference lines are the last before the instructions
			}
			else
				loadIngredientHeader( *text_it, recipe );
		}

		//if it has no more than two spaces followed by a non-digit
		//then we'll assume it is a direction line
		else if ( last_line_empty && ( *text_it ).contains( QRegExp( "^\\s{0,2}[^\\d\\s=]" ) ) )
			break;
		else
			loadIngredient( *text_it, recipe, is_sub );

		last_line_empty = false;
		text_it++;
	}

	loadInstructions( text_it, recipe );

	add
		( recipe );

	current_header.clear();
}

void RezkonvImporter::loadIngredient( const QString &string, Recipe &recipe, bool &is_sub )
{
	Ingredient new_ingredient;
	new_ingredient.amount = 0; //amount not required, so give default of 0

	QRegExp cont_test( "^-{1,2}" );
	if ( string.trimmed().contains( cont_test ) ) {
		QString name = string.trimmed();
		name.remove( cont_test );
		kDebug() << "Appending to last ingredient: " << name ;
		if ( !recipe.ingList.isEmpty() )  //so it doesn't crash when the first ingredient appears to be a continuation of another
			recipe.ingList.last().name += ' ' + name;

		return ;
	}

	//amount
	if ( !string.mid( 0, 7 ).trimmed().isEmpty() )
		readRange( string.mid( 0, 7 ), new_ingredient.amount, new_ingredient.amount_offset );

	//unit
	QString unit_str = string.mid( 8, 9 ).trimmed();
	new_ingredient.units = Unit( unit_str, new_ingredient.amount );

	//name and preparation method
	new_ingredient.name = string.mid( 18, string.length() - 18 ).trimmed();

	//separate out the preparation method
	QString name_and_prep = new_ingredient.name;
	int separator_index = name_and_prep.indexOf( "," );
	if ( separator_index != -1 ) {
		new_ingredient.name = name_and_prep.mid( 0, separator_index ).trimmed();
		new_ingredient.prepMethodList = ElementList::split(",",name_and_prep.mid( separator_index + 1, name_and_prep.length() ).trimmed() );
	}

	//header (if present)
	new_ingredient.group = current_header;

	bool last_is_sub = is_sub;
	if ( !new_ingredient.prepMethodList.isEmpty() &&
	new_ingredient.prepMethodList.last().name == "or" ) {
		new_ingredient.prepMethodList.pop_back();
		is_sub = true;
	}
	else
		is_sub = false;

	if ( last_is_sub )
		recipe.ingList.last().substitutes.append(new_ingredient);
	else
		recipe.ingList.append( new_ingredient );
}

void RezkonvImporter::loadIngredientHeader( const QString &string, Recipe &/*recipe*/ )
{
	QString header = string;
	header.remove( QRegExp( "^=*" ) ).remove( QRegExp( "=*$" ) );
	header = header.trimmed();

	kDebug() << "found ingredient header: " << header ;

	current_header = header;
}

void RezkonvImporter::loadInstructions( QStringList::const_iterator &text_it, Recipe &recipe )
{
	QString instr;
	QRegExp rx_title( "^:{0,1}\\s*O-Titel\\s*:" );
	QString line;
	text_it++;
	while ( text_it != m_end_it ) {
		line = *text_it;

		//titles longer than the line width are rewritten here
		if ( line.contains( rx_title ) ) {
			line.remove( rx_title );
			recipe.title = line.trimmed();

			QRegExp rx_line_cont( ":\\s*>{0,1}\\s*:" );
			while ( ( line = *text_it ).contains( rx_line_cont ) ) {
				line.remove( rx_line_cont );
				recipe.title += line;

				text_it++;
			}
			kDebug() << "Found long title: " << recipe.title ;
		}
		else {
			if ( line.trimmed().isEmpty() && ( (text_it+1) != m_end_it ) )
				instr += "\n\n";

			instr += line.trimmed();
		}

		text_it++;
	}

	recipe.instructions = instr;
}

void RezkonvImporter::loadReferences( QStringList::const_iterator &text_it, Recipe &recipe )
{
	kDebug() << "Found source header" ;

	text_it++;
	while ( text_it != m_end_it && !text_it->trimmed().isEmpty() ) {
		QRegExp rx_line_begin( "^\\s*-{0,2}\\s*" );

		QRegExp rx_creation_date = QRegExp( "^\\s*-{0,2}\\s*Erfasst \\*RK\\*", Qt::CaseInsensitive );
		if ( ( *text_it ).contains( rx_creation_date ) )  // date followed by typist
		{
			QString date = *text_it;
			date.remove( rx_creation_date ).remove( QRegExp( " von\\s*$" ) );

			// Date is given as DD.MM.YY
			QString s = date.section( '.', 0, 0 );
			int day = s.toInt();

			s = date.section( '.', 1, 1 );
			int month = s.toInt();

			s = date.section( '.', 2, 2 );
			int year = s.toInt();
			year += 1900;
			if ( year < 1970 )
				year += 100; //we'll assume nothing has been created before 1970 (y2k issues :p)

			recipe.ctime = QDateTime(QDate(year,month,day));

			#if 0
			//typist
			text_it++;
			QString typist = = *text_it;
			typist.remove( rx_line_begin );
			#endif

		}
		else //everything else is an author
		{
			if ( ( *text_it ).contains( rx_line_begin ) ) {
				QString author = *text_it;
				author.remove( rx_line_begin );

				recipe.authorList.append( Element( author ) );
			}
			else
				break;
		}
		text_it++;
	}
}

void RezkonvImporter::readRange( const QString &range_str, double &amount, double &amount_offset )
{
	QString from = range_str.section( '-', 0, 0 );
	QString to   = range_str.section( '-', 1, 1 );

	amount = MixedNumber::fromString( from ).toDouble();

	if ( !to.trimmed().isEmpty() )
		amount_offset = MixedNumber::fromString( to ).toDouble() - amount;
}
