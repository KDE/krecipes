/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "recipeviewdialog.h"

#include "mixednumber.h"
#include "image.h"
#include "propertycalculator.h"
#include "gui/setupdisplay.h"

#include <klocale.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>

#include <qimage.h>
#include <qsimplerichtext.h>
#include <qptrdict.h>
#include <qfont.h>

#include "DBBackend/recipedb.h"

RecipeViewDialog::RecipeViewDialog(QWidget *parent, RecipeDB *db, int recipeID):QVBox(parent)
{

// Initialize UI Elements
recipeView=new KHTMLPart(this);

// Store/Initialize local variables
database=db; // Store the database pointer.
loadedRecipe=new Recipe();
properties=new IngredientPropertyList;

div_elements.setAutoDelete(true);

//----------Load  the recipe --------
loadRecipe(recipeID);

//this->calculateProperties();


 }

RecipeViewDialog::~RecipeViewDialog()
{
	delete properties;
}

void RecipeViewDialog::loadRecipe(int recipeID)
{

// Load specified Recipe ID
database->loadRecipe(loadedRecipe,recipeID);

// Calculate the property list
calculateProperties(*loadedRecipe,database,properties);

//Creates initial layout and saves to config file
SetupDisplay::createSetupIfNecessary();

// Store Photo
KConfig *config = kapp->config();
config->setGroup("PhotoSetup");
temp_photo_geometry = config->readRectEntry("Geometry");
int width = temp_photo_geometry.width();
int height = temp_photo_geometry.height();

QImage image;
if (loadedRecipe->photo.isNull())
	image = QImage(defaultPhoto);
else
	image = loadedRecipe->photo.convertToImage();

QPixmap pm = image.smoothScale(width, height, QImage::ScaleMin);
pm.save("/tmp/krecipes_photo.png","PNG");
temp_photo_geometry = QRect(QPoint(temp_photo_geometry.topLeft()),pm.size()); //preserve aspect ratio

// Loads the layout according to the config file
createBlocks();

// Display the recipe
showRecipe();
}

void RecipeViewDialog::showRecipe(void)
{
QString recipeHTML;

// Create HTML Code
if (loadedRecipe->recipeID<0)
{
// Show default (empty) recipe
recipeHTML=QString("<html><head><title>%1</title></head>").arg(i18n("Title of the Recipe"));
recipeHTML+=QString("<div STYLE=\"position: absolute; top: 30px; left:1%; width: 22%\"> <li>%1</li>").arg(i18n("Ingredient 1"));
recipeHTML+=QString("<li>%1</li> <li>%2</li> </div>").arg(i18n("Ingredient 2")).arg(i18n("Ingredient 3"));
recipeHTML+="<div STYLE=\"position: absolute; top: 30px; left:25%; width: 74%\">";
recipeHTML+=QString("<center><h1>%1</h1></center>").arg(i18n("Title of the Recipe"));
recipeHTML+=QString("<p>%1 </p></div></body></html>").arg(i18n("Recipe Instructions"));

}
else
{
// Format the loaded recipe as HTML code

recipeHTML += "<html><head>";

// title (not shown)
recipeHTML += QString("<title>%1</title>").arg( loadedRecipe->title);

recipeHTML += "<STYLE type=\"text/css\">\n";

KConfig *config = kapp->config();
config->setGroup("BackgroundSetup");
QColor color = config->readColorEntry( "BackgroundColor" );
recipeHTML += "BODY\n";
recipeHTML += "{\n";
recipeHTML += QString("background-color: %1;\n").arg(color.name());
recipeHTML += "}\n";

for ( DivElement *div = div_elements.first(); div; div = div_elements.next() )
	recipeHTML += div->generateCSS();
recipeHTML += "</STYLE>\n";

recipeHTML += "</head><body>";

for ( DivElement *div = div_elements.first(); div; div = div_elements.next() )
	recipeHTML += div->generateHTML();

// Close HTML
recipeHTML+="</body></html>\n";
}

kdDebug()<<recipeHTML<<endl;

delete recipeView;              // Temporary workaround
recipeView=new KHTMLPart(this); // to avoid the problem of caching images of KHTMLPart

recipeView->begin(KURL("file:/tmp/" )); // Initialize to /tmp, where the photo was stored
recipeView->write(recipeHTML);
recipeView->end();
}

