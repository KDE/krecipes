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

#include "propertycalculator.h"
#include "datablocks/mixednumber.h"
#include "dialogs/setupdisplay.h"
#include "image.h"
#include "krepagelayout.h"

#include <cmath> //for ceil()

//TODO: remove dependency on RecipeDB... pass the properties to this class instead of having it calculate them
HTMLExporter::HTMLExporter( RecipeDB *db, const QString& filename, const QString &format ) :
		BaseExporter( filename, format ), database( db )
{
	properties = new IngredientPropertyList;

	KConfig *config = KGlobal::config();
	config->setGroup( "Page Setup" );

	//let's do everything we can to be sure at least some layout is loaded
	QString template_filename = config->readEntry( "Template", locate( "appdata", "layouts/default.template" ) );
	if ( template_filename.isEmpty() || !QFile::exists( template_filename ) )
		template_filename = locate( "appdata", "layouts/default.template" );
	kdDebug() << "Using template file: " << template_filename << endl;

	QFile input( template_filename );
	if ( input.open( IO_ReadOnly ) ) {
		m_templateContent = QString( input.readAll() );
	}
	else
		kdDebug()<<"couldn't find/open template file"<<endl;

	//let's do everything we can to be sure at least some layout is loaded
	layout_filename = config->readEntry( "Layout", locate( "appdata", "layouts/default.klo" ) );
	if ( layout_filename.isEmpty() || !QFile::exists( layout_filename ) )
		layout_filename = locate( "appdata", "layouts/default.klo" );
	kdDebug() << "Using layout file: " << layout_filename << endl;
}

HTMLExporter::~HTMLExporter()
{
	delete properties;
}

int HTMLExporter::supportedItems() const
{
	return RecipeDB::All;
}

QString HTMLExporter::createContent( const Recipe& recipe )
{
	QString templateCopy = m_templateContent;

	QDomElement el = getLayoutAttribute( doc, "properties", "visible" );
	if ( el.isNull() || el.text() == "true" ) // Calculate the property list
		calculateProperties( recipe, database, properties );

	storePhoto( recipe, doc );

	populateTemplate( recipe, doc, templateCopy );
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
	m_error = false;

	QFile input( layout_filename );

	if ( m_templateContent.isEmpty() ) {
		QString errorStr = i18n("<html><body>\n"
			"<p><b>Error: </b>Unable to find a layout file, which is"
			" needed to view the recipe.</p>"
			"<p>Krecipes was probably not properly installed.</p>"
			"</body></html>");

		m_error = true;
		return errorStr;
	}

	QString error;
	int line;
	int column;
	if ( !doc.setContent( &input, &error, &line, &column ) ) {
		kdDebug()<<"Unable to load style information.  Continuing to create HTML..."<<endl;
	}

	//put all the recipe photos into this directory
	QDir dir;
	QFileInfo fi(fileName());
	dir.mkdir( fi.dirPath(true) + "/" + fi.baseName() + "_photos" );

	RecipeList::const_iterator recipe_it;

	QDomElement bg_element = getLayoutAttribute( doc, "background", "background-color" );

	KLocale*loc = KGlobal::locale();
	QString encoding = loc->encoding();

	QString output = "<html>";
	output += "<head>";
	output += "<meta name=\"lang\" content=\"" + loc->language() + "\">\n";
	output += "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n";
	output += QString( "<title>%1</title>" ).arg( i18n( "Krecipes Recipes" ) );

	output += "<style type=\"text/css\">\n";
	output += "body\n";
	output += "{\n";
	output += QString( "background-color: %1;\n" ).arg( bg_element.text() );
	output += "}\n";
	output += generateCSSClasses( doc );
	output += "</style>";
	output += "</head>";
	output += "<body>";

	return output;
}

QString HTMLExporter::createFooter()
{
	if ( m_error )
		return QString::null;

	return "</body></html>";
}

void HTMLExporter::storePhoto( const Recipe &recipe, const QDomDocument &doc )
{
	QDomElement photo_geom_el = getLayoutAttribute( doc, "photo", "geometry" );
	temp_photo_size = QSize(     qRound( photo_geom_el.attribute( "width" ).toDouble() ),
	                             qRound( photo_geom_el.attribute( "height" ).toDouble() )
	                        );

	int phwidth = temp_photo_size.width();

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
	temp_photo_size = QSize( pm.size() ); //preserve aspect ratio
}

void HTMLExporter::populateTemplate( const Recipe &recipe, const QDomDocument &doc, QString &content )
{
	KConfig * config = KGlobal::config();

	//=======================TITLE======================//
	content = content.replace("**TITLE**",recipe.title);

	//=======================INSTRUCTIONS======================//
	QString instr_html = QStyleSheet::escape( recipe.instructions );
	instr_html.replace( "\n", "<br />" );
	content = content.replace( "**INSTRUCTIONS**", instr_html );

	//=======================SERVINGS======================//
	QString yield_html = QString( "<b>%1: </b>%2" ).arg( i18n( "Yield" ) ).arg( recipe.yield.toString() );
	content = content.replace( "**YIELD**", yield_html );

	//=======================PREP TIME======================//
	QString preptime_html;
	if ( !recipe.prepTime.isNull() && recipe.prepTime.isValid() )
		preptime_html = QString( "<b>%1: </b>%2" ).arg( i18n( "Preparation Time" ) ).arg( recipe.prepTime.toString( "h:mm" ) );
	content = content.replace( "**PREP_TIME**", preptime_html );

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
	content = content.replace( "**PHOTO**", photo_html );

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
	content = content.replace( "**AUTHORS**", authors_html );

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

	content = content.replace( "**CATEGORIES**", categories_html );

	//=======================HEADER======================//
	QString header_html = QString( "<b>%1 #%2</b>" ).arg( i18n( "Recipe" ) ).arg( recipe.recipeID );
	content = content.replace( "**HEADER**", header_html );

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
	content = content.replace( "**INGREDIENTS**", ingredients_html );

	//=======================PROPERTIES======================//
	QString properties_html;
	IngredientProperty * prop;

	for ( prop = properties->getFirst(); prop; prop = properties->getNext() ) {
		// if the amount given is <0, it means the property calculator found that the property was undefined for some ingredients, so the amount will be actually bigger

		QString amount_str;

		double prop_amount = prop->amount;
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
		                   .arg( QStyleSheet::escape( prop->name ) )
		                   .arg( amount_str )
		                   .arg( QStyleSheet::escape( prop->units ) );
	}
	if ( !properties_html.isEmpty() ) {
		properties_html.prepend( "<ul>" );
		properties_html.append( "</ul>" );
	}
	content = content.replace( "**PROPERTIES**", properties_html );

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
	content = content.replace( "**RATINGS**", ratings_html );

	///////////TODO?: Add an "end of recipe" element here (as a separator between this and the next recipes//////////////
}

