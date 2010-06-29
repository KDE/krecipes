/***************************************************************************
*   Copyright © 2007 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "xsltexporter.h"

#include "kreexporter.h"

#include <q3ptrdict.h>
#include <QImage>
#include <QFileInfo>
#include <QDir>
#include <QList>
#include <QPixmap>

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

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

#include <cmath> //round

#include "backends/recipedb.h"
#include "dialogs/setupdisplay.h"
#include "image.h"
#include "krepagelayout.h"

const char* i18n_strings[] = {
  "I18N_INSTRUCTIONS", I18N_NOOP("Instructions"),
  "I18N_OVERALL_RATING", I18N_NOOP("Overall Rating"),
  "I18N_CATEGORIES", I18N_NOOP("Categories"),
  "I18N_YIELD", I18N_NOOP("Yield"),
  "I18N_PREP", I18N_NOOP("Prep"),
  "I18N_AUTHORS", I18N_NOOP("Authors"),
  "I18N_AMOUNT", I18N_NOOP("Amount"),
  "I18N_INGREDIENT", I18N_NOOP("Ingredient"),
  "I18N_INGREDIENTS", I18N_NOOP("Ingredients"),
  "I18N_PREPARATION", I18N_NOOP("Preparation"),
  "I18N_PROPERTIES", I18N_NOOP("Properties"),
  "I18N_RATINGS", I18N_NOOP("Ratings"),
  "I18N_OR", I18N_NOOP("OR"),
  "I18N_REVIEWS", I18N_NOOP("reviews"),
  NULL
};
#define NUM_I18N_STRINGS 28

XSLTExporter::XSLTExporter( const QString& filename, const QString &format ) :
		BaseExporter( filename, format )
{
	KConfigGroup config = KGlobal::config()->group( "Page Setup" );

	//let's do everything we can to be sure at least some layout is loaded
	QString template_filename = config.readEntry( "Template", KStandardDirs::locate( "appdata", "layouts/Default.xsl" ) );
	if ( template_filename.isEmpty() || !QFile::exists( template_filename )
     || template_filename.endsWith(".template") ) //handle the transition to xslt
		template_filename = KStandardDirs::locate( "appdata", "layouts/Default.xsl" );

	kDebug() << "Using template file: " << template_filename ;

	setTemplate( template_filename );

	//let's do everything we can to be sure at least some layout is loaded
	m_layoutFilename = config.readEntry( "Layout", KStandardDirs::locate( "appdata", "layouts/None.klo" ) );
	if ( m_layoutFilename.isEmpty() || !QFile::exists( m_layoutFilename ) )
		m_layoutFilename = KStandardDirs::locate( "appdata", "layouts/None.klo" );
}

XSLTExporter::~XSLTExporter()
{
}

void XSLTExporter::setStyle( const QString &filename )
{
	m_layoutFilename = filename;
}

void XSLTExporter::setTemplate( const QString &filename )
{
	QFile templateFile( filename );
	if ( templateFile.open( QIODevice::ReadOnly ) ) {
		m_templateFilename = filename;
	}
	else
		kDebug()<<"couldn't find/open template file";
}

int XSLTExporter::supportedItems() const
{
	int items = RecipeDB::All;

	QMap<QString,bool>::const_iterator it;
	for ( it = m_visibilityMap.begin(); it != m_visibilityMap.end(); ++it ) {
		if (it.value() == false) {
			if (it.key() == "authors") {
				items ^= RecipeDB::Authors;
			} else if (it.key() == "categories") {
				items ^= RecipeDB::Categories;
			} else if (it.key() == "ingredients") {
				items ^= RecipeDB::Ingredients;
			} else if (it.key() == "instructions") {
				items ^= RecipeDB::Instructions;
			} else if (it.key() == "photo") {
				items ^= RecipeDB::Photo;
			} else if (it.key() == "prep_time") {
				items ^= RecipeDB::PrepTime;
			} else if (it.key() == "properties") {
				items ^= RecipeDB::Properties;
			} else if (it.key() == "ratings") {
				items ^= RecipeDB::Ratings;
			} else if (it.key() == "title") {
				items ^= RecipeDB::Title;
			} else if (it.key() == "yield") {
				items ^= RecipeDB::Yield;
			}
		}
	}

	return items;
}

QString XSLTExporter::createFooter()
{
	xsltCleanupGlobals();
	xmlCleanupParser();

	return QString();
}

QString XSLTExporter::createHeader( const RecipeList & )
{
	QFile layoutFile( m_layoutFilename );
	QString error; int line; int column;
	QDomDocument doc;
	if ( !doc.setContent( &layoutFile, &error, &line, &column ) ) {
		kDebug()<<"Unable to load style information.  Will create HTML without it...";
	}
	else
		processDocument(doc);

	return QString();
}

QString XSLTExporter::createContent( const RecipeList &recipes )
{
	m_error = false;

	//put all the recipe photos into this directory
	QDir dir;
	QFileInfo fi(fileName());
	dir.mkdir( fi.absolutePath() + '/' + fi.baseName() + "_photos" );

	RecipeList::const_iterator recipe_it;
	for ( recipe_it = recipes.begin(); recipe_it != recipes.end(); ++recipe_it ) {
		storePhoto( *recipe_it );

		int rating_total = 0;
		double rating_sum = 0;
		for ( RatingList::const_iterator rating_it = (*recipe_it).ratingList.begin(); rating_it != (*recipe_it).ratingList.end(); ++rating_it ) {
			foreach ( RatingCriteria rc, (*rating_it).ratingCriterias() ) {
				QString image_url = fi.baseName() + "_photos/" + QString::number(rc.stars()) + "-stars.png";
				if ( !QFile::exists( fi.absolutePath() + '/' + image_url ) ) {
					QPixmap starPixmap = Rating::starsPixmap(rc.stars());
					starPixmap.save( fi.absolutePath() + '/' + image_url, "PNG" );
				}
				rating_total++;
				rating_sum += rc.stars();
			}
		}

		if ( rating_total > 0 ) {
			double average = round(2*rating_sum/rating_total)/2;
			QString image_url = fi.baseName() + "_photos/" + QString::number(average) + "-stars.png";
			if ( !QFile::exists( fi.absolutePath() + '/' + image_url ) ) {
				QPixmap starPixmap = Rating::starsPixmap(average);
				starPixmap.save( fi.absolutePath() + '/' + image_url, "PNG" );
			}
		}
	}

	KLocale*loc = KGlobal::locale();
	QString encoding = loc->encoding();

	QString cssContent;
	QFileInfo info(m_templateFilename);
	QFile cssFile(info.absolutePath() + '/' + info.baseName() + ".css");
	kDebug()<<info.absolutePath() + '/' + info.baseName() + ".css";
	if ( cssFile.open( QIODevice::ReadOnly ) ) {
		cssContent = QString( cssFile.readAll() );
	}

	cssContent += m_cachedCSS;

	QFile linkedCSSFile(fi.absolutePath() + "/style.css");
	if (linkedCSSFile.open(QIODevice::WriteOnly)) {
		QTextStream stream( &linkedCSSFile );
		stream << cssContent;
	}

	m_cachedCSS.clear();

	KreExporter *exporter = new KreExporter( NULL, "unused", "*.kreml", false );
	QString buffer;
	QTextStream stream(&buffer,QIODevice::WriteOnly);
	exporter->writeStream(stream,recipes);
	delete exporter;

	QByteArray content = buffer.toUtf8();
	xmlDocPtr kremlDoc = xmlReadMemory(content, content.length(), "noname.xml", "utf-8", 0);
	if (kremlDoc == NULL) {
		kDebug() << "Failed to parse document" ;
		return i18n("<html><b>Error:</b> Problem with KreML exporter.  Please export the recipe you are trying to view as KreML and attach it to a bug report to a Krecipes developer.</html>");
	}

	//const char *filename = "/home/jason/svn/utils/krecipes/layouts/Default.xsl";
	QByteArray filename = m_templateFilename.toUtf8();
	xsltStylesheetPtr xslt = xsltParseStylesheetFile((const xmlChar*)filename.constData());
	if ( !xslt ) {
		return i18n("<html><b>Error:</b> Bad template: %1.  Use \"Edit->Page Setup...\" to select a new template.</html>",QString( filename ));
	}

	QFileInfo imgDirInfo(m_templateFilename);
	const char *params[NUM_I18N_STRINGS+3];
	int i = 0;
	params[i++] = "imgDir";
	QByteArray imgDir = '\''+imgDirInfo.absolutePath().toUtf8()+'\'';
	params[i++] = imgDir.data();

	for ( uint j = 0; j < NUM_I18N_STRINGS; j+=2 ) {
		params[i++] = i18n_strings[j];

		QString translatedString = '\''+i18n(i18n_strings[j+1])+'\'';
		params[i++] = qstrdup(translatedString.toUtf8());
	}
	params[i] = NULL;
	xmlDocPtr htmlDoc = xsltApplyStylesheet(xslt, kremlDoc, (const char**)params);

	for ( uint j = 0; j < NUM_I18N_STRINGS; j+=2 ) {
		delete[] params[2+j+1];
	}

	xmlChar *xmlOutput;
	int length;
	xsltSaveResultToString(&xmlOutput, &length, htmlDoc, xslt);

	QString output = QString::fromUtf8((char*)xmlOutput);

	xsltFreeStylesheet(xslt);

	xmlFreeDoc(kremlDoc);
	xmlFreeDoc(htmlDoc);

	return output;
}

void XSLTExporter::beginObject( const QString &object )
{
	m_cachedCSS += '.'+object+", ."+object+" td { \n";
}

void XSLTExporter::endObject()
{
	m_cachedCSS += " } \n";
}

void XSLTExporter::loadBackgroundColor( const QString &/*object*/, const QColor& color )
{
	m_cachedCSS += bgColorAsCSS(color);
}

void XSLTExporter::loadFont( const QString &/*object*/, const QFont& font )
{
	m_cachedCSS += fontAsCSS(font);
}

void XSLTExporter::loadTextColor( const QString &/*object*/, const QColor& color )
{
	m_cachedCSS += textColorAsCSS(color);
}

void XSLTExporter::loadVisibility( const QString &object, bool visible )
{
	m_cachedCSS += visibilityAsCSS(visible);
	m_visibilityMap.insert(object,visible);
}

void XSLTExporter::loadAlignment( const QString &/*object*/, int alignment )
{
	m_cachedCSS += alignmentAsCSS(alignment);
}

void XSLTExporter::loadBorder( const QString &/*object*/, const KreBorder& border )
{
	m_cachedCSS += borderAsCSS(border);
}

void XSLTExporter::loadColumns( const QString & object, int cols )
{
	m_columnsMap.insert(object,cols);
}

void XSLTExporter::storePhoto( const Recipe &recipe )
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
		kDebug() << "photo: " << photo_path ;
		pm.save( photo_path, "PNG" );
	}
}

