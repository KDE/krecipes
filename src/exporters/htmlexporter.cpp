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

#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <kprogress.h>
#include <kstandarddirs.h>

#include "propertycalculator.h"
#include "mixednumber.h"
#include "gui/setupdisplay.h"
#include "image.h"

//TODO: remove dependency on RecipeDB... pass the properties to this class instead of having it calculate them
HTMLExporter::HTMLExporter( RecipeDB *db, const QString& filename, const QString &format, int width ) :
  BaseExporter( filename, format ), database(db), m_width(width)
{
	div_elements.setAutoDelete(true);
	properties = new IngredientPropertyList;
}


HTMLExporter::~HTMLExporter()
{
	delete properties;
}

QString HTMLExporter::createContent( const RecipeList& recipes )
{
	if ( recipes.count() == 0 )
		return "<html></html>";

	setProgressBarTotalSteps( recipes.count() * 2 ); //we'll be looping through all the recipes twice

	KConfig *config = KGlobal::config();
	QString recipeHTML;

	//Creates initial layout and saves to config file
	SetupDisplay::createSetupIfNecessary();

	//Recipe recipe = recipes[0];
	RecipeList::const_iterator recipe_it;

	recipeHTML += "<html><head>";
	recipeHTML += QString("<title>%1</title>").arg( (recipes.count() == 1) ? recipes[0].title : i18n("Krecipes Recipes") );

	//loop through recipes and only create the css properties
	int offset = 0;
	recipeHTML += "<STYLE type=\"text/css\">\n";
	for ( recipe_it = recipes.begin(); recipe_it != recipes.end(); ++recipe_it )
	{
		// Calculate the property list
		calculateProperties( *recipe_it, database, properties );
		offset = createBlocks( *recipe_it, offset ) + 15;

		config->setGroup("BackgroundSetup");
		QColor color = config->readColorEntry( "BackgroundColor" );
		recipeHTML += "BODY\n";
		recipeHTML += "{\n";
		recipeHTML += QString("background-color: %1;\n").arg(color.name());
		recipeHTML += "}\n";

		for ( DivElement *div = div_elements.first(); div; div = div_elements.next() )
			recipeHTML += div->generateCSS();

		if ( progressBarCanceled() ) return QString::null;
		advanceProgressBar();
	}
	recipeHTML += "</STYLE></head><body>\n";

	//put all the recipe photos into this directory
	QDir dir;
	QFileInfo fi( *file );
	dir.mkdir( fi.dirPath()+"/"+filename+"_photos" );

	//now loop through the recipes, generating the content
	for ( recipe_it = recipes.begin(); recipe_it != recipes.end(); ++recipe_it )
	{
		// Calculate the property list
		calculateProperties( *recipe_it, database, properties );
		(void)createBlocks( *recipe_it );
		storePhoto( *recipe_it );

		for ( DivElement *div = div_elements.first(); div; div = div_elements.next() )
			recipeHTML += div->generateHTML();

		if ( progressBarCanceled() ) return QString::null;
		advanceProgressBar();
	}

	// Close HTML
	recipeHTML+="</body></html>\n";

	return recipeHTML;
}

void HTMLExporter::storePhoto( const Recipe &recipe )
{
	KConfig *config = KGlobal::config();
	config->setGroup("PhotoSetup");
	temp_photo_geometry = config->readRectEntry("Geometry");

	int phwidth = (int) (temp_photo_geometry.width()/100.0*m_width); // Scale to this dialog
	int phheight =(int) (temp_photo_geometry.height()/100.0*m_width); // Scale to this dialog

	QImage image;
	if (recipe.photo.isNull())
		image = QImage(defaultPhoto);
	else
		image = recipe.photo.convertToImage();

	QPixmap pm = image.smoothScale(phwidth, phheight, QImage::ScaleMin);
	QFileInfo fi(*file);
	pm.save(fi.dirPath()+"/"+filename+"_photos/"+recipe.title+".png","PNG");
	temp_photo_geometry = QRect(temp_photo_geometry.topLeft(),pm.size()); //preserve aspect ratio
}

