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

#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kapplication.h>
#include <khtml_part.h>
#include <khtmlview.h>

#include "propertycalculator.h"
#include "mixednumber.h"
#include "gui/setupdisplay.h"
#include "image.h"

HTMLExporter::HTMLExporter( RecipeDB *db, const QString& filename, const QString format, int width ) :
  BaseExporter( db, filename, format ), database(db), m_width(width)
{
	div_elements.setAutoDelete(true);
	properties = new IngredientPropertyList;
}


HTMLExporter::~HTMLExporter()
{
	delete properties;
}

QString HTMLExporter::createContent( const QPtrList<Recipe>& recipes )
{
	if ( recipes.count() == 0 )
		return "<html></html>";

	KConfig *config = kapp->config();
	QString recipeHTML;

	//Creates initial layout and saves to config file
	SetupDisplay::createSetupIfNecessary();

	QPtrListIterator<Recipe> recipes_it( recipes );
	Recipe *recipe; bool first = true;
	while ( (recipe = recipes_it.current()) != 0 )
	{
		++recipes_it;

		// Calculate the property list
		calculateProperties(*recipe,database,properties);
		createBlocks( *recipe );

		if ( first )
		{
			storePhoto( *recipe );

			recipeHTML += "<html><head>";

			// title (not shown)
			recipeHTML += QString("<title>%1</title>").arg( (recipes.count() == 1) ? recipe->title : i18n("Krecipes Recipes") );

			//=========================STYLE=======================//
			recipeHTML += "<STYLE type=\"text/css\">\n";

			config->setGroup("BackgroundSetup");
			QColor color = config->readColorEntry( "BackgroundColor" );
			recipeHTML += "BODY\n";
			recipeHTML += "{\n";
			recipeHTML += QString("background-color: %1;\n").arg(color.name());
			recipeHTML += "}\n";

				for ( DivElement *div = div_elements.first(); div; div = div_elements.next() )
				recipeHTML += div->generateCSS();
			recipeHTML += "</STYLE>\n";
			//=======================END-STYLE=====================//

			recipeHTML += "</head><body>";

			first = false;
		}

		for ( DivElement *div = div_elements.first(); div; div = div_elements.next() )
			recipeHTML += div->generateHTML();
	}

	// Close HTML
	recipeHTML+="</body></html>\n";

	kdDebug()<<recipeHTML<<endl;

	return recipeHTML;
}

void HTMLExporter::storePhoto( const Recipe &recipe )
{
	KConfig *config = kapp->config();
	config->setGroup("PhotoSetup");
	temp_photo_geometry = config->readRectEntry("Geometry");

	int phwidth = temp_photo_geometry.width()/100.0*m_width; // Scale to this dialog
	int phheight =temp_photo_geometry.height()/100.0*m_width; // Scale to this dialog

	QImage image;
	if (recipe.photo.isNull())
		image = QImage(defaultPhoto);
	else
		image = recipe.photo.convertToImage();

	QPixmap pm = image.smoothScale(phwidth, phheight, QImage::ScaleMin);
	pm.save("/tmp/krecipes_photo.png","PNG");
	temp_photo_geometry = QRect(temp_photo_geometry.topLeft(),pm.size()); //preserve aspect ratio
}

