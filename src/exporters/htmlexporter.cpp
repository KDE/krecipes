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

#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <kprogress.h>
#include <kstandarddirs.h>
#include <kurl.h>

#include "propertycalculator.h"
#include "mixednumber.h"
#include "gui/setupdisplay.h"
#include "image.h"

#include <cmath> //for ceil()

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
	
	//let's do everything we can to be sure at least some layout is loaded
	QString layout_filename = config->readEntry("Layout",locate("appdata","layouts/default.klo"));
	if ( layout_filename.isEmpty() || !QFile::exists(layout_filename) )
		layout_filename = locate("appdata","layouts/default.klo");
	kdDebug()<<"Using layout file: "<<layout_filename<<endl;
	QFile input( layout_filename );
	
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
	
	classesCSS = generateCSSClasses( doc );
	recipeStyleHTML += classesCSS;

	QString recipeBodyHTML = "<BODY>";
	for ( recipe_it = recipes.begin(); recipe_it != recipes.end(); ++recipe_it )
	{
		calculateProperties( *recipe_it, database, properties ); // Calculate the property list
		storePhoto( *recipe_it, doc );
		offset = createBlocks( *recipe_it, doc, offset ) + 10;

		for ( DivElement *div = div_elements.first(); div; div = div_elements.next() )
		{
			recipeStyleHTML += div->generateCSS();
			recipeBodyHTML += div->generateHTML();
		}

		if ( progressBarCancelled() ) return QString::null; //FIXME: should we just return what we've generated so far?  It does simplify things elsewhere... (all we have to do is put "break;" here and anything works out; it will return the complete recipes generated so far)
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
	temp_photo_geometry = QRect(    qRound(photo_geom_el.attribute("left").toDouble()),
					qRound(photo_geom_el.attribute("top").toDouble()),
					qRound(photo_geom_el.attribute("width").toDouble()),
					qRound(photo_geom_el.attribute("height").toDouble())
				);

	int phwidth = (int) (temp_photo_geometry.width()/100.0*m_width); // Scale to this dialog
	int phheight =(int) (temp_photo_geometry.height()/100.0*m_width); // Scale to this dialog

	QImage image;
	if (recipe.photo.isNull())
		image = QImage(defaultPhoto);
	else
		image = recipe.photo.convertToImage();

	QPixmap pm;
	if ( phwidth > phheight )
		pm = image.smoothScale(phwidth, phheight, QImage::ScaleMax);
	else
		pm = image.smoothScale(phwidth, phheight, QImage::ScaleMin);
	//QPixmap pm = image.scaleWidth(phwidth);
	QFileInfo fi(*file);
	pm.save(fi.dirPath()+"/"+filename+"_photos/"+escape(recipe.title)+".png","PNG");
	temp_photo_geometry = QRect(temp_photo_geometry.topLeft(),pm.size()); //preserve aspect ratio
}