int HTMLExporter::createBlocks( const Recipe &recipe, int offset )
{
	div_elements.clear();
	QFont default_font;

	QRect *geometry;
	DivElement *new_element;

	CustomRectList geometries;
	geometries.setAutoDelete(true);
	QPtrDict<DivElement> geom_contents;

	KConfig *config = KGlobal::config();

	//=======================TITLE======================//
	config->setGroup("TitleSetup");
	geometry = new QRect( config->readRectEntry( "Geometry" ) );
	geometry->moveBy( 0, offset );
	geometries.append( geometry );

	new_element = new DivElement( "title_"+QString::number(recipe.recipeID), recipe.title);

	readFontProperties( new_element, config );
	readAlignmentProperties( new_element, config );
	readBgColorProperties( new_element, config );
	readTextColorProperties( new_element, config );
	readVisibilityProperties( new_element, config );

	geom_contents.insert( geometry, new_element );
	div_elements.append( new_element );

	//=======================INSTRUCTIONS======================//

	config->setGroup("InstructionsSetup");
	geometry = new QRect( config->readRectEntry( "Geometry" ) );
	geometry->moveBy( 0, offset );
	geometries.append( geometry );

	QString instr_html = recipe.instructions;
	instr_html.replace("\n","<BR>");
	new_element = new DivElement( "instructions_"+QString::number(recipe.recipeID), instr_html );

	readFontProperties( new_element, config );
	readAlignmentProperties( new_element, config );
	readBgColorProperties( new_element, config );
	readTextColorProperties( new_element, config );
	readVisibilityProperties( new_element, config );

	geom_contents.insert( geometry, new_element );
	div_elements.append( new_element );

	//=======================SERVINGS======================//
	config->setGroup("ServingsSetup");
	geometry = new QRect( config->readRectEntry( "Geometry" ) );
	geometry->moveBy( 0, offset );
	geometries.append( geometry );

	QString servings_html = QString("<b>%1: </b>%2").arg(i18n("Servings")).arg(recipe.persons);
	new_element = new DivElement( "servings_"+QString::number(recipe.recipeID), servings_html );

	readFontProperties( new_element, config );
	readAlignmentProperties( new_element, config );
	readBgColorProperties( new_element, config );
	readTextColorProperties( new_element, config );
	readVisibilityProperties( new_element, config );

	geom_contents.insert( geometry, new_element );
	div_elements.append( new_element );

	//========================PHOTO========================//
	config->setGroup("PhotoSetup");
	geometry=new QRect(temp_photo_geometry);
	geometry->moveBy( 0, offset );
	geometry->setWidth((int)(geometry->width()*100.0/m_width));// The size of all objects needs to be saved in percentage format
	geometry->setHeight((int) (geometry->height()*100.0/m_width));// The size of all objects needs to be saved in percentage format
	geometries.append( geometry );

	QString photo_html = QString("<img src=\"%1_photos/%2.png\">").arg(filename).arg(recipe.title);
	new_element = new DivElement( "photo_"+QString::number(recipe.recipeID), photo_html );
	new_element->setFixedHeight(true);

	readVisibilityProperties( new_element, config );

	geom_contents.insert( geometry, new_element );
	div_elements.append( new_element );

	//=======================AUTHORS======================//
	config->setGroup("AuthorsSetup");
	geometry = new QRect( config->readRectEntry( "Geometry" ) );
	geometry->moveBy( 0, offset );
	geometries.append( geometry );

	QString authors_html;
	authors_html=QString("<b>%1</b>: ").arg(i18n("Authors"));

	int counter=0;
	for ( ElementList::const_iterator author_it = recipe.authorList.begin(); author_it != recipe.authorList.end(); ++author_it )
	{
		if (counter) authors_html += ", ";
		authors_html += (*author_it).name;
		counter++;
	}
	new_element = new DivElement( "authors_"+QString::number(recipe.recipeID), authors_html );

	readFontProperties( new_element, config );
	readAlignmentProperties( new_element, config );
	readBgColorProperties( new_element, config );
	readTextColorProperties( new_element, config );
	readVisibilityProperties( new_element, config );

	geom_contents.insert( geometry, new_element );
	div_elements.append( new_element );

	//=======================CATEGORIES======================//
	config->setGroup("CategoriesSetup");
	geometry = new QRect( config->readRectEntry( "Geometry" ) );
	geometry->moveBy( 0, offset );
	geometries.append( geometry );

	QString categories_html;
	categories_html=QString("<b>%1: </b>").arg(i18n("Categories"));

	counter=0;
	for ( ElementList::const_iterator cat_it = recipe.categoryList.begin(); cat_it != recipe.categoryList.end(); ++cat_it )
	{
		if (counter) categories_html += ", ";
		categories_html += (*cat_it).name;
		counter++;
	}
	new_element = new DivElement( "categories_"+QString::number(recipe.recipeID), categories_html );

	readFontProperties( new_element, config );
	readAlignmentProperties( new_element, config );
	readBgColorProperties( new_element, config );
	readTextColorProperties( new_element, config );
	readVisibilityProperties( new_element, config );

	geom_contents.insert( geometry, new_element );
	div_elements.append( new_element );

	//=======================ID======================//
	config->setGroup("HeaderSetup");
	geometry = new QRect( config->readRectEntry( "Geometry" ) );
	geometry->moveBy( 0, offset );
	geometries.append( geometry );

	QString header_html = QString("<b>%1 #%2</b>").arg(i18n("Recipe")).arg(recipe.recipeID);
	new_element = new DivElement( "header_"+QString::number(recipe.recipeID), header_html );

	readFontProperties( new_element, config );
	readAlignmentProperties( new_element, config );
	readBgColorProperties( new_element, config );
	readTextColorProperties( new_element, config );
	readVisibilityProperties( new_element, config );

	geom_contents.insert( geometry, new_element );
	div_elements.append( new_element );

	//=======================INGREDIENTS======================//
	config->setGroup("IngredientsSetup");
	geometry = new QRect( config->readRectEntry( "Geometry" ) );
	geometry->moveBy( 0, offset );
	geometries.append( geometry );

	QString ingredients_html;
	config->setGroup("Numbers");

	MixedNumber::Format number_format = (config->readBoolEntry("Fraction")) ? MixedNumber::MixedNumberFormat : MixedNumber::DecimalFormat;

	config->setGroup("IngredientsSetup");
	QString ingredient_format = config->readEntry("Format","%n: %a %u");

	for ( IngredientList::const_iterator ing_it = recipe.ingList.begin(); ing_it != recipe.ingList.end(); ++ing_it )
	{
		QString amount_str = MixedNumber((*ing_it).amount).toString( number_format );

		if (amount_str == "0")
			amount_str = "";

		QString tmp_format(ingredient_format);
		tmp_format.replace(QRegExp(QString::fromLatin1("%n")),(*ing_it).name);
		tmp_format.replace(QRegExp(QString::fromLatin1("%a")),amount_str);
		tmp_format.replace(QRegExp(QString::fromLatin1("%u")),(*ing_it).units);

		ingredients_html += QString("<li>%1</li>").arg(tmp_format);
	}
	new_element = new DivElement( "ingredients_"+QString::number(recipe.recipeID), ingredients_html );

	config->setGroup("IngredientsSetup");
	readFontProperties( new_element, config );
	readAlignmentProperties( new_element, config );
	readBgColorProperties( new_element, config );
	readTextColorProperties( new_element, config );
	readVisibilityProperties( new_element, config );

	geom_contents.insert( geometry, new_element );
	div_elements.append( new_element );

	//=======================PROPERTIES======================//
	config->setGroup("PropertiesSetup");
	geometry = new QRect( config->readRectEntry( "Geometry" ) );
	geometry->moveBy( 0, offset );
	geometries.append( geometry );

	QString properties_html;
	IngredientProperty * prop;
	for ( prop = properties->getFirst(); prop; prop = properties->getNext() )
	{

		// if the amount given is <0, it means the property calculator found that the property was undefined for some ingredients, so the amount will be actually bigger

		QString amount_str;

		if (prop->amount>0) amount_str = QString::number(prop->amount);
		else {
			amount_str = QString::number(-(prop->amount));
			amount_str+="+";
			}

		properties_html += QString("<li>%1: %2  %3</li>")
			    .arg(prop->name)
			    .arg(amount_str)
			    .arg(prop->units);
	}
	new_element = new DivElement( "properties_"+QString::number(recipe.recipeID), properties_html );

	readFontProperties( new_element, config );
	readAlignmentProperties( new_element, config );
	readBgColorProperties( new_element, config );
	readVisibilityProperties( new_element, config );

	geom_contents.insert( geometry, new_element );
	div_elements.append( new_element );

	///////////TODO?: Add an "end of recipe" element here (as a separator between this and the next recipes//////////////

	//this takes expands all items to an appropriate size

	int height_taken = 0;
	geometries.sort(); //we'll work with these in order from top to bottom
	for ( QRect *rect = geometries.first(); rect; rect = geometries.next() )
	{
		DivElement *element = geom_contents.find( rect );

		element->addProperty( "position: absolute;" );


		// For those elements that have no fixed height (lists), calculate the height

		int elementHeight=(int) (rect->height()/100.0*m_width); //Initialize with the current user settings
		int elementWidth=(int) (rect->width()/100.0*m_width);

		if ( !element->fixedHeight() )
		{

			// Generate a test page to calculate the size in khtml
			QString tempHTML="<HTML><HEAD><STYLE type=\"text/css\">";
			tempHTML+= element->generateCSS(true);
			tempHTML+="</STYLE></HEAD>";
			tempHTML+="<BODY>";
			tempHTML+=QString("<DIV id=%1>").arg(element->id());
			tempHTML+= element->innerHTML();
			tempHTML+="</DIV></BODY></HTML>";

			KHTMLPart *sizeCalculator=new KHTMLPart((QWidget*) 0);
			sizeCalculator->view()->setVScrollBarMode (QScrollView::AlwaysOff);
			sizeCalculator->view()->setMinimumSize(QSize(elementWidth,0));
			sizeCalculator->view()->resize(QSize(elementWidth+40,0));
			sizeCalculator->begin(KURL(locateLocal("tmp","/")));
			sizeCalculator->write(tempHTML);
			sizeCalculator->end();


			// Set the size of the element
			int newHeight=sizeCalculator->view()->contentsHeight();
			/* if (newHeight>elementHeight) */ elementHeight=newHeight; // Keep user's size if it's defined as bigger

			delete sizeCalculator;
		}
		rect->setHeight((int)(ceil(elementHeight*100.0/m_width))); // set the new height to the element
									 // Note that ceil is needed to avoid size
									 // shrinking due to float->int conversion

		// Move elements around if there's any overlapping
		pushItemsDownIfNecessary( geometries, rect );

		// Scale the objects to page size
		element->addProperty( QString("top: %1px;").arg(static_cast<int>(rect->top()/100.0*m_width)) );
		element->addProperty( QString("left: %1px;").arg(static_cast<int>(rect->left()/100.0*m_width)) );
		element->addProperty( QString("width: %1px;").arg(static_cast<int>(rect->width()/100.0*m_width)) );
		element->addProperty(QString("height: %1px;").arg(static_cast<int>(rect->height()/100.0*m_width)) );

		height_taken = QMAX(rect->bottom(),height_taken);
	}

	return height_taken;
}


