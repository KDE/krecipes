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
		return "<HTML></HTML>";

	KConfig *config = KGlobal::config();
	config->setGroup( "Page Setup" );
	kdDebug()<<"Using layout file: "<<config->readEntry("Layout",locate("appdata","layouts/default.klo"))<<endl;
	QFile input( config->readEntry("Layout",locate("appdata","layouts/default.klo")) );
	
	QDomDocument doc;
	
	if ( !input.open( IO_ReadOnly ) )
		return "<HTML></HTML>";
	
	QString error; int line; int column;
	if (!doc.setContent(&input,&error,&line,&column))
	{
		kdDebug()<<QString( i18n("\"%1\" at line %2, column %3.  This may not be a Krecipes layout file or it has become corrupt.") ).arg(error).arg(line).arg(column)<<endl;
		return "<HTML></HTML>";
	}

	//put all the recipe photos into this directory
	QDir dir;
	QFileInfo fi( *file );
	dir.mkdir( fi.dirPath()+"/"+filename+"_photos" );

	RecipeList::const_iterator recipe_it;
	QString recipeTitleHTML = QString("<TITLE>%1</TITLE>").arg( (recipes.count() == 1) ? recipes[0].title : i18n("Krecipes Recipes") );
	int offset = 0;

	QDomElement bg_element = getLayoutAttribute( doc, "background", "background-color" );
	QString recipeStyleHTML = "<STYLE type=\"text/css\">\n";
	recipeStyleHTML += "BODY\n";
	recipeStyleHTML += "{\n";
	recipeStyleHTML += QString("background-color: %1;\n").arg( bg_element.text() );
	recipeStyleHTML += "}\n";

	QString recipeBodyHTML = "<BODY>";
	for ( recipe_it = recipes.begin(); recipe_it != recipes.end(); ++recipe_it )
	{
		calculateProperties( *recipe_it, database, properties ); // Calculate the property list
		storePhoto( *recipe_it, doc );
		offset = createBlocks( *recipe_it, doc, offset ) + 15;

		for ( DivElement *div = div_elements.first(); div; div = div_elements.next() )
		{
			recipeStyleHTML += div->generateCSS();
			recipeBodyHTML += div->generateHTML();
		}

		if ( progressBarCanceled() ) return QString::null; //FIXME: should we just return what we've generated so far?  It does simplify things elsewhere... (all we have to do is put "break;" here and anything works out; it will return the complete recipes generated so far)
		advanceProgressBar();
	}
	recipeStyleHTML += "</STYLE>";
	recipeBodyHTML += "</BODY>";


	//and now piece it all together
	QString recipeHTML = "<HTML><HEAD>";
	recipeHTML += recipeStyleHTML;
	recipeHTML += "</HEAD>\n";
	recipeHTML += recipeBodyHTML;
	recipeHTML += "</HTML>";

	return recipeHTML;
}