int HTMLExporter::createBlocks( const Recipe &recipe, const QDomDocument &doc, int offset )
{
	const QMap<QString,QString> html_map = generateBlocksHTML( recipe );

	QRect *geometry;
	DivElement *new_element;

	CustomRectList geometries;
	geometries.setAutoDelete(true);
	QPtrDict<DivElement> geom_contents;

	for ( QMap<QString,QString>::const_iterator it = html_map.begin(); it != html_map.end(); ++it )
	{
		QString key = it.key();
				
		new_element = new DivElement( key+"_"+QString::number(recipe.recipeID), key, it.data() );

		if ( key == "photo" )
		{
			temp_photo_geometry.setWidth((int)(double(temp_photo_geometry.width())*100.0/m_width));// The size of all objects needs to be saved in percentage format
			temp_photo_geometry.setHeight((int) (double(temp_photo_geometry.height())*100.0/m_width));// The size of all objects needs to be saved in percentage format

			geometry = new QRect(temp_photo_geometry);
			geometry->moveBy( 0, offset );
			geometries.append( geometry );
			
			new_element->setFixedHeight(true);
		}
		else
		{
			geometry = new QRect; readGeometry( geometry, doc, key );
			geometry->moveBy( 0, offset );
			geometries.append( geometry );
		}
		
		geom_contents.insert( geometry, new_element );
		div_elements.append( new_element );
	}

	//this takes expands all items to an appropriate size
	int height_taken = 0;
	geometries.sort(); //we'll work with these in order from top to bottom
	for ( QRect *rect = geometries.first(); rect; rect = geometries.next() )
	{
		DivElement *element = geom_contents.find( rect );

		// For those elements that have no fixed height (lists), calculate the height
		int elementHeight=(int) (rect->height()/100.0*m_width); //Initialize with the current user settings
		if ( !element->fixedHeight() )
		{
			int elementWidth=(int) (rect->width()/100.0*m_width);
		
			// Generate a test page to calculate the size in khtml
			QString tempHTML="<HTML><HEAD><STYLE type=\"text/css\">";
			tempHTML+= classesCSS;
			tempHTML+= QString("#%1 {").arg(element->id());
			tempHTML+= QString("width: %1px;").arg(elementWidth);
			tempHTML+= "}";
			tempHTML+="BODY { margin: 0px; }"; //very important subtlety in determining the exact width
			tempHTML+="</STYLE></HEAD>";
			tempHTML+="<BODY>";
			tempHTML+=element->generateHTML();
			tempHTML+="</BODY></HTML>";

			KHTMLPart *sizeCalculator=new KHTMLPart((QWidget*) 0);
			sizeCalculator->view()->setVScrollBarMode (QScrollView::AlwaysOff);
			sizeCalculator->view()->setMinimumSize(QSize(elementWidth,0));
			sizeCalculator->view()->resize(QSize(elementWidth,0));
			sizeCalculator->begin(KURL(locateLocal("tmp","/")));
			sizeCalculator->write(tempHTML);
			sizeCalculator->end();
			
			// Set the size of the element
			int newHeight=sizeCalculator->view()->contentsHeight();
			/* if (newHeight>elementHeight) */ elementHeight=newHeight; // Keep user's size if it's defined as bigger

			delete sizeCalculator;
		}
		rect->setHeight((int)(ceil(elementHeight*100.0/m_width)));

		// Move elements around if there's any overlapping
		pushItemsDownIfNecessary( geometries, rect );

		element->addProperty( QString("top: %1px;").arg(static_cast<int>(rect->top()/100.0*m_width)) );
		element->addProperty( QString("left: %1px;").arg(static_cast<int>(rect->left()/100.0*m_width)) );
		element->addProperty( QString("width: %1px;").arg(static_cast<int>(rect->width()/100.0*m_width)) );
		element->addProperty( QString("height: %1px;").arg( elementHeight ) );

		height_taken = QMAX(rect->top()+int(elementHeight*100.0/m_width),height_taken);
	}

	return height_taken;
}

