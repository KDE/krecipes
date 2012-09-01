/***************************************************************************
*   Copyright © 2003 Richard Lärkäng                                      *
*   Copyright © 2003-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "recipemlimporter.h"

#include <QFile>

#include <klocale.h>
#include <kdebug.h>

#include "datablocks/recipe.h"
#include "datablocks/mixednumber.h"

RecipeMLImporter::RecipeMLImporter() : BaseImporter()
{}

void RecipeMLImporter::parseFile( const QString& file )
{
	QFile input( file );
	if ( input.open( QIODevice::ReadOnly ) ) {
		QDomDocument doc;
		QString error;
		int line;
		int column;
		if ( !doc.setContent( &input, &error, &line, &column ) ) {
			setErrorMsg( i18n( "\"%1\" at line %2, column %3.  This may not be a RecipeML file." , error , line , column ));
			return ;
		}

		QDomElement recipeml = doc.documentElement();

		if ( recipeml.tagName() != "recipeml" ) {
			setErrorMsg( i18n( "This file does not appear to be a valid RecipeML archive." ) );
			return ;
		}

		QDomNodeList l = recipeml.childNodes();

		for ( int i = 0 ; i < l.count(); i++ ) {
			QDomElement el = l.item( i ).toElement();
			QString tagName = el.tagName();

			if ( tagName == "meta" )
				continue;
			else if ( tagName == "recipe" )
				readRecipemlRecipe( el );
			else if ( tagName == "menu" )
				readRecipemlMenu( el );
			else
				kDebug() << "Unknown tag within <recipeml>: " << tagName ;
		}
	}
	else
		setErrorMsg( i18n( "Unable to open file." ) );
}

RecipeMLImporter::~RecipeMLImporter()
{}

void RecipeMLImporter::readRecipemlRecipe( const QDomElement& recipe_element )
{
	recipe.empty();

	QDomNodeList l = recipe_element.childNodes();

	for ( int i = 0; i < l.count(); i++ ) {
		QDomElement el = l.item( i ).toElement();
		QString tagName = el.tagName();

		if ( tagName == "head" )
			readRecipemlHead( el );
		else if ( tagName == "ingredients" )
			readRecipemlIngs( el );
		else if ( tagName == "directions" )
			readRecipemlDirections( el );
	else if ( tagName == "description" ) {} //TODO: what do we do with this?
		else if ( tagName == "equipment" ) {} //TODO: what do we do with this?
		else if ( tagName == "nutrition" ) {} //TODO: what do we do with this?
		else if ( tagName == "diet-exchanges" ) {} //TODO: what do we do with this?
		else
			kDebug() << "Unknown tag within <recipe>: " << el.tagName() ;
	}

	add
		( recipe );
}

void RecipeMLImporter::readRecipemlHead( const QDomElement& head )
{
	QDomNodeList l = head.childNodes();
	for ( int i = 0 ; i < l.count(); i++ ) {
		QDomElement el = l.item( i ).toElement();
		QString tagName = el.tagName();

		if ( tagName == "title" )
			recipe.title = el.text().trimmed();
		else if ( tagName == "subtitle" )
			recipe.title += ": " + el.text().trimmed();
	else if ( tagName == "version" ) {} //TODO: what do we do with this?
		else if ( tagName == "source" )
			readRecipemlSrcItems( el );
		else if ( tagName == "categories" ) {
			QDomNodeList categories = el.childNodes();
			for ( int j = 0; j < categories.count(); j++ ) {
				QDomElement c = categories.item( j ).toElement();
				if ( c.tagName() == "cat" ) {
					recipe.categoryList.append( Element( c.text() ) );
				}
			}
		}
		else if ( tagName == "description" )
			recipe.instructions += "\n\nDescription: " + el.text().trimmed();
		else if ( tagName == "preptime" )
			readRecipemlPreptime( el );
		else if ( tagName == "yield" ) {
			QDomNodeList yieldChildren = el.childNodes();
			for ( int j = 0; j < yieldChildren.count(); j++ ) {
				QDomElement y = yieldChildren.item( j ).toElement();
				QString tagName = y.tagName();
				if ( tagName == "range" ) {
					double amount = 0.0;
					double amountOffset = 0.0;
					readRecipemlRange( y, amount, amountOffset );
					recipe.yield.setAmount(amount);
					recipe.yield.setAmountOffset(amountOffset);
				}
				else if ( tagName == "unit" )
					recipe.yield.setType(y.text());
				else
					kDebug() << "Unknown tag within <yield>: " << y.tagName() ;
			}
		}
		else
			kDebug() << "Unknown tag within <head>: " << el.tagName() ;
	}
}

void RecipeMLImporter::readRecipemlIngs( const QDomElement& ings )
{
	QDomNodeList l = ings.childNodes();
	for ( int i = 0 ; i < l.count(); i++ ) {
		QDomElement el = l.item( i ).toElement();
		QString tagName = el.tagName();

		if ( tagName == "ing" )
			readRecipemlIng( el );
		else if ( tagName == "ing-div" )  //NOTE: this can have the "type" attribute
		{
			QString header;
			QDomNodeList ingDiv = el.childNodes();
			for ( int j = 0; j < ingDiv.count(); j++ )
			{
				QDomElement cEl = ingDiv.item( j ).toElement();
				if ( cEl.tagName() == "title" )
					header = cEl.text().trimmed();
				else if ( cEl.tagName() == "description" ) {} //TODO: what do we do with this?
				else if ( cEl.tagName() == "ing" )
					readRecipemlIng( cEl, 0, header );
				else if ( tagName == "note" ) {} //TODO: what do we do with this?
				else
					kDebug() << "Unknown tag within <ing-div>: " << cEl.tagName() ;
			}
		}
	else if ( tagName == "note" ) {} //TODO: what do we do with this?
		else
			kDebug() << "Unknown tag within <ingredients>: " << el.tagName() ;
	}
}

void RecipeMLImporter::readRecipemlIng( const QDomElement& ing, Ingredient *ing_parent, const QString &header )
{
	Ingredient new_ing;

	QDomNodeList ingChilds = ing.childNodes();

	QString name, unit, size, prep_method;
	Ingredient quantity;
	quantity.amount = 1;// default quantity assumed by RecipeML DTD

	for ( int j = 0; j < ingChilds.count(); j++ ) {
		QDomElement ingChild = ingChilds.item( j ).toElement();
		QString tagName = ingChild.tagName();

		if ( tagName == "amt" ) {
			QDomNodeList amtChilds = ingChild.childNodes();

			for ( int k = 0; k < amtChilds.count(); k++ ) {
				QDomElement amtChild = amtChilds.item( k ).toElement();

				if ( amtChild.tagName() == "qty" )
					readRecipemlQty( amtChild, quantity );
				else if ( amtChild.tagName() == "size" )
					size = amtChild.text().trimmed();
				else if ( amtChild.tagName() == "unit" )
					unit = amtChild.text().trimmed();
				else
					kDebug() << "Unknown tag within <amt>: " << amtChild.tagName() ;
			}
		}
		else if ( tagName == "item" ) {
			name = ingChild.text().trimmed();
			if ( ing.attribute( "optional", "no" ) == "yes" )
				prep_method = "(optional)";
		}
		else if ( tagName == "prep" ) { //FIXME: this overwrite the optional attribute
			prep_method = ingChild.text().trimmed();
		}
		else if ( tagName == "alt-ing" )
			readRecipemlIng( ingChild, &new_ing, header );
		else
			kDebug() << "Unknown tag within <ing>: " << ingChild.tagName() ;
	}

	if ( !size.isEmpty() )
		unit.prepend( size + ' ' );

	new_ing.name = name;
	new_ing.units = Unit( unit, quantity.amount+quantity.amount_offset );
	new_ing.amount = quantity.amount;
	new_ing.amount_offset = quantity.amount_offset;
	new_ing.group = header;
	new_ing.prepMethodList = ElementList::split(",",prep_method);

	if ( !ing_parent )
		recipe.ingList.append(new_ing);
	else
		ing_parent->substitutes.append( new_ing );
}

void RecipeMLImporter::readRecipemlDirections( const QDomElement& dirs )
{
	QDomNodeList l = dirs.childNodes();

	QStringList directions;

	for ( int i = 0 ; i < l.count(); i++ ) {
		QDomElement el = l.item( i ).toElement();

		if ( el.tagName() == "step" )
			directions.append( el.text().trimmed() );
		else
			kDebug() << "Unknown tag within <directions>: " << el.tagName() ;
	}

	QString directionsText;

	if ( directions.count() > 1 ) {
		for ( int i = 1; i <= directions.count(); i++ ) {
			if ( i != 1 ) {
				directionsText += "\n\n";
			}

			QString sWith = QString( "%1. " ).arg( i );
			QString text = directions[ i - 1 ];
			if ( !text.trimmed().startsWith( sWith ) )
				directionsText += sWith;
			directionsText += text;
		}
	}
	else
		directionsText = directions[ 0 ];

	recipe.instructions = directionsText;
}

void RecipeMLImporter::readRecipemlMenu( const QDomElement& menu_el )
{
	QDomNodeList l = menu_el.childNodes();
	for ( int i = 0 ; i < l.count(); i++ ) {
		QDomElement el = l.item( i ).toElement();
		QString tagName = el.tagName();

		if ( tagName == "head" )
			readRecipemlHead( el );
	else if ( tagName == "description" ) {} //TODO: what do we do with this?
		else if ( tagName == "recipe" )
			readRecipemlRecipe( el );
		else
			kDebug() << "Unknown tag within <menu>: " << tagName ;
	}
}

void RecipeMLImporter::readRecipemlSrcItems( const QDomElement& sources )
{
	QDomNodeList l = sources.childNodes();
	for ( int i = 0 ; i < l.count(); i++ ) {
		QDomElement srcitem = l.item( i ).toElement();
		QString tagName = srcitem.tagName();

		if ( tagName == "srcitem" )
			recipe.authorList.append( Element( srcitem.text().trimmed() ) );
		else
			kDebug() << "Unknown tag within <source>: " << tagName ;
	}
}

void RecipeMLImporter::readRecipemlPreptime( const QDomElement &preptime )
{
	QDomNodeList l = preptime.childNodes();
	for ( int i = 0 ; i < l.count(); i++ ) {
		QDomElement el = l.item( i ).toElement();
		QString tagName = el.tagName();

		if ( tagName == "time" ) {
			int qty = 0;
			QString timeunit;

			QDomNodeList time_l = el.childNodes();
			for ( int i = 0 ; i < time_l.count(); i++ ) {
				QDomElement time_el = time_l.item( i ).toElement();
				QString time_tagName = time_el.tagName();

				if ( time_tagName == "qty" )
					qty = time_el.text().toInt();
				else if ( time_tagName == "timeunit" )
					timeunit = time_el.text();
				else
					kDebug() << "Unknown tag within <time>: " << time_tagName ;
			}

			int minutes = 0;
			int hours = 0;
			if ( timeunit == "minutes" )
				minutes = qty;
			else if ( timeunit == "hours" )
				hours = qty;
			else
				kDebug() << "Unknown timeunit: " << timeunit ;

			recipe.prepTime = QTime( hours + minutes / 60, minutes % 60 );
		}
		else
			kDebug() << "Unknown tag within <preptime>: " << tagName ;
	}
}

void RecipeMLImporter::readRecipemlQty( const QDomElement &qty, Ingredient &ing )
{
	QDomNodeList qtyChilds = qty.childNodes();

	for ( int i = 0; i < qtyChilds.count(); i++ ) {
		QDomElement qtyChild = qtyChilds.item( i ).toElement();
		QString tagName = qtyChild.tagName();
		if ( tagName == "range" )
			readRecipemlRange( qtyChild, ing.amount, ing.amount_offset );
		else if ( tagName.isEmpty() ) {
			MixedNumber number;
			MixedNumber::fromString( qty.text(), number, false );
			ing.amount = number.toDouble();
		} else
			kDebug() << "Unknown tag within <qty>: " << tagName ;
	}
}

void RecipeMLImporter::readRecipemlRange( const QDomElement& range, double &amount, double &amount_offset )
{
	QDomNodeList rangeChilds = range.childNodes();
	double q1 = 1, q2 = 0; // default quantity assumed by RecipeML DTD
	MixedNumber number;
	for ( int j = 0; j < rangeChilds.count(); j++ ) {
		QDomElement rangeChild = rangeChilds.item( j ).toElement();
		QString subTagName = rangeChild.tagName();
		if ( subTagName == "q1" ) {
			MixedNumber::fromString( rangeChild.text(), number, false );
			q1 = number.toDouble();
		} else if ( subTagName == "q2" ) {
			MixedNumber::fromString( rangeChild.text(), number, false );
			q2 = number.toDouble();
		} else
			kDebug() << "Unknown tag within <range>: " << subTagName ;
	}

	amount = q1;
	amount_offset = q2-q1;
}
