/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "htmlexporter.h"

#include <qptrdict.h>
#include <qimage.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qstylesheet.h> //for QStyleSheet::escape() to escape for HTML
#include <dom/dom_element.h>
#include <qpainter.h>

#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <kprogress.h>
#include <kstandarddirs.h>
#include <kurl.h>
#include <kiconloader.h>

#include "datablocks/mixednumber.h"
#include "backends/recipedb.h"
#include "dialogs/setupdisplay.h"
#include "image.h"
#include "krepagelayout.h"

#include <cmath> //for ceil()

HTMLExporter::HTMLExporter( const QString& filename, const QString &format ) :
		BaseExporter( filename, format )
{
	KConfig *config = KGlobal::config();
	config->setGroup( "Page Setup" );

	//let's do everything we can to be sure at least some layout is loaded
	QString template_filename = config->readEntry( "Template", locate( "appdata", "layouts/Default.template" ) );
	if ( template_filename.isEmpty() || !QFile::exists( template_filename ) )
		template_filename = locate( "appdata", "layouts/Default.template" );
	kdDebug() << "Using template file: " << template_filename << endl;

	QFile templateFile( template_filename );
	if ( templateFile.open( IO_ReadOnly ) ) {
		m_templateContent = QString( templateFile.readAll() );
	}
	else
		kdDebug()<<"couldn't find/open template file"<<endl;

	//let's do everything we can to be sure at least some layout is loaded
	m_layoutFilename = config->readEntry( "Layout", locate( "appdata", "layouts/Default.klo" ) );
	if ( m_layoutFilename.isEmpty() || !QFile::exists( m_layoutFilename ) )
		m_layoutFilename = locate( "appdata", "layouts/Default.klo" );
	kdDebug() << "Using layout file: " << m_layoutFilename << endl;
}

HTMLExporter::~HTMLExporter()
{
}

int HTMLExporter::supportedItems() const
{
	int items = RecipeDB::All ^ RecipeDB::Properties;

	QMap<QString,bool>::const_iterator it = m_visibilityMap.find("properties");
	if ( it == m_visibilityMap.end() || it.data() == true )
		items |= RecipeDB::Properties;

	return RecipeDB::All;
}

QString HTMLExporter::createContent( const Recipe& recipe )
{
	QString templateCopy = m_templateContent;

	storePhoto( recipe );

	populateTemplate( recipe, templateCopy );
	return templateCopy;
}

QString HTMLExporter::createContent( const RecipeList& recipes )
{
	QString fileContent;

	RecipeList::const_iterator recipe_it;
	for ( recipe_it = recipes.begin(); recipe_it != recipes.end(); ++recipe_it ) {
		fileContent += createContent(*recipe_it);
	}

	return fileContent;
}

QString HTMLExporter::createHeader( const RecipeList & )
{
	m_visibilityMap.clear();

	KConfig *config = KGlobal::config();
	config->setGroup( "Page Setup" );

	m_error = false;

	if ( m_templateContent.isEmpty() ) {
		QString errorStr = i18n("<html><body>\n"
			"<p><b>Error: </b>Unable to find a layout file, which is"
			" needed to view the recipe.</p>"
			"<p>Krecipes was probably not properly installed.</p>"
			"</body></html>");
		m_error = true;
		return errorStr;
	}

	QFile layoutFile( m_layoutFilename );
	QString error; int line; int column;
	QDomDocument doc;
	if ( !doc.setContent( &layoutFile, &error, &line, &column ) ) {
		kdDebug()<<"Unable to load style information.  Will create HTML without it..."<<endl;
	}
	else
		processDocument(doc);

	//put all the recipe photos into this directory
	QDir dir;
	QFileInfo fi(fileName());
	dir.mkdir( fi.dirPath(true) + "/" + fi.baseName() + "_photos" );

	RecipeList::const_iterator recipe_it;

	KLocale*loc = KGlobal::locale();
	QString encoding = loc->encoding();

	QString output = "<html>";
	output += "<head>";
	output += "<meta name=\"lang\" content=\"" + loc->language() + "\">\n";
	output += "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n";
	output += QString( "<title>%1</title>" ).arg( i18n( "Krecipes Recipes" ) );

	output += "<style type=\"text/css\">\n";
	output += m_cachedCSS;
	m_cachedCSS = QString::null;
	output += "</style>";
	output += "</head>";
	output += "<body class=\"background\">";

	return output;
}