QMap<QString,QString> HTMLExporter::generateBlocksHTML( const Recipe &recipe )
{
	KConfig *config = KGlobal::config();
	QMap<QString,QString> html_map;

	//=======================TITLE======================//
	html_map.insert("title",recipe.title);

	//=======================INSTRUCTIONS======================//
	QString instr_html = QStyleSheet::escape(recipe.instructions);
	instr_html.replace("\n","<BR>");
	html_map.insert("instructions",instr_html);

	//=======================SERVINGS======================//
	QString servings_html = QString("<b>%1: </b>%2").arg(i18n("Servings")).arg(recipe.persons);
	html_map.insert("servings",servings_html);

	//========================PHOTO========================//
	QString image_url = QString("%1_photos/%2.png").arg(filename).arg(escape(recipe.title));
 	image_url = KURL::encode_string(image_url);
	QString photo_html = QString("<img src=\"%1\">").arg(image_url);
	html_map.insert("photo",photo_html);

	//=======================AUTHORS======================//
	QString authors_html;

	int counter=0;
	for ( ElementList::const_iterator author_it = recipe.authorList.begin(); author_it != recipe.authorList.end(); ++author_it )
	{
		if (counter) authors_html += ", ";
		authors_html += QStyleSheet::escape((*author_it).name);
		counter++;
	}
	if ( !authors_html.isEmpty() )
		authors_html.prepend( QString("<b>%1: </b>").arg(i18n("Authors")) );
	html_map.insert("authors",authors_html);

	//=======================CATEGORIES======================//
	QString categories_html;

	counter=0;
	for ( ElementList::const_iterator cat_it = recipe.categoryList.begin(); cat_it != recipe.categoryList.end(); ++cat_it )
	{
		if (counter) categories_html += ", ";
		categories_html += QStyleSheet::escape((*cat_it).name);
		counter++;
	}
	if ( !categories_html.isEmpty() )
		categories_html.prepend( QString("<b>%1: </b>").arg(i18n("Categories")) );
	
	html_map.insert("categories",categories_html);

	//=======================HEADER======================//
	QString header_html = QString("<b>%1 #%2</b>").arg(i18n("Recipe")).arg(recipe.recipeID);
	html_map.insert("header",header_html);

	//=======================INGREDIENTS======================//
	QString ingredients_html;
	config->setGroup("Formatting");

	MixedNumber::Format number_format = (config->readBoolEntry("Fraction")) ? MixedNumber::MixedNumberFormat : MixedNumber::DecimalFormat;

	QString ingredient_format = config->readEntry("Ingredient","%n%p: %a %u");

	for ( IngredientList::const_iterator ing_it = recipe.ingList.begin(); ing_it != recipe.ingList.end(); ++ing_it )
	{
		QString amount_str = MixedNumber((*ing_it).amount).toString( number_format );

		if ((*ing_it).amount <= 1e-10)
			amount_str = "";

		QString tmp_format(ingredient_format);
		tmp_format.replace(QRegExp(QString::fromLatin1("%n")),QStyleSheet::escape((*ing_it).name));
		tmp_format.replace(QRegExp(QString::fromLatin1("%a")),amount_str);
		tmp_format.replace(QRegExp(QString::fromLatin1("%u")),QStyleSheet::escape((*ing_it).units));
		tmp_format.replace(QRegExp(QString::fromLatin1("%p")),((*ing_it).prepMethod.isEmpty()) ?
				   QString::fromLatin1("") : QString::fromLatin1("; ")+QStyleSheet::escape((*ing_it).prepMethod));

		ingredients_html += QString("<li>%1</li>").arg(tmp_format);
	}
	if ( !ingredients_html.isEmpty() )
	{
		ingredients_html.prepend("<ul>");
		ingredients_html.append("</ul>");
	}
	html_map.insert("ingredients",ingredients_html);

	//=======================PROPERTIES======================//
	QString properties_html;
	IngredientProperty * prop;
	
	for ( prop = properties->getFirst(); prop; prop = properties->getNext() )
	{
		// if the amount given is <0, it means the property calculator found that the property was undefined for some ingredients, so the amount will be actually bigger

		QString amount_str;

		double prop_amount = prop->amount;
		if (prop_amount>0) { //TODO: make the precision configuratble
			prop_amount = double(qRound(prop_amount*10.0))/10.0; //not a "chemistry experiment" ;)  Let's only have one decimal place 
			amount_str = beautify(KGlobal::locale()->formatNumber(prop_amount,5));
		}
		else {
			prop_amount = double(qRound(-prop_amount*10.0))/10.0; //not a "chemistry experiment" ;)  Let's only have one decimal place
			amount_str = beautify(KGlobal::locale()->formatNumber(prop_amount,5));
			amount_str+="+";
		}

		properties_html += QString("<li>%1: <nobr>%2 %3</nobr></li>")
			    .arg(QStyleSheet::escape(prop->name))
			    .arg(amount_str)
			    .arg(QStyleSheet::escape(prop->units));
	}
	if ( !properties_html.isEmpty() )
	{
		properties_html.prepend("<ul>");
		properties_html.append("</ul>");
	}
	html_map.insert("properties",properties_html);
	
	///////////TODO?: Add an "end of recipe" element here (as a separator between this and the next recipes//////////////
	
	return html_map;
}