void HTMLExporter::readAlignmentProperties( DivElement *element, KConfig *config )
{
	unsigned int alignment = config->readNumEntry( "Alignment" );

	if ( alignment & Qt::AlignLeft )
		element->addProperty( "text-align: left;" );
	if ( alignment & Qt::AlignRight )
		element->addProperty( "text-align: right;" );
	if ( alignment & Qt::AlignHCenter )
		element->addProperty( "text-align: center;" );
	if ( alignment & Qt::AlignTop )
		element->addProperty( "vertical-align: top;" );
	if ( alignment & Qt::AlignBottom )
		element->addProperty( "vertical-align: bottom;" );
	if ( alignment & Qt::AlignVCenter )
		element->addProperty( "vertical-align: middle;" );
}

void HTMLExporter::readBgColorProperties( DivElement *element, KConfig *config )
{
	QColor color = config->readColorEntry( "BackgroundColor" );
	element->addProperty( QString("background-color: %1;").arg(color.name()) );
}

void HTMLExporter::readFontProperties( DivElement *element, KConfig *config )
{
	QFont font = config->readFontEntry( "Font" );
	element->addProperty( QString("font-family: %1;").arg(font.family()) );
	element->addProperty( QString("font-size: %1pt;").arg(font.pointSize()) );
	element->addProperty( QString("font-weight: %1;").arg(font.weight()) );
}

