/***************************************************************************
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "cookmlexporter.h"

#include <QBuffer>
#include <qdom.h>
#include <QImage>
#include <qimagewriter.h>

#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <ktemporaryfile.h>
#include <kcodecs.h>
#include <kglobal.h>
#include <kstandarddirs.h>

#include "backends/recipedb.h"

CookMLExporter::CookMLExporter( const QString& filename, const QString &format ) :
		BaseExporter( filename, format )
{}


CookMLExporter::~CookMLExporter()
{}

int CookMLExporter::supportedItems() const
{
	return RecipeDB::All ^ RecipeDB::Ratings;
}

QString CookMLExporter::createHeader( const RecipeList& )
{
	QString xml = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
	xml += "<!DOCTYPE cookml PUBLIC \"-\" \"cookml.dtd\">";
	xml += "<cookml version=\"1.0.13\" prog=\"Krecipes\" progver=\""+krecipes_version()+"\">";
	return xml;
}

QString CookMLExporter::createFooter()
{
	return "</cookml>";
}

QString CookMLExporter::createContent( const RecipeList& recipes )
{
	QString xml;
	QDomDocument doc;

	RecipeList::const_iterator recipe_it;
	for ( recipe_it = recipes.begin(); recipe_it != recipes.end(); ++recipe_it ) {
		QDomElement recipe_tag = doc.createElement( "recipe" );
		recipe_tag.setAttribute( "lang", ( KGlobal::locale() ) ->language() );

		//cookml_tag.appendChild( recipe_tag );

		QDomElement head_tag = doc.createElement( "head" );
		head_tag.setAttribute( "title", ( *recipe_it ).title );
		head_tag.setAttribute( "servingqty", ( *recipe_it ).yield.amount() );
		head_tag.setAttribute( "servingtype", ( *recipe_it ).yield.type() );
		head_tag.setAttribute( "rid", "" ); //FIXME:what's this...recipe ID?? //FIXME: i18n("") is gettext-reserved
		recipe_tag.appendChild( head_tag );

		for ( ElementList::const_iterator cat_it = ( *recipe_it ).categoryList.begin(); cat_it != ( *recipe_it ).categoryList.end(); ++cat_it ) {
			QDomElement cat_tag = doc.createElement( "cat" );
			cat_tag.appendChild( doc.createTextNode( ( *cat_it ).name ) );
			head_tag.appendChild( cat_tag );
		}

		for ( ElementList::const_iterator author_it = ( *recipe_it ).authorList.begin(); author_it != ( *recipe_it ).authorList.end(); ++author_it ) {
			QDomElement sourceline_tag = doc.createElement( "sourceline" );
			sourceline_tag.appendChild( doc.createTextNode( ( *author_it ).name ) );
			head_tag.appendChild( sourceline_tag );
		}

		QDomElement picbin_tag = doc.createElement( "picbin" );
		picbin_tag.setAttribute( "format", "JPG" );

		QByteArray data;
		QBuffer buffer( &data );
		buffer.open( QIODevice::WriteOnly );
		QImageWriter iio( &buffer, "JPEG" );
		iio.write( ( *recipe_it ).photo.toImage() );
		//( *recipe_it ).photo.save( &buffer, "JPEG" ); don't need QImageIO in QT 3.2

		picbin_tag.appendChild( doc.createTextNode( KCodecs::base64Encode( data, true ) ) );
		head_tag.appendChild( picbin_tag );

		QDomElement part_tag = doc.createElement( "part" );
		for ( IngredientList::const_iterator ing_it = ( *recipe_it ).ingList.begin(); ing_it != ( *recipe_it ).ingList.end(); ++ing_it ) {
			QDomElement ingredient_tag = doc.createElement( "ingredient" );
			ingredient_tag.setAttribute( "qty", QString::number( ( *ing_it ).amount ) );
			ingredient_tag.setAttribute( "unit", (*ing_it).units.determineName(( *ing_it ).amount, /*useAbbrev=*/false) );
			ingredient_tag.setAttribute( "item", ( *ing_it ).name );
			ingredient_tag.setAttribute( "preparation", ( *ing_it ).prepMethodList.join(",") );
			part_tag.appendChild( ingredient_tag );
		}
		recipe_tag.appendChild( part_tag );

		QDomElement preparation_tag = doc.createElement( "preparation" );
		recipe_tag.appendChild( preparation_tag );

		QDomElement text_tag = doc.createElement( "text" );
		preparation_tag.appendChild( text_tag );
		text_tag.appendChild( doc.createTextNode( ( *recipe_it ).instructions ) );

		xml += recipe_tag.text();
	}

	return xml;
}