void HTMLExporter::beginObject( const QString &object )
{
	m_cachedCSS += "."+object+" { \n";
}

void HTMLExporter::endObject()
{
	m_cachedCSS += " } \n";
}

void HTMLExporter::loadBackgroundColor( const QString &/*object*/, const QColor& color )
{
	m_cachedCSS += bgColorAsCSS(color);
}

void HTMLExporter::loadFont( const QString &/*object*/, const QFont& font )
{
	m_cachedCSS += fontAsCSS(font);
}

void HTMLExporter::loadTextColor( const QString &/*object*/, const QColor& color )
{
	m_cachedCSS += textColorAsCSS(color);
}

void HTMLExporter::loadVisibility( const QString &object, bool visible )
{
	m_cachedCSS += visibilityAsCSS(visible);
	m_visibilityMap.insert(object,visible);
}

void HTMLExporter::loadAlignment( const QString &/*object*/, int alignment )
{
	m_cachedCSS += alignmentAsCSS(alignment);
}

void HTMLExporter::loadBorder( const QString &/*object*/, const KreBorder& border )
{
	m_cachedCSS += borderAsCSS(border);
}

QString HTMLExporter::createFooter()
{
	if ( m_error )
		return QString::null;

	return "</body></html>";
}

void HTMLExporter::storePhoto( const Recipe &recipe )
{
	QImage image;
	QString photo_name;
	if ( recipe.photo.isNull() ) {
		image = QImage( defaultPhoto );
		photo_name = "default_photo";
	}
	else {
		image = recipe.photo.convertToImage();
		photo_name = recipe.title;
	}

	QPixmap pm = image;//image.smoothScale( phwidth, 0, QImage::ScaleMax );

	QFileInfo fi(fileName());
	QString photo_path = fi.dirPath(true) + "/" + fi.baseName() + "_photos/" + escape( photo_name ) + ".png";
	if ( !QFile::exists( photo_path ) ) {
		pm.save( photo_path, "PNG" );
	}
}

QString HTMLExporter::HTMLIfVisible( const QString &name, const QString &html )
{
	QMap<QString,bool>::const_iterator it = m_visibilityMap.find(name);
	if ( it == m_visibilityMap.end() || it.data() == true )
		return html;
	else
		return QString::null;
}

