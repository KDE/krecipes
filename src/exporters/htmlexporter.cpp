/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003, 2006 Jason Kivlighn <jkivlighn@gmail.com>           *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "htmlexporter.h"

#include <QImage>
#include <QFileInfo>
#include <QDir>
#include <QPixmap>
#include <QTextDocument>
#include <dom/dom_element.h>

#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <kprogressdialog.h>
#include <kstandarddirs.h>
#include <kurl.h>
#include <kiconloader.h>

#include "datablocks/mixednumber.h"
#include "backends/recipedb.h"
#include "dialogs/setupdisplay.h"
#include "image.h"

#include <cmath> //for ceil()

HTMLExporter::HTMLExporter( const QString& filename, const QString &format ) :
		BaseExporter( filename, format )
{
	m_templateContent = "\
<table width=\"100%\">\
    <tr><td class=\"title\" colspan=\"3\" width=\"100%\">**TITLE**</td></tr>\
    <tr><td colspan=\"3\">&nbsp;</td></tr>\
    <tr>\
      <td width=\"20%\">\
        <img src=\"**PHOTO**\" class=\"photo\" />\
      </td>\
\
      <td width=\"3%\">&nbsp;</td>\
\
      <td>\
        <p class=\"overall_rating\">**OVERALL_RATING**</p>\
        <p class=\"categories\">**CATEGORIES**</p>\
        <p class=\"authors\">**AUTHORS**</p>\
        <p class=\"yield\">**YIELD**</p>\
        <p class=\"prep_time\">**PREP_TIME**</p>\
      </td>\
    </tr>\
\
    <tr><td colspan=\"3\">&nbsp;</td></tr>\
\
    <tr><td valign=\"top\" class=\"ingredients\" colspan=\"3\">**INGREDIENTS**</td></tr>\
\
    <tr><td colspan=\"3\">&nbsp;</td></tr>\
\
    <tr><td valign=\"top\" class=\"instructions\" colspan=\"3\">**INSTRUCTIONS**</td></tr>\
\
    <tr><td colspan=\"3\">&nbsp;</td></tr>\
\
    <tr><td valign=\"top\" class=\"properties\" colspan=\"3\">**PROPERTIES**</td></tr>\
\
    <tr><td colspan=\"3\">&nbsp;</td></tr>\
\
    <tr><td class=\"ratings\" colspan=\"3\">**RATINGS**</td></tr>\
</table>\
<hr size=\"3\" />\
";
}

HTMLExporter::~HTMLExporter()
{
}

int HTMLExporter::supportedItems() const
{
	int items = RecipeDB::All;
	return items;
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
	m_error = false;

	//put all the recipe photos into this directory
	QDir dir;
	QFileInfo fi(fileName());
	dir.mkdir( fi.absolutePath() + '/' + fi.baseName() + "_photos" );

	RecipeList::const_iterator recipe_it;

	KLocale*loc = KGlobal::locale();
	QString encoding = loc->encoding();

	QString output = "<html>";
	output += "<head>";
	output += "<meta name=\"lang\" content=\"" + loc->language() + "\">\n";
	output += "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n";
	output += QString( "<title>%1</title>" ).arg( i18n( "Krecipes Recipes" ) );

	output += "<style type=\"text/css\">\n";

	output += "\
.title {\
  text-align: center;\
  font-weight: bold;\
  font-size: 130%;\
}\
\
.ingredients-header, .properties-header, .ratings-header, .instructions-header {\
  border-top: 2px solid gray;\
  padding-top: 5px;\
  font-size: 120%; \
";


	output += "</style>";
	output += "</head>";
	output += "<body class=\"background\">";

	return output;
}


QString HTMLExporter::createFooter()
{
	if ( m_error )
		return QString();

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
		image = recipe.photo.toImage();
		photo_name = QString::number(recipe.recipeID);
	}

	QPixmap pm = QPixmap::fromImage( image );//image.smoothScale( phwidth, 0, QImage::ScaleMax );

	QFileInfo fi(fileName());
	QString photo_path = fi.absolutePath() + '/' + fi.baseName() + "_photos/" + photo_name + ".png";
	if ( !QFile::exists( photo_path ) ) {
		pm.save( photo_path, "PNG" );
	}
}