void HTMLExporter::storePhoto( const Recipe &recipe, const QDomDocument &doc )
{
	QDomElement photo_geom_el = getLayoutAttribute( doc, "photo", "geometry" );
	temp_photo_geometry = QRect( 	photo_geom_el.attribute("left").toInt(),
					photo_geom_el.attribute("top").toInt(),
					photo_geom_el.attribute("width").toInt(),
					photo_geom_el.attribute("height").toInt()
				);

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

int HTMLExporter::createBlocks( const Recipe &recipe, const QDomDocument &doc, int offset )
{
	div_elements.clear();

	QRect *geometry;
	DivElement *new_element;

	CustomRectList geometries;
	geometries.setAutoDelete(true);
	QPtrDict<DivElement> geom_contents;

	KConfig *config = KGlobal::config();

	//=======================TITLE======================//
	geometry = new QRect; readGeometry( geometry, doc, "title" );
	geometry->moveBy( 0, offset );
	geometries.append( geometry );

	new_element = new DivElement( "title_"+QString::number(recipe.recipeID), recipe.title);

	readFontProperties( new_element, doc, "title" );
	readAlignmentProperties( new_element, doc, "title" );
	readBgColorProperties( new_element, doc, "title" );
	readTextColorProperties( new_element, doc, "title" );
	readVisibilityProperties( new_element, doc, "title" );

	geom_contents.insert( geometry, new_element );
	div_elements.append( new_element );

	//=======================INSTRUCTIONS======================//
	geometry = new QRect; readGeometry( geometry, doc, "instructions" );
	geometry->moveBy( 0, offset );
	geometries.append( geometry );

	QString instr_html = recipe.instructions;
	instr_html.replace("\n","<BR>");
	new_element = new DivElement( "instructions_"+QString::number(recipe.recipeID), instr_html );

	readFontProperties( new_element, doc, "instructions" );
	readAlignmentProperties( new_element, doc, "instructions" );
	readBgColorProperties( new_element, doc, "instructions" );
	readTextColorProperties( new_element, doc, "instructions" );
	readVisibilityProperties( new_element, doc, "instructions" );

	geom_contents.insert( geometry, new_element );
	div_elements.append( new_element );

	//=======================SERVINGS======================//
	geometry = new QRect; readGeometry( geometry, doc, "servings" );
	geometry->moveBy( 0, offset );
	geometries.append( geometry );

	QString servings_html = QString("<b>%1: </b>%2").arg(i18n("Servings")).arg(recipe.persons);
	new_element = new DivElement( "servings_"+QString::number(recipe.recipeID), servings_html );

	readFontProperties( new_element, doc, "servings" );
	readAlignmentProperties( new_element, doc, "servings" );
	readBgColorProperties( new_element, doc, "servings" );
	readTextColorProperties( new_element, doc, "servings" );
	readVisibilityProperties( new_element, doc, "servings" );

	geom_contents.insert( geometry, new_element );
	div_elements.append( new_element );

	//========================PHOTO========================//
	geometry=new QRect(temp_photo_geometry);
	geometry->setWidth((int)(double(geometry->width())*100.0/m_width));// The size of all objects needs to be saved in percentage format
	geometry->setHeight((int) (double(geometry->height())*100.0/m_width));// The size of all objects needs to be saved in percentage format
	geometry->moveBy( 0, offset );
	geometries.append( geometry );

	QString photo_html = QString("<img src=\"%1_photos/%2.png\">").arg(filename).arg(recipe.title);
	new_element = new DivElement( "photo_"+QString::number(recipe.recipeID), photo_html );
	new_element->setFixedHeight(true);

	readVisibilityProperties( new_element, doc, "photo" );

	geom_contents.insert( geometry, new_element );
	div_elements.append( new_element );

	//=======================AUTHORS======================//
	geometry = new QRect; readGeometry( geometry, doc, "authors" );
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

	readFontProperties( new_element, doc, "authors" );
	readAlignmentProperties( new_element, doc, "authors" );
	readBgColorProperties( new_element, doc, "authors" );
	readTextColorProperties( new_element, doc, "authors" );
	readVisibilityProperties( new_element, doc, "authors" );

	geom_contents.insert( geometry, new_element );
	div_elements.append( new_element );

	//=======================CATEGORIES======================//
	geometry = new QRect; readGeometry( geometry, doc, "categories" );
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

	readFontProperties( new_element, doc, "categories" );
	readAlignmentProperties( new_element, doc, "categories" );
	readBgColorProperties( new_element, doc, "categories" );
	readTextColorProperties( new_element, doc, "categories" );
	readVisibilityProperties( new_element, doc, "categories" );

	geom_contents.insert( geometry, new_element );
	div_elements.append( new_element );

	//=======================HEADER======================//
	geometry = new QRect; readGeometry( geometry, doc, "header" );
	geometry->moveBy( 0, offset );
	geometries.append( geometry );

	QString header_html = QString("<b>%1 #%2</b>").arg(i18n("Recipe")).arg(recipe.recipeID);
	new_element = new DivElement( "header_"+QString::number(recipe.recipeID), header_html );

	readFontProperties( new_element, doc, "header" );
	readAlignmentProperties( new_element, doc, "header" );
	readBgColorProperties( new_element, doc, "header" );
	readTextColorProperties( new_element, doc, "header" );
	readVisibilityProperties( new_element, doc, "header" );

	geom_contents.insert( geometry, new_element );
	div_elements.append( new_element );

	//=======================INGREDIENTS======================//
	geometry = new QRect; readGeometry( geometry, doc, "ingredients" );
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

	readFontProperties( new_element, doc, "ingredients" );
	readAlignmentProperties( new_element, doc, "ingredients" );
	readBgColorProperties( new_element, doc, "ingredients" );
	readTextColorProperties( new_element, doc, "ingredients" );
	readVisibilityProperties( new_element, doc, "ingredients" );

	geom_contents.insert( geometry, new_element );
	div_elements.append( new_element );

	//=======================PROPERTIES======================//
	geometry = new QRect; readGeometry( geometry, doc, "properties" );
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

	readFontProperties( new_element, doc, "properties" );
	readAlignmentProperties( new_element, doc, "properties" );
	readBgColorProperties( new_element, doc, "properties" );
	readVisibilityProperties( new_element, doc, "properties" );

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
			tempHTML+=element->generateHTML();
			tempHTML+="</BODY></HTML>";

			KHTMLPart *sizeCalculator=new KHTMLPart((QWidget*) 0);
			sizeCalculator->view()->setVScrollBarMode (QScrollView::AlwaysOff);
			sizeCalculator->view()->setMinimumSize(QSize(elementWidth+40,0));
			sizeCalculator->view()->resize(QSize(elementWidth,0));
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

void HTMLExporter::readGeometry( QRect *geom, const QDomDocument &doc, const QString &object )
{
	QDomElement geom_el = getLayoutAttribute( doc, object, "geometry" );

	if ( !geom_el.isNull() )
	{
		geom->setLeft( geom_el.attribute("left").toInt() );
		geom->setTop( geom_el.attribute("top").toInt() );
		geom->setWidth( geom_el.attribute("width").toInt() );
		geom->setHeight( geom_el.attribute("height").toInt() );
	}
}

void HTMLExporter::readAlignmentProperties( DivElement *element, const QDomDocument &doc, const QString &object )
{
	QDomElement el = getLayoutAttribute( doc, object, "alignment" );
	
	if ( !el.isNull() )
	{
		unsigned int alignment = el.text().toInt();

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
}

void HTMLExporter::readBgColorProperties( DivElement *element, const QDomDocument &doc, const QString &object )
{
	QDomElement el = getLayoutAttribute( doc, object, "background-color" );

	if ( !el.isNull() )
		element->addProperty( QString("background-color: %1;").arg(el.text()) );
}

void HTMLExporter::readFontProperties( DivElement *element, const QDomDocument &doc, const QString &object )
{
	QDomElement el = getLayoutAttribute( doc, object, "font" );
	
	if ( !el.isNull() )
	{
		QFont font;
		font.fromString( el.text() );

		element->addProperty( QString("font-family: %1;").arg(font.family()) );
		element->addProperty( QString("font-size: %1pt;").arg(font.pointSize()) );
		element->addProperty( QString("font-weight: %1;").arg(font.weight()) );
	}
}

void HTMLExporter::readTextColorProperties( DivElement *element, const QDomDocument &doc, const QString &object )
{
	QDomElement el = getLayoutAttribute( doc, object, "text-color" );

	if ( !el.isNull() )
		element->addProperty( QString("color: %1;").arg(el.text()) );
}

void HTMLExporter::readVisibilityProperties( DivElement *element, const QDomDocument &doc, const QString &object )
{
	QDomElement el = getLayoutAttribute( doc, object, "visible" );
	
	if ( !el.isNull() )
	{
		bool shown = (el.text() == "false") ? false : true;
		if ( shown )
			element->addProperty( "visibility: visible;" );
		else
			element->addProperty( "visibility: hidden;" );
	}
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

QDomElement HTMLExporter::getLayoutAttribute( const QDomDocument &doc, const QString &object, const QString &attribute )
{
	QDomNodeList node_list = doc.elementsByTagName( object );
	if ( node_list.count() == 0 )
	{
		kdDebug()<<"Warning: Requested object \""<<object<<"\" not found."<<endl;
		return QDomElement();
	}
		
	QDomElement object_element = node_list.item(0).toElement(); //there should only be one, so we'll just take the first
	
	QDomNodeList l = object_element.childNodes();
	for (unsigned i = 0; i < l.count(); i++)
	{
		QDomElement el = l.item(i).toElement();
		
		if ( el.tagName() == attribute )
			return el;
	}
	
	kdDebug()<<"Warning: Requested attribute \""<<attribute<<"\" not found."<<endl;
	return QDomElement();
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
	if ( m_content.isEmpty() )
		result += "visibility: hidden;\n";

	result += "}\n";

	return result;
}