void HTMLExporter::populateTemplate( const Recipe &recipe, QString &content )
{
	KConfig * config = KGlobal::config();

	//=======================TITLE======================//
	content = content.replace("**TITLE**",HTMLIfVisible("title",recipe.title));

	//=======================INSTRUCTIONS======================//
	QString instr_html = QStyleSheet::escape( recipe.instructions );
	instr_html.replace( "\n", "<br />" );
	content = content.replace( "**INSTRUCTIONS**", HTMLIfVisible("instructions",instr_html) );

	//=======================SERVINGS======================//
	QString yield_html = QString( "<b>%1: </b>%2" ).arg( i18n( "Yield" ) ).arg( recipe.yield.toString() );
	content = content.replace( "**YIELD**", HTMLIfVisible("yield",yield_html) );

	//=======================PREP TIME======================//
	QString preptime_html;
	if ( !recipe.prepTime.isNull() && recipe.prepTime.isValid() )
		preptime_html = QString( "<b>%1: </b>%2" ).arg( i18n( "Preparation Time" ) ).arg( recipe.prepTime.toString( "h:mm" ) );
	content = content.replace( "**PREP_TIME**", HTMLIfVisible("prep_time",preptime_html) );

	//========================PHOTO========================//
	QString photo_name;
	if ( recipe.photo.isNull() )
		photo_name = "default_photo";
	else
		photo_name = recipe.title;

	QFileInfo fi(fileName());
	QString image_url = fi.baseName() + "_photos/" + escape( photo_name ) + ".png";
	image_url = KURL::encode_string( image_url );
	QString photo_html = QString( "<img src=\"%1\" />" ).arg( image_url );
	content = content.replace( "**PHOTO**", HTMLIfVisible("photo",photo_html) );

	//=======================AUTHORS======================//
	QString authors_html;

	int counter = 0;
	for ( ElementList::const_iterator author_it = recipe.authorList.begin(); author_it != recipe.authorList.end(); ++author_it ) {
		if ( counter )
			authors_html += ", ";
		authors_html += QStyleSheet::escape( ( *author_it ).name );
		counter++;
	}
	if ( !authors_html.isEmpty() )
		authors_html.prepend( QString( "<b>%1: </b>" ).arg( i18n( "Authors" ) ) );
	content = content.replace( "**AUTHORS**", HTMLIfVisible("authors",authors_html) );

	//=======================CATEGORIES======================//
	QString categories_html;

	counter = 0;
	for ( ElementList::const_iterator cat_it = recipe.categoryList.begin(); cat_it != recipe.categoryList.end(); ++cat_it ) {
		if ( counter )
			categories_html += ", ";
		categories_html += QStyleSheet::escape( ( *cat_it ).name );
		counter++;
	}
	if ( !categories_html.isEmpty() )
		categories_html.prepend( QString( "<b>%1: </b>" ).arg( i18n( "Categories" ) ) );

	content = content.replace( "**CATEGORIES**", HTMLIfVisible("categories",categories_html) );

	//=======================HEADER======================//
	QString header_html = QString( "<b>%1 #%2</b>" ).arg( i18n( "Recipe" ) ).arg( recipe.recipeID );
	content = content.replace( "**HEADER**", HTMLIfVisible("header",header_html) );

	//=======================INGREDIENTS======================//
	QString ingredients_html;
	config->setGroup( "Formatting" );

	MixedNumber::Format number_format = ( config->readBoolEntry( "Fraction" ) ) ? MixedNumber::MixedNumberFormat : MixedNumber::DecimalFormat;

	QString ingredient_format = config->readEntry( "Ingredient", "%n%p: %a %u" );

	IngredientList list_copy = recipe.ingList; //simple workaround until I fix iterating over the list dealing with groups
	for ( IngredientList group_list = list_copy.firstGroup(); group_list.count() != 0; group_list = list_copy.nextGroup() ) {
		QString group = group_list[ 0 ].group; //just use the first's name... they're all the same
		if ( !group.isEmpty() )
			ingredients_html += "<li>" + group + ":</li><ul>";

		for ( IngredientList::const_iterator ing_it = group_list.begin(); ing_it != group_list.end(); ++ing_it ) {
			QString amount_str = MixedNumber( ( *ing_it ).amount ).toString( number_format );

			if ( (*ing_it).amount_offset > 0 )
				amount_str += "-"+MixedNumber( ( *ing_it ).amount + ( *ing_it ).amount_offset ).toString( number_format );
			else if ( ( *ing_it ).amount <= 1e-10 )
				amount_str = "";

			QString tmp_format( ingredient_format );
			tmp_format.replace( QRegExp( QString::fromLatin1( "%n" ) ), QStyleSheet::escape( ( *ing_it ).name ) );
			tmp_format.replace( QRegExp( QString::fromLatin1( "%a" ) ), amount_str );
			tmp_format.replace( QRegExp( QString::fromLatin1( "%u" ) ), QStyleSheet::escape( ( ( *ing_it ).amount + ( *ing_it ).amount_offset > 1 ) ? ( *ing_it ).units.plural : ( *ing_it ).units.name ) );
			tmp_format.replace( QRegExp( QString::fromLatin1( "%p" ) ), ( ( *ing_it ).prepMethodList.count() == 0 ) ?
			                    QString::fromLatin1( "" ) : QString::fromLatin1( "; " ) + QStyleSheet::escape( ( *ing_it ).prepMethodList.join(",") ) );

			ingredients_html += QString( "<li>%1</li>" ).arg( tmp_format );
		}

		if ( !group.isEmpty() )
			ingredients_html += "</ul>";
	}
	if ( !ingredients_html.isEmpty() ) {
		ingredients_html.prepend( "<ul>" );
		ingredients_html.append( "</ul>" );
	}
	content = content.replace( "**INGREDIENTS**", HTMLIfVisible("ingredients",ingredients_html) );

	//=======================PROPERTIES======================//
	QString properties_html;

	for ( IngredientPropertyList::const_iterator prop_it = recipe.properties.begin(); prop_it != recipe.properties.end(); ++prop_it ) {
		// if the amount given is <0, it means the property calculator found that the property was undefined for some ingredients, so the amount will be actually bigger

		QString amount_str;

		double prop_amount = (*prop_it).amount;
		if ( prop_amount > 0 ) { //TODO: make the precision configuratble
			prop_amount = double( qRound( prop_amount * 10.0 ) ) / 10.0; //not a "chemistry experiment" ;)  Let's only have one decimal place
			amount_str = beautify( KGlobal::locale() ->formatNumber( prop_amount, 5 ) );
		}
		else {
			prop_amount = double( qRound( -prop_amount * 10.0 ) ) / 10.0; //not a "chemistry experiment" ;)  Let's only have one decimal place
			amount_str = beautify( KGlobal::locale() ->formatNumber( prop_amount, 5 ) );
			amount_str += "+";
		}

		properties_html += QString( "<li>%1: <nobr>%2 %3</nobr></li>" )
		                   .arg( QStyleSheet::escape( (*prop_it).name ) )
		                   .arg( amount_str )
		                   .arg( QStyleSheet::escape( (*prop_it).units ) );
	}
	if ( !properties_html.isEmpty() ) {
		properties_html.prepend( "<ul>" );
		properties_html.append( "</ul>" );
	}
	content = content.replace( "**PROPERTIES**", HTMLIfVisible("properties",properties_html) );

	//=======================RATINGS======================//
	QString ratings_html;
	if ( recipe.ratingList.count() > 0 )
		ratings_html += QString("<b>%1:</b>").arg(i18n("Ratings"));

	for ( RatingList::const_iterator rating_it = recipe.ratingList.begin(); rating_it != recipe.ratingList.end(); ++rating_it ) {
		ratings_html += "<hr />";

		if ( !( *rating_it ).rater.isEmpty() )
			ratings_html += "<p><b>"+( *rating_it ).rater+"</b></p>";

		if ( (*rating_it).ratingCriteriaList.count() > 0 )
			ratings_html += "<table>";
		for ( RatingCriteriaList::const_iterator rc_it = (*rating_it).ratingCriteriaList.begin(); rc_it != (*rating_it).ratingCriteriaList.end(); ++rc_it ) {
			QString image_url = fi.baseName() + "_photos/" + QString::number((*rc_it).stars) + "-stars.png";
			image_url = KURL::encode_string( image_url );
			ratings_html +=  "<tr><td>"+(*rc_it).name+":</td><td><img src=\""+image_url+"\" /></td></tr>";
			if ( !QFile::exists( fi.dirPath(true) + "/" + image_url ) ) {
				QPixmap starPixmap = Rating::starsPixmap((*rc_it).stars,true);
				starPixmap.save( fi.dirPath(true) + "/" + image_url, "PNG" );
			}
			
		}
		if ( (*rating_it).ratingCriteriaList.count() > 0 )
			ratings_html += "</table>";

		if ( !( *rating_it ).comment.isEmpty() )
			ratings_html += "<p><i>"+( *rating_it ).comment+"</i></p>";
	}
	content = content.replace( "**RATINGS**", HTMLIfVisible("ratings",ratings_html) );
}