void HTMLExporter::readTextColorProperties( DivElement *element, KConfig *config )
{
	QColor color = config->readColorEntry( "TextColor" );
	element->addProperty( QString("color: %1;").arg(color.name()) );
}

void HTMLExporter::readVisibilityProperties( DivElement *element, KConfig *config )
{
	bool shown = config->readBoolEntry( "Visibility" );
	if ( shown )
		element->addProperty( "visibility: visible;" );
	else
		element->addProperty( "visibility: hidden;" );
}

void HTMLExporter::pushItemsDownIfNecessary( QPtrList<QRect> &geometries, QRect *top_geom )
{
	for ( QRect *item = geometries.next(); item; item = geometries.next() )
	{
		int height_offset = 0;

		QRect intersection = item->intersect( *top_geom );

		if ( intersection.isValid() )
		{
			height_offset = QABS( intersection.top() - top_geom->bottom() );
			item->moveBy( 0, height_offset+5 );
		}
	}

	geometries.findRef( top_geom ); //set it back to where is was
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
	QFile old_file(filename+".html");
	if ( old_file.exists() )
		old_file.remove();

	//remove photos
	for ( QStringList::const_iterator it = recipe_titles.begin(); it != recipe_titles.end(); ++it )
	{
		QFile photo( filename+"_photos/"+*it+".png");
		if ( photo.exists() )
			photo.remove(); //remove photos in directory before removing it (there should only be one photo in this directory)
	}

	//remove photo directory
	QDir photo_dir;
	photo_dir.rmdir( filename+"_photos" );
}