QString HTMLExporter::generateCSSClasses( const QDomDocument &doc )
{
	QString css;

	css += "UL { padding-left: 1.25em; }\n";

	QStringList classes_list;
	classes_list << "title" << "instructions" << "servings" << "prep_time" << "photo" << "authors" <<
	"categories" << "header" << "ingredients" << "properties";

	for ( QStringList::const_iterator it = classes_list.begin(); it != classes_list.end(); ++it ) {
		css += "." + *it + "\n";
		css += "{\n";
		css += readFontProperties( doc, *it );
		css += readAlignmentProperties( doc, *it );
		css += readBgColorProperties( doc, *it );
		css += readTextColorProperties( doc, *it );
		css += readVisibilityProperties( doc, *it );
		css += readBorderProperties( doc, *it );
		css += "}\n\n";
	}

	return css;
}

QString HTMLExporter::readAlignmentProperties( const QDomDocument &doc, const QString &object )
{
	QDomElement el = getLayoutAttribute( doc, object, "alignment" );

	if ( !el.isNull() ) {
		QString text;
		unsigned int alignment = el.text().toInt();

		if ( alignment & Qt::AlignLeft )
			text += "text-align: left;\n";
		if ( alignment & Qt::AlignRight )
			text += "text-align: right;\n";
		if ( alignment & Qt::AlignHCenter )
			text += "text-align: center;\n";
		if ( alignment & Qt::AlignTop )
			text += "vertical-align: top;\n";
		if ( alignment & Qt::AlignBottom )
			text += "vertical-align: bottom;\n";
		if ( alignment & Qt::AlignVCenter )
			text += "vertical-align: middle;\n";

		return text;
	}

	return QString::null;
}

QString HTMLExporter::readBorderProperties( const QDomDocument &doc, const QString &object )
{
	QDomElement el = getLayoutAttribute( doc, object, "border" );

	if ( !el.isNull() ) {
		return QString( "border: %1px %2 %3;\n" ).arg( el.attribute( "width" ) ).arg( el.attribute( "style" ) ).arg( el.attribute( "color" ) );
	}

	return QString::null;
}

QString HTMLExporter::readBgColorProperties( const QDomDocument &doc, const QString &object )
{
	QDomElement el = getLayoutAttribute( doc, object, "background-color" );

	if ( !el.isNull() )
		return QString( "background-color: %1;\n" ).arg( el.text() );

	return QString::null;
}

QString HTMLExporter::readFontProperties( const QDomDocument &doc, const QString &object )
{
	QDomElement el = getLayoutAttribute( doc, object, "font" );

	if ( !el.isNull() ) {
		QString text;
		QFont font;
		font.fromString( el.text() );

		text += QString( "font-family: %1;\n" ).arg( font.family() );
		text += QString( "font-weight: %1;\n" ).arg( font.weight() );
		text += QString( "font-size: %1pt;\n" ).arg( font.pointSize() );

		return text;
	}

	return QString::null;
}

QString HTMLExporter::readTextColorProperties( const QDomDocument &doc, const QString &object )
{
	QDomElement el = getLayoutAttribute( doc, object, "text-color" );

	if ( !el.isNull() )
		return QString( "color: %1;\n" ).arg( el.text() );

	return QString::null;
}

QString HTMLExporter::readVisibilityProperties( const QDomDocument &doc, const QString &object )
{
	QDomElement el = getLayoutAttribute( doc, object, "visible" );

	if ( !el.isNull() ) {
		bool shown = ( el.text() == "false" ) ? false : true;
		if ( shown )
			return "visibility: visible;\n";
		else
			return "visibility: hidden;\n";
	}

	return QString::null;
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

QDomElement HTMLExporter::getLayoutAttribute( const QDomDocument &doc, const QString &object, const QString &attribute )
{
	QDomNodeList node_list = doc.elementsByTagName( object );
	if ( node_list.count() == 0 ) {
		kdDebug() << "Warning: Requested object \"" << object << "\" not found." << endl;
		return QDomElement();
	}

	QDomElement object_element = node_list.item( 0 ).toElement(); //there should only be one, so we'll just take the first

	QDomNodeList l = object_element.childNodes();
	for ( unsigned i = 0; i < l.count(); i++ ) {
		QDomElement el = l.item( i ).toElement();

		if ( el.tagName() == attribute )
			return el;
	}

	kdDebug() << "Warning: Requested attribute \"" << attribute << "\" not found." << endl;
	return QDomElement();
}

QString HTMLExporter::escape( const QString & str )
{
	QString tmp( str );
	return tmp.replace( '/', "_" );
}