void HTMLExporter::removeHTMLFiles( const QString &filename, const QString &recipe_title )
{
	QStringList title;
	title << recipe_title;
	removeHTMLFiles( filename, title );
}

void HTMLExporter::removeHTMLFiles( const QString &filename, const QStringList &recipe_titles )
{
	//remove HTML file
	QFile old_file( filename + ".html" );
	if ( old_file.exists() )
		old_file.remove();

	//remove photos
	for ( QStringList::const_iterator it = recipe_titles.begin(); it != recipe_titles.end(); ++it ) {
		QFile photo( filename + "_photos/" + escape( *it ) + ".png" );
		if ( photo.exists() )
			photo.remove(); //remove photos in directory before removing it 
	}

	//take care of the default photo
	QFile photo( filename + "_photos/default_photo.png" );
	if ( photo.exists() ) photo.remove();

	//remove photo directory
	QDir photo_dir;
	photo_dir.rmdir( filename + "_photos" );

	for ( double d = 0.5; d < 5.5; d += 0.5 ) {
		if ( QFile::exists(filename+"_photos/"+QString::number(d)+"-stars.png") ) photo.remove(filename+"_photos/"+QString::number(d)+"-stars.png");
	}
}

QString HTMLExporter::escape( const QString & str )
{
	QString tmp( str );
	return tmp.replace( '/', "_" );
}