/////////////////////////   CustomRectList   //////////////////////////////////

CustomRectList::CustomRectList() : QPtrList<QRect>()
{
}

int CustomRectList::compareItems( QPtrCollection::Item item1, QPtrCollection::Item item2 )
{
	QRect *geom1 = static_cast<QRect*>(item1);
	QRect *geom2 = static_cast<QRect*>(item2);

	if ( geom1->y() > geom2->y() )
		return 1;
	else if ( geom1->y() < geom2->y() )
		return -1;
	else
		return 0;
}


/////////////////////////   DivElement   //////////////////////////////////

DivElement::DivElement( const QString &id, const QString &content ) :
  m_id(id),
  m_content(content),
  m_fixed_height(false)
{
}

QFont DivElement::font()
{
	QFont f; //constructs the default font

	for ( QStringList::Iterator it = m_properties.begin(); it != m_properties.end(); ++it )
	{
		if ( (*it).contains("font-family") )
			f.setFamily((*it).mid( (*it).find(':')+1, (*it).length()-1 ).stripWhiteSpace());
		else if ( (*it).contains("font-size") )
		{
			int colon_index = (*it).find(':');
			f.setPointSize((*it).mid( colon_index+1, (*it).find('p') - colon_index-1 ).toInt());
		}
		//else if ( (*it).contains("font-weight") )
		//else if ( (*it).contains("font-style") )
	}

	return f;
}

QString DivElement::generateHTML()
{
	QString result;

	result += QString("<DIV id=\"%1\">\n").arg(m_id);
	result += m_content + "\n";
	result += "</DIV>\n";

	return result;
}

QString DivElement::generateCSS(bool noPositioning)
{
	QString result;

	result += QString("#%1\n").arg(m_id);
	result += "{\n";

	for ( QStringList::Iterator it = m_properties.begin(); it != m_properties.end(); ++it )
	if (!noPositioning)
		{
        	result += *it + "\n";
		}
	else // Don't use the element positions
		{
		if (!((*it).contains("top:")||((*it).contains("left:"))))
			{
			result += *it + "\n";
			}
		}

	//don't show empty blocks
	if ( m_content == "" || m_content.isNull() )
		result += "visibility: hidden;\n";

	result += "}\n";

	return result;
}