void HTMLExporter::populateTemplate( const Recipe &recipe, QString &content )
{
	//=======================TITLE======================//
	content = content.replace("**TITLE**",recipe.title);

	//=======================INSTRUCTIONS======================//
	QString instr_html = Qt::escape( recipe.instructions );
	instr_html.replace( '\n', "<br />" );
	if (!instr_html.isEmpty()) {
		instr_html.prepend("<h1 class=\"instructions-header\">"+i18n("Instructions")+"</h1>");
	}
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
		photo_name = QString::number(recipe.recipeID);

	QFileInfo fi(fileName());
	QString image_url = fi.baseName() + "_photos/" + escape( photo_name ) + ".png";
	content = content.replace( "**PHOTO**", image_url );

	//=======================AUTHORS======================//
	QString authors_html;

	int counter = 0;
	for ( ElementList::const_iterator author_it = recipe.authorList.begin(); author_it != recipe.authorList.end(); ++author_it ) {
		if ( counter )
			authors_html += ", ";
		authors_html += Qt::escape( ( *author_it ).name );
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
		categories_html += Qt::escape( ( *cat_it ).name );
		counter++;
	}
	if ( !categories_html.isEmpty() )
		categories_html.prepend( QString( "<b>%1: </b>" ).arg( i18n( "Categories" ) ) );

	content = content.replace( "**CATEGORIES**", categories_html );

	//=======================INGREDIENTS======================//
	QString ingredients_html;
	KConfigGroup config = KGlobal::config()->group( "Formatting" );
	bool useAbbreviations = config.readEntry("AbbreviateUnits", false);

	MixedNumber::Format number_format = ( config.readEntry( "Fraction", false ) ) ? MixedNumber::MixedNumberFormat : MixedNumber::DecimalFormat;

	int cols = 2;
	int per_col = recipe.ingList.count() / cols;
	if ( recipe.ingList.count() % cols != 0 ) //round up if division is not exact
		per_col++;

	int count = 0;
	IngredientList list_copy = recipe.ingList; //simple workaround until I fix iterating over the list dealing with groups
	for ( IngredientList group_list = list_copy.firstGroup(); group_list.count() != 0; group_list = list_copy.nextGroup() ) {
		QString group = group_list[ 0 ].group; //just use the first's name... they're all the same

		bool loneHeader = false;
		if ( count != 0 && count % per_col == 0 ) {
			loneHeader = true;
			if ( !group.isEmpty() )
				ingredients_html += "</ul>";
			ingredients_html.append("</ul></td><td valign=\"top\"><ul>");
			if ( !group.isEmpty() )
				ingredients_html += "<li class=\"ingredient-group\" style=\"page-break-after: avoid\">" + group + ":</li><ul>";
		}
		else {
			if ( !group.isEmpty() )
				ingredients_html += "<li class=\"ingredient-group\" style=\"page-break-after: avoid\">" + group + ":</li><ul>";
		}

		for ( IngredientList::const_iterator ing_it = group_list.begin(); ing_it != group_list.end(); ++ing_it, ++count ) {
			if ( count != 0 && count % per_col == 0 && !loneHeader ) {
				if ( !group.isEmpty() )
					ingredients_html += "</ul>";
				ingredients_html.append("</ul></td><td valign=\"top\"><ul>");
				if ( !group.isEmpty() )
					ingredients_html += "<ul>";
			}

			QString amount_str = MixedNumber( ( *ing_it ).amount ).toString( number_format );

			if ( (*ing_it).amount_offset > 0 )
				amount_str += '-'+MixedNumber( ( *ing_it ).amount + ( *ing_it ).amount_offset ).toString( number_format );
			else if ( ( *ing_it ).amount <= 1e-10 )
				amount_str = "";

			QString unit = ( *ing_it ).units.determineName( ( *ing_it ).amount + ( *ing_it ).amount_offset, useAbbreviations );

			QString tmp_format;
			tmp_format += "<span class=\"ingredient-amount\">"+amount_str+" </span>";
			tmp_format += "<span class=\"ingredient-unit\">"+Qt::escape(unit)+" </span>";
			tmp_format += "<span class=\"ingredient-name\">"+Qt::escape( ( *ing_it ).name )+"</span>";
			tmp_format += "<span class=\"ingredient-prep-methods\">"+(( ( *ing_it ).prepMethodList.count() == 0 ) ?
			                    QString::fromLatin1( "" ) : QString::fromLatin1( "; " ) + Qt::escape( ( *ing_it ).prepMethodList.join(",") ))+"</span>";

			if ( (*ing_it).substitutes.count() > 0 )
				tmp_format += ", "+i18n("OR");

			ingredients_html += QString( "<li>%1</li>" ).arg( tmp_format );

			for ( Ingredient::SubstitutesList::const_iterator sub_it = (*ing_it).substitutes.begin(); sub_it != (*ing_it).substitutes.end(); ) {
				QString amount_str = MixedNumber( ( *sub_it ).amount ).toString( number_format );
	
				if ( (*ing_it).amount_offset > 0 )
					amount_str += '-'+MixedNumber( ( *sub_it ).amount + ( *sub_it ).amount_offset ).toString( number_format );
				else if ( ( *sub_it ).amount <= 1e-10 )
					amount_str = "";
	
				QString unit = ( *sub_it ).units.determineName( ( *sub_it ).amount + ( *sub_it ).amount_offset, config.readEntry("AbbreviateUnits", false) );

				QString tmp_format;
				tmp_format += "<span class=\"ingredient-amount\">"+amount_str+" </span>";
				tmp_format += "<span class=\"ingredient-unit\">"+Qt::escape(unit)+" </span>";
				tmp_format += "<span class=\"ingredient-name\">"+Qt::escape( ( *sub_it ).name )+"</span>";
				tmp_format += "<span class=\"ingredient-prep-methods\">"+(( ( *sub_it ).prepMethodList.count() == 0 ) ?
														QString::fromLatin1( "" ) : QString::fromLatin1( "; " ) + Qt::escape( ( *sub_it ).prepMethodList.join(",") ))+"</span>";

				++sub_it;
				if ( sub_it != (*ing_it).substitutes.end() )
					tmp_format += ", "+i18n("OR");
				ingredients_html += QString( "<li>%1</li>" ).arg( tmp_format );
			}
		}

		if ( !group.isEmpty() )
			ingredients_html += "</ul>";
	}
	if ( !ingredients_html.isEmpty() ) {
		ingredients_html.prepend( "<table><tr><td valign=\"top\"><ul>" );
		ingredients_html.append( "</ul></td></tr></table>" );
		ingredients_html.prepend("<h1 class=\"ingredients-header\">"+i18n("Ingredients")+"</h1>");
	}
	content = content.replace( "**INGREDIENTS**", ingredients_html );

	//=======================PROPERTIES======================//
	QString properties_html;

	QStringList hiddenList = config.readEntry("HiddenProperties", QStringList());
	IngredientPropertyList visibleProperties;
	for ( IngredientPropertyList::const_iterator prop_it = recipe.properties.begin(); prop_it != recipe.properties.end(); ++prop_it ) {
		if ( hiddenList.indexOf((*prop_it).name) == -1 ) 
			visibleProperties.append( *prop_it );
	}

	cols = 3;
	per_col = visibleProperties.count() / cols;
	if ( visibleProperties.count() % cols != 0 ) //round up if division is not exact
		per_col++;

	count = 0;
	for ( IngredientPropertyList::const_iterator prop_it = visibleProperties.constBegin(); prop_it != visibleProperties.constEnd(); ++prop_it ) {
		if ( count != 0 && count % per_col == 0 )
			properties_html.append("</ul></td><td valign=\"top\"><ul>");

		// if the amount given is <0, it means the property calculator found that the property was undefined for some ingredients, so the amount will be actually bigger

		QString amount_str;

		double prop_amount = (*prop_it).amount;
		if ( prop_amount > 0 ) { //TODO: make the precision configuratble
			prop_amount = double( qRound( prop_amount * 10.0 ) ) / 10.0; //not a "chemistry experiment" ;)  Let's only have one decimal place
			amount_str = beautify( KGlobal::locale() ->formatNumber( prop_amount, 5 ) );
		}
		else
			amount_str = '0';

		properties_html += QString( "<li>%1: <nobr>%2 %3</nobr></li>" )
		                   .arg( Qt::escape( (*prop_it).name ) )
		                   .arg( amount_str )
		                   .arg( Qt::escape( (*prop_it).units ) );

		++count;
	}

	if ( !properties_html.isEmpty() ) {
		properties_html.prepend( "<table><tr><td valign=\"top\"><ul>" );
		properties_html.append( "</ul></td></tr></table>" );
		properties_html.prepend("<h1 class=\"properties-header\">"+i18n("Properties")+"</h1>");
	}
	content = content.replace( "**PROPERTIES**", properties_html );

	//=======================RATINGS======================//
	QString ratings_html;
	if ( recipe.ratingList.count() > 0 )
		ratings_html += QString("<h1 class=\"ratings-header\">%1</h1>").arg(i18n("Ratings"));

	int rating_total = 0;
	double rating_sum = 0;
	for ( RatingList::const_iterator rating_it = recipe.ratingList.begin(); rating_it != recipe.ratingList.end(); ++rating_it ) {
        rating_total += rating_it->numberOfRatingCriterias();
        rating_sum += rating_it->sum();

		ratings_html += "<hr />";

		if ( !( *rating_it ).rater().isEmpty() )
			ratings_html += "<p><b>"+( *rating_it ).rater()+"</b></p>";

		if ( (*rating_it).hasRatingCriterias() )
			ratings_html += "<table>";
		foreach ( RatingCriteria rc, (*rating_it).ratingCriterias() ) {
			QString image_url = fi.baseName() + "_photos/" + QString::number(rc.stars()) + "-stars.png";
			ratings_html +=  "<tr><td>"+rc.name()+":</td><td><img src=\""+image_url+"\" /></td></tr>";
			if ( !QFile::exists( fi.absolutePath() + '/' + image_url ) ) {
				QPixmap starPixmap = Rating::starsPixmap(rc.stars());
				starPixmap.save( fi.absolutePath() + '/' + image_url, "PNG" );
				kDebug() << "saving: " << fi.absolutePath() + '/' + image_url ;
			}
		}
		if ( (*rating_it).hasRatingCriterias() )
			ratings_html += "</table>";

		if ( !( *rating_it ).comment().isEmpty() )
			ratings_html += "<p><i>"+( *rating_it ).comment()+"</i></p>";
	}
	content = content.replace( "**RATINGS**", ratings_html );

	QString overall_html;
	if ( rating_total > 0 ) {
		double average = int(2*rating_sum/rating_total)/2;
		overall_html += QString("<b>%1:</b>").arg(i18n("Overall Rating"));
		QString image_url = fi.baseName() + "_photos/" + QString::number(average) + "-stars.png";
		overall_html +=  "<img src=\""+ image_url+"\" />";
		if ( !QFile::exists( fi.absolutePath() + '/' + image_url ) ) {
			QPixmap starPixmap = Rating::starsPixmap(average);
			starPixmap.save( fi.absolutePath() + '/' + image_url, "PNG" );
		}
	}
	content = content.replace( "**OVERALL_RATING**", overall_html );
}

void HTMLExporter::removeHTMLFiles( const QString &filename, int recipe_id )
{
	QList<int> id;
	id << recipe_id;
	removeHTMLFiles( filename, id );
}

void HTMLExporter::removeHTMLFiles( const QString &filename, const QList<int> &recipe_ids )
{
	//remove HTML file
	QFile old_file( filename + ".html" );
	if ( old_file.exists() )
		old_file.remove();

	//remove photos
	for ( QList<int>::const_iterator it = recipe_ids.begin(); it != recipe_ids.end(); ++it ) {
		QFile photo( filename + "_photos/" + QString::number(*it) + ".png" );
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
	return tmp.replace( '/', '_' );
}