void RecipeViewDialog::print(void)
{
	if ( recipeView && loadedRecipe->recipeID >= 0 )
		recipeView->view()->print();
}

void RecipeViewDialog::createBlocks()
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

	new_element = new DivElement( "title", loadedRecipe->title );

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

	QString instr_html = loadedRecipe->instructions;
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

	QString servings_html = QString(i18n("Servings: %1")).arg(loadedRecipe->persons);
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
	geometry = new QRect(temp_photo_geometry);
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
	QPtrListIterator<Element> author_it( loadedRecipe->authorList );
	Element *author_el;
	while ( (author_el = author_it.current()) != 0 )
	{
        	++author_it;

		if (authors_html != QString::null) authors_html += ", ";
		authors_html += author_el->name;
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
	QPtrListIterator<Element> cat_it( loadedRecipe->categoryList );
	Element *cat_el;
	while ( (cat_el = cat_it.current()) != 0 )
	{
        	++cat_it;

		if (categories_html != QString::null) categories_html += ", ";
		categories_html += cat_el->name;
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

	QString header_html = QString(i18n("Recipe: #%1")).arg(loadedRecipe->recipeID);
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
	config->setGroup("Units");

	MixedNumber::Format number_format = (config->readBoolEntry("Fraction")) ? MixedNumber::MixedNumberFormat : MixedNumber::DecimalFormat;

	config->setGroup("IngredientsSetup");
	QString ingredient_format = config->readEntry("Format","%n: %a %u");

	Ingredient * ing;
	for ( ing = loadedRecipe->ingList.getFirst(); ing; ing = loadedRecipe->ingList.getNext() )
	{
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

	//this takes expands all items to an appropriate size, and ensures no widgets
	//overlap in the process
	geometries.sort(); //we'll work with these in order from top to bottom
	for ( QRect *rect = geometries.first(); rect; rect = geometries.next() )
	{
		DivElement *element = geom_contents.find( rect );

		if ( !element->fixedHeight() )
		{
			QString text = element->innerHTML();
			QSimpleRichText rt( text, element->font(), "file:/tmp/" );
			rt.setWidth( rect->width() );
			rect->setHeight( rt.height() );
		}

		pushItemsDownIfNecessary( geometries, rect );

		element->addProperty( "position: absolute;" );
		element->addProperty( QString("top: %1px;").arg(rect->top()) );
		element->addProperty( QString("left: %1px;").arg(rect->left()) );
		element->addProperty( QString("width: %1px;").arg(rect->width()) );
		element->addProperty( QString("height: %1px;").arg(rect->height()) );
	}
}

void RecipeViewDialog::readAlignmentProperties( DivElement *element, KConfig *config )
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

void RecipeViewDialog::readBgColorProperties( DivElement *element, KConfig *config )
{
	QColor color = config->readColorEntry( "BackgroundColor" );
	element->addProperty( QString("background-color: %1;").arg(color.name()) );
}

void RecipeViewDialog::readFontProperties( DivElement *element, KConfig *config )
{
	QFont font = config->readFontEntry( "Font" );
	element->addProperty( QString("font-family: %1;").arg(font.family()) );
	element->addProperty( QString("font-size: %1pt;").arg(font.pointSize()) );
	element->addProperty( QString("font-weight: %1;").arg(font.weight()) );
}

void RecipeViewDialog::readTextColorProperties( DivElement *element, KConfig *config )
{
	QColor color = config->readColorEntry( "TextColor" );
	element->addProperty( QString("color: %1;").arg(color.name()) );
}

void RecipeViewDialog::readVisibilityProperties( DivElement *element, KConfig *config )
{
	bool shown = config->readBoolEntry( "Visibility" );

	if ( shown )
		element->addProperty( "visibility: visible;" );
	else
		element->addProperty( "visibility: hidden;" );
}

void RecipeViewDialog::pushItemsDownIfNecessary( QPtrList<QRect> &geometries, QRect *top_geom )
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

QString DivElement::generateCSS()
{
	QString result;

	result += QString("DIV#%1\n").arg(m_id);
	result += "{\n";

	for ( QStringList::Iterator it = m_properties.begin(); it != m_properties.end(); ++it )
        	result += *it + "\n";

	//don't show empty blocks
	if ( m_content == "" || m_content.isNull() )
		result += "visibility: hidden;\n";

	result += "}\n";

	return result;
}