QString HTMLExporter::generateCSSClasses( const QDomDocument &doc )
{
	QString css;

	QStringList classes_list;
	classes_list << "title" << "instructions" << "servings" << "photo" << "authors" << 
	  "categories" << "header" << "ingredients" << "properties";
	  
	for ( QStringList::const_iterator it = classes_list.begin(); it != classes_list.end(); ++it )
	{
		css += "."+*it+"\n";
		css += "{\n";
		css += readFontProperties( doc, *it );
		css += readAlignmentProperties( doc, *it );
		css += readBgColorProperties( doc, *it );
		css += readTextColorProperties( doc, *it );
		css += readVisibilityProperties( doc, *it );
		css += "position: absolute;\n";
		css += "}\n\n";
	}

	return css;
}

void HTMLExporter::readGeometry( QRect *geom, const QDomDocument &doc, const QString &object )
{
	QDomElement geom_el = getLayoutAttribute( doc, object, "geometry" );

	if ( !geom_el.isNull() )
	{
		geom->setLeft( qRound(geom_el.attribute("left").toDouble()) );
		geom->setTop( qRound(geom_el.attribute("top").toDouble()) );
		geom->setWidth( qRound(geom_el.attribute("width").toDouble()) );
		geom->setHeight( qRound(geom_el.attribute("height").toDouble()) );
	}
}

QString HTMLExporter::readAlignmentProperties( const QDomDocument &doc, const QString &object )
{
	QDomElement el = getLayoutAttribute( doc, object, "alignment" );
	
	if ( !el.isNull() )
	{
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

QString HTMLExporter::readBgColorProperties( const QDomDocument &doc, const QString &object )
{
	QDomElement el = getLayoutAttribute( doc, object, "background-color" );

	if ( !el.isNull() )
		return QString("background-color: %1;\n").arg(el.text());
		
	return QString::null;
}

QString HTMLExporter::readFontProperties( const QDomDocument &doc, const QString &object )
{
	QDomElement el = getLayoutAttribute( doc, object, "font" );
	
	if ( !el.isNull() )
	{
		QString text;
		QFont font;
		font.fromString( el.text() );

		text += QString("font-family: %1;\n").arg(font.family());
		text += QString("font-size: %1pt;\n").arg(font.pointSize());
		text += QString("font-weight: %1;\n").arg(font.weight());
		
		return text;
	}
	
	return QString::null;
}

QString HTMLExporter::readTextColorProperties( const QDomDocument &doc, const QString &object )
{
	QDomElement el = getLayoutAttribute( doc, object, "text-color" );

	if ( !el.isNull() )
		return QString("color: %1;\n").arg(el.text());
		
	return QString::null;
}

QString HTMLExporter::readVisibilityProperties( const QDomDocument &doc, const QString &object )
{
	QDomElement el = getLayoutAttribute( doc, object, "visible" );
	
	if ( !el.isNull() )
	{
		bool shown = (el.text() == "false") ? false : true;
		if ( shown )
			return "visibility: visible;\n";
		else
			return "visibility: hidden;\n";
	}
	
	return QString::null;
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
		QFile photo( filename+"_photos/"+escape(*it)+".png" );
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

QString HTMLExporter::escape( const QString & str )
{
	QString tmp(str);
	return tmp.replace('/',"_");
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

DivElement::DivElement( const QString &id, const QString &className, const QString &content ) :
  m_id(id),
  m_class(className),
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

	result += QString("<DIV id=\"%1\" class=\"%2\">\n").arg(m_id).arg(m_class);
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