void HTMLExporter::createBlocks( const Recipe &recipe )
{
	div_elements.clear();
	QFont default_font;

	QRect *geometry;
	DivElement *new_element;

	CustomRectList geometries;
	geometries.setAutoDelete(true);
	QPtrDict<DivElement> geom_contents;

	KConfig *config = kapp->config();

	//=======================TITLE======================//
	config->setGroup("TitleSetup");
	geometry = new QRect( config->readRectEntry( "Geometry" ) );
	geometries.append( geometry );

	new_element = new DivElement( "title",recipe.title);

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
	geometries.append( geometry );

	QString instr_html = recipe.instructions;
	instr_html.replace("\n","<BR>");
	new_element = new DivElement( "instructions", instr_html );

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
	geometries.append( geometry );

	QString servings_html = QString("<b>%1: </b>%2").arg(i18n("Servings")).arg(recipe.persons);
	new_element = new DivElement( "servings", servings_html );

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
	geometry->setWidth(geometry->width()*100.0/m_width);// The size of all objects needs to be saved in percentage format
	geometry->setHeight(geometry->height()*100.0/m_width);// The size of all objects needs to be saved in percentage format
	geometries.append( geometry );

	QString photo_html = QString("<img src=\"/tmp/krecipes_photo.png\">");
	new_element = new DivElement( "photo", photo_html );
	new_element->setFixedHeight(true);

	readVisibilityProperties( new_element, config );

	geom_contents.insert( geometry, new_element );
	div_elements.append( new_element );

	//=======================AUTHORS======================//
	config->setGroup("AuthorsSetup");
	geometry = new QRect( config->readRectEntry( "Geometry" ) );
	geometries.append( geometry );

	QString authors_html;
	authors_html=QString("<b>%1</b>: ").arg(i18n("Authors"));

	QPtrListIterator<Element> author_it( recipe.authorList );
	Element *author_el;
	int counter=0;
	while ( (author_el = author_it.current()) != 0 )
	{
        	++author_it;

		if (counter) authors_html += ", ";
		authors_html += author_el->name;
		counter++;
	}
	new_element = new DivElement( "authors", authors_html );

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
	geometries.append( geometry );

	QString categories_html;
	categories_html=QString("<b>%1: </b>").arg(i18n("Categories"));

	QPtrListIterator<Element> cat_it( recipe.categoryList );

	Element *cat_el;
	counter=0;
	while ( (cat_el = cat_it.current()) != 0 )
	{
        	++cat_it;

		if (counter) categories_html += ", ";
		categories_html += cat_el->name;
		counter++;
	}
	new_element = new DivElement( "categories", categories_html );

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
	geometries.append( geometry );

	QString header_html = QString("<b>%1 #%2</b>").arg(i18n("Recipe")).arg(recipe.recipeID);
	new_element = new DivElement( "header", header_html );

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
	geometries.append( geometry );

	QString ingredients_html;
	config->setGroup("Numbers");

	MixedNumber::Format number_format = (config->readBoolEntry("Fraction")) ? MixedNumber::MixedNumberFormat : MixedNumber::DecimalFormat;

	config->setGroup("IngredientsSetup");
	QString ingredient_format = config->readEntry("Format","%n: %a %u");

	QPtrListIterator<Ingredient> ing_it( recipe.ingList );
	Ingredient *ing;
	while ( (ing = ing_it.current()) != 0 )
	{
        	++ing_it;

		QString amount_str = MixedNumber(ing->amount).toString( number_format );

		if (amount_str == "0")
			amount_str = "";

		QString tmp_format(ingredient_format);
		tmp_format.replace(QRegExp(QString::fromLatin1("%n")),ing->name);
		tmp_format.replace(QRegExp(QString::fromLatin1("%a")),amount_str);
		tmp_format.replace(QRegExp(QString::fromLatin1("%u")),ing->units);

		ingredients_html += QString("<li>%1</li>").arg(tmp_format);
	}
	new_element = new DivElement( "ingredients", ingredients_html );

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
	new_element = new DivElement( "properties", properties_html );

	readFontProperties( new_element, config );
	readAlignmentProperties( new_element, config );
	readBgColorProperties( new_element, config );
	readVisibilityProperties( new_element, config );

	geom_contents.insert( geometry, new_element );
	div_elements.append( new_element );

	//this takes expands all items to an appropriate size

	geometries.sort(); //we'll work with these in order from top to bottom
	for ( QRect *rect = geometries.first(); rect; rect = geometries.next() )
	{
		DivElement *element = geom_contents.find( rect );

		element->addProperty( "position: absolute;" );


		// For those elements that have no fixed height (lists), calculate the height

		int elementHeight=rect->height()/100.0*m_width; //Initialize with the current user settings
		int elementWidth=rect->width()/100.0*m_width;

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
			sizeCalculator->begin(KURL("file:/tmp/" ));
			sizeCalculator->write(tempHTML);
			sizeCalculator->end();


			// Set the size of the element
			int newHeight=sizeCalculator->view()->contentsHeight();
			if (newHeight>elementHeight) elementHeight=newHeight; // Keep user's size if it's defined as bigger

			delete sizeCalculator;
		}
		rect->setHeight(ceil(elementHeight*100.0/m_width)); // set the new height to the element
									 // Note that ceil is needed to avoid size
									 // shrinking due to float->int conversion

		// Move elements around if there's any overlapping

		pushItemsDownIfNecessary( geometries, rect );

		// Scale the objects to page size

		element->addProperty( QString("top: %1px;").arg(rect->top()/100.0*m_width) );
		element->addProperty( QString("left: %1px;").arg(rect->left()/100.0*m_width) );
		element->addProperty( QString("width: %1px;").arg(rect->width()/100.0*m_width) );
		element->addProperty(QString("height: %1px;").arg(rect->height()/100.0*m_width));
	}
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
