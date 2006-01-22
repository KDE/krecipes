/***************************************************************************
*   Copyright (C) 2003-2005                                               *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "setupdisplay.h"

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kfontdialog.h>
#include <kcolordialog.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kiconloader.h>
#include <kstandarddirs.h>
#include <kinputdialog.h>
#include <ktempfile.h>

#include <khtmlview.h>
#include <dom/dom_doc.h>
#include <dom/css_rule.h>

#include <qaction.h>
#include <qlabel.h>
#include <qfile.h>
#include <qregexp.h>
#include <qtextedit.h>
#include <qtooltip.h>
#include <qobjectlist.h>
#include <qvaluelist.h>
#include <qlayout.h>

#include "image.h"
#include "datablocks/mixednumber.h"
#include "dialogs/borderdialog.h"
#include "exporters/htmlexporter.h"

#include <cmath>

KreDisplayItem::KreDisplayItem( const QString &n, const QString &_name ) : nodeId(n), name(_name)
{
	clear();
}

void KreDisplayItem::clear()
{
	alignment = Qt::AlignHCenter;
	show = true;
	backgroundColor = QColor(255,255,255);
	textColor = QColor(0,0,0);
	columns = 1;
}

SetupDisplay::SetupDisplay( const Recipe &sample, QWidget *parent ) : KHTMLPart(parent),
		box_properties( new PropertiesMap ),
		node_item_map( new QMap<QString, KreDisplayItem*> ),
		has_changes( false ),
		popup(0)
{
	connect( this, SIGNAL( popupMenu(const QString &,const QPoint &) ), SLOT( nodeClicked(const QString &,const QPoint &) ) );

	if ( sample.recipeID != -1 )
		m_sample = sample;
	else {
		m_sample.title = i18n("Recipe Title");
 		m_sample.yield.amount = 0;
		m_sample.categoryList.append( Element(i18n( "Category 1, Category 2, ..." ) ) );
		m_sample.instructions = i18n("Instructions");
		m_sample.prepTime = QTime(0,0);
	
		m_sample.authorList.append( Element(i18n( "Author 1, Author 2, ..." )) );
	
		Ingredient ing;
		ing.name = i18n("Ingredient 1");
		m_sample.ingList.append( ing );
	
		ing.name = i18n("Ingredient 2");
		m_sample.ingList.append( ing );

		ing.name = "...";
		m_sample.ingList.append( ing );
	
		RatingCriteria rc;
		Rating rating1;
		rating1.rater = i18n("Rater");
		rating1.comment = i18n("Comment");
	
		rc.name = i18n("Criteria 1");
		rc.stars = 5.0;
		rating1.append(rc);
	
		rc.name = i18n("Criteria 2");
		rc.stars = 2.5;
		rating1.append(rc);

		IngredientProperty prop;
		prop.name = i18n("Property 1");
		m_sample.properties.append(prop);
		prop.name = i18n("Property 2");
		m_sample.properties.append(prop);
		prop.name = "...";
		m_sample.properties.append(prop);

		m_sample.ratingList.append(rating1);
	}

	loadHTMLView();
	show();

	createItem( "background", i18n("Background"), BackgroundColor );
	createItem( "title", i18n("Title"), Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
	createItem( "instructions", i18n("Instructions"), Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
	createItem( "prep_time", i18n("Preparation Time"), Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
	createItem( "photo", i18n("Photo"), Visibility | Border );
	createItem( "authors", i18n("Authors"), Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
	createItem( "categories", i18n("Categories"), Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
	createItem( "header", i18n("Header"), Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
	createItem( "ingredients", i18n("Ingredients"), Font | BackgroundColor | TextColor | Visibility | Alignment | Border | Columns );
	createItem( "properties", i18n("Properties"), Font | BackgroundColor | TextColor | Visibility | Alignment | Border | Columns );
	createItem( "ratings", i18n("Ratings"), Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
	createItem( "yield", i18n("Yield"), Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
}

SetupDisplay::~SetupDisplay()
{
	delete box_properties;
	delete node_item_map;
}

void SetupDisplay::loadHTMLView( const QString &templateFile, const QString &styleFile )
{
	QString tmp_filename = locateLocal( "tmp", "krecipes_recipe_view" );
	HTMLExporter exporter( tmp_filename + ".html", "html" );
	if ( templateFile != QString::null )
		exporter.setTemplate( templateFile );
	if ( styleFile != QString::null )
		exporter.setStyle( styleFile );

	RecipeList recipeList;
	recipeList.append(m_sample);

	QFile file(tmp_filename + ".html");
	if ( file.open( IO_WriteOnly ) ) {
		QTextStream stream(&file);
		exporter.writeStream(stream,recipeList);
	}
	else {
		kdDebug()<<"Unable to open file for writing"<<endl;
	}
	file.close();

	KURL url;
	url.setPath( tmp_filename + ".html" );
	openURL( url );
	kdDebug() << "Opening URL: " << url.htmlURL() << endl;
}

void SetupDisplay::loadTemplate( const QString &filename )
{
	bool storeChangedState = has_changes;
	KTempFile tmpFile;
	saveLayout(tmpFile.name());
	has_changes = storeChangedState; //saveLayout() sets changes to false
	
	loadHTMLView( filename, tmpFile.name() );

	m_activeTemplate = filename;
}

void SetupDisplay::createItem( const QString &node, const QString &name, unsigned int properties )
{
	KreDisplayItem * item = new KreDisplayItem( node, name );
	box_properties->insert( item, properties );
	node_item_map->insert( node, item );
}

void SetupDisplay::loadLayout( const QString &filename )
{
	QFile input( filename );
	if ( input.open( IO_ReadOnly ) ) {
		QDomDocument doc;
		QString error;
		int line;
		int column;
		if ( !doc.setContent( &input, &error, &line, &column ) ) {
			kdDebug() << QString( i18n( "\"%1\" at line %2, column %3.  This may not be a Krecipes layout file." ) ).arg( error ).arg( line ).arg( column ) << endl;
			return ;
		}

		while ( m_styleSheet.cssRules().length() != 0 )
			m_styleSheet.deleteRule(0);

		QMap<QString,KreDisplayItem*>::iterator it;
		for ( it = node_item_map->begin(); it != node_item_map->end(); ++it ) {
			it.data()->clear();
		}
		processDocument( doc );

		loadHTMLView(m_activeTemplate, filename);

		has_changes = false;
	}
	else
		kdDebug() << "Unable to open file: " << filename << endl;
}

void SetupDisplay::beginObject( const QString &object )
{
	QMap<QString, KreDisplayItem*>::iterator map_it = node_item_map->find( object );
	if ( map_it != node_item_map->end() )
		m_currentItem = map_it.data();
	else
		m_currentItem = 0;
}

void SetupDisplay::endObject()
{
	m_currentItem = 0;
}

void SetupDisplay::loadBackgroundColor( const QString &object, const QColor &color )
{
	if ( m_currentItem ) {
		m_currentItem->backgroundColor = color;
		m_styleSheet.insertRule("."+object+" { "+bgColorAsCSS(color)+" }",m_styleSheet.cssRules().length());
	}
}

void SetupDisplay::loadFont( const QString &object, const QFont &font )
{
	if ( m_currentItem ) {
		m_currentItem->font = font;
		m_styleSheet.insertRule("."+object+" { "+fontAsCSS(font)+" }",m_styleSheet.cssRules().length());
	}
}

void SetupDisplay::loadTextColor( const QString &object, const QColor &color )
{
	if ( m_currentItem ) {
		m_currentItem->textColor = color;
		m_styleSheet.insertRule("."+object+" { "+textColorAsCSS(color)+" }",m_styleSheet.cssRules().length());
	}
}

void SetupDisplay::loadVisibility( const QString &object, bool visible )
{
	if ( m_currentItem ) {
		m_currentItem->show = visible;
		emit itemVisibilityChanged( m_currentItem, visible );
	
		m_styleSheet.insertRule("."+object+" { "+visibilityAsCSS(visible)+" }",m_styleSheet.cssRules().length());
	}
}

void SetupDisplay::loadAlignment( const QString &object, int alignment )
{
	if ( m_currentItem ) {
		m_currentItem->alignment = alignment;
		m_styleSheet.insertRule("."+object+" { "+alignmentAsCSS(alignment)+" }",m_styleSheet.cssRules().length());
	}
}

void SetupDisplay::loadBorder( const QString &object, const KreBorder& border )
{
	if ( m_currentItem ) {
		m_currentItem->border = border;
		m_styleSheet.insertRule("."+object+" { "+borderAsCSS(border)+" }",m_styleSheet.cssRules().length());
	}
}

void SetupDisplay::loadColumns( const QString &/*object*/, int cols )
{
	if ( m_currentItem ) {
		m_currentItem->columns = cols;
	}
}

void SetupDisplay::saveLayout( const QString &filename )
{
	QDomImplementation dom_imp;
	QDomDocument doc = dom_imp.createDocument( QString::null, "krecipes-layout", dom_imp.createDocumentType( "krecipes-layout", QString::null, QString::null ) );

	QDomElement layout_tag = doc.documentElement();
	layout_tag.setAttribute( "version", 0.4 );
	//layout_tag.setAttribute( "generator", QString("Krecipes v%1").arg(krecipes_version()) );
	doc.appendChild( layout_tag );

	for ( QMap<QString, KreDisplayItem*>::const_iterator it = node_item_map->begin(); it != node_item_map->end(); ++it ) {
		QDomElement base_tag = doc.createElement( it.key() );
		layout_tag.appendChild( base_tag );

		int properties = (*box_properties)[it.data()];
		if ( properties & BackgroundColor ) {
			QDomElement backgroundcolor_tag = doc.createElement( "background-color" );
			backgroundcolor_tag.appendChild( doc.createTextNode( it.data()->backgroundColor.name() ) );
			base_tag.appendChild( backgroundcolor_tag );
		}

		if ( properties & TextColor ) {
			QDomElement textcolor_tag = doc.createElement( "text-color" );
			textcolor_tag.appendChild( doc.createTextNode( it.data()->textColor.name() ) );
			base_tag.appendChild( textcolor_tag );
		}

		if ( properties & Font ) {
			QDomElement font_tag = doc.createElement( "font" );
			font_tag.appendChild( doc.createTextNode( it.data()->font.toString() ) );
			base_tag.appendChild( font_tag );
		}

		if ( properties & Visibility ) {
			QDomElement visibility_tag = doc.createElement( "visible" );
			visibility_tag.appendChild( doc.createTextNode( (it.data()->show) ? "true" : "false" ) );
			base_tag.appendChild( visibility_tag );
		}

		if ( properties & Alignment ) {
			QDomElement alignment_tag = doc.createElement( "alignment" );
			alignment_tag.appendChild( doc.createTextNode( QString::number( it.data()->alignment ) ) );
			base_tag.appendChild( alignment_tag );
		}

		if ( properties & Border ) {
			QDomElement border_tag = doc.createElement( "border" );
			border_tag.setAttribute( "width", it.data()->border.width );
			border_tag.setAttribute( "style", it.data()->border.style );
			border_tag.setAttribute( "color", it.data()->border.color.name() );
			base_tag.appendChild( border_tag );
		}

		if ( properties & Columns ) {
			QDomElement columns_tag = doc.createElement( "columns" );
			columns_tag.appendChild( doc.createTextNode( QString::number( it.data()->columns ) ) );
			base_tag.appendChild( columns_tag );
		}
	}

	QFile out_file( filename );
	if ( out_file.open( IO_WriteOnly ) ) {
		has_changes = false;

		QTextStream stream( &out_file );
		stream << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n" << doc.toString();
	}
	else
		kdDebug() << "Error: Unable to write to file " << filename << endl;
}

void SetupDisplay::begin(const KURL &url, int xOffset, int yOffset)
{
	kdDebug()<<"begin"<<endl;
	KHTMLPart::begin(url,xOffset,yOffset);
	kdDebug()<<"end"<<endl;

	DOM::Document doc = document();
	DOM::DOMImplementation impl = doc.implementation();
	kdDebug() << "(1) document: " << document().handle() << endl;
	if ( !impl.isNull() ) {
		m_styleSheet = impl.createCSSStyleSheet("-krecipes","screen");
		doc.addStyleSheet(m_styleSheet);
		applyStylesheet();
	}
}

void SetupDisplay::nodeClicked(const QString &url,const QPoint &point)
{
	DOM::Node node = nodeUnderMouse();
	DOM::Element element;
	if ( node.nodeType() != DOM::Node::ELEMENT_NODE ) {
		kdDebug()<<"not an element"<<endl;
		element = (DOM::Element)node.parentNode();
	}
	else
		element = (DOM::Element)node;

	while ( !element.parentNode().isNull() ) {
		if ( element.hasAttribute("class") ) {
			QString id = element.getAttribute("class").string();
			if ( node_item_map->keys().contains(id) )
				break;
		}

		element = (DOM::Element)element.parentNode();
	}

	m_currNodeId = element.getAttribute("class").string();
	if ( m_currNodeId.isEmpty() ) {
		kdDebug()<<"Code error: unable to determine class of selected element"<<endl;
		return;
	}

	KreDisplayItem *item = *node_item_map->find( m_currNodeId );
	
	delete popup;
	popup = new KPopupMenu( view() );
	popup->insertTitle( item->name );

	unsigned int properties = 0;
	for ( PropertiesMap::const_iterator it = box_properties->begin(); it != box_properties->end(); ++it ) {
		if ( it.key()->nodeId == m_currNodeId ) {
			properties = it.data();
			break;
		}
	}

	KIconLoader il;

	if ( properties & BackgroundColor )
		popup->insertItem( i18n( "Background Color..." ), this, SLOT( setBackgroundColor() ) );

	if ( properties & TextColor )
		popup->insertItem( i18n( "Text Color..." ), this, SLOT( setTextColor() ) );

	if ( properties & Font )
		popup->insertItem( il.loadIconSet( "text", KIcon::Small, 16 ), i18n( "Font..." ), this, SLOT( setFont() ) );

	if ( properties & Visibility ) {
		int id = popup->insertItem( i18n( "Show" ), this, SLOT( setShown( int ) ) );
		popup->setItemChecked( id, item->show );
	}

	if ( properties & Alignment ) {
		QPopupMenu * sub_popup = new QPopupMenu( popup );

		QActionGroup *alignment_actions = new QActionGroup( this );
		alignment_actions->setExclusive( true );

		QAction *c_action = new QAction( i18n( "Center" ), i18n( "Center" ), 0, alignment_actions, 0, true );
		QAction *l_action = new QAction( i18n( "Left" ), i18n( "Left" ), 0, alignment_actions, 0, true );
		QAction *r_action = new QAction( i18n( "Right" ), i18n( "Right" ), 0, alignment_actions, 0, true );

		int align = item->alignment;
		if ( align & Qt::AlignHCenter )
			c_action->setOn(true);
		if ( align & Qt::AlignLeft )
			l_action->setOn(true);
		if ( align & Qt::AlignRight )
			r_action->setOn(true);

		connect( alignment_actions, SIGNAL( selected( QAction* ) ), SLOT( setAlignment( QAction* ) ) );

		popup->insertItem( i18n( "Alignment" ), sub_popup );

		alignment_actions->addTo( sub_popup );
	}

	if ( properties & Border )
		popup->insertItem( i18n( "Border..." ), this, SLOT( setBorder() ) );

	if ( properties & Columns )
		popup->insertItem( i18n( "Columns..." ), this, SLOT( setColumns() ) );

	popup->popup( point );
}

void SetupDisplay::applyStylesheet()
{
	if ( !document().isNull() && !m_styleSheet.isNull() ) {
		document().removeStyleSheet(m_styleSheet);
		document().addStyleSheet(m_styleSheet);
	}
}

void SetupDisplay::setBackgroundColor()
{
	KreDisplayItem *item = *node_item_map->find( m_currNodeId );
	if ( KColorDialog::getColor( item->backgroundColor, view() ) == QDialog::Accepted ) {
		m_currentItem = item;
		loadBackgroundColor(m_currNodeId,item->backgroundColor);
		m_currentItem = 0;

		applyStylesheet();
		has_changes = true;
	}
}

void SetupDisplay::setBorder()
{
	KreDisplayItem *item = *node_item_map->find( m_currNodeId );
	BorderDialog borderDialog( item->border, view() );
	if ( borderDialog.exec() == QDialog::Accepted ) {
		m_currentItem = item;
		loadBorder( m_currNodeId, borderDialog.border() );
		m_currentItem = 0;

		applyStylesheet();
		has_changes = true;
	}
}

void SetupDisplay::setColumns()
{
	KreDisplayItem *item = *node_item_map->find( m_currNodeId );
	int cols = KInputDialog::getInteger( QString::null, i18n("Select the number of columns to use:"), item->columns, 1, 100, 1, 0, view() );
	if ( cols > 0 ) {
		m_currentItem = item;
		loadColumns( m_currNodeId, cols );
		m_currentItem = 0;

		loadTemplate( m_activeTemplate );
		has_changes = true;
	}
}

void SetupDisplay::setTextColor()
{
	KreDisplayItem *item = *node_item_map->find( m_currNodeId );
	if ( KColorDialog::getColor( item->textColor, view() ) == QDialog::Accepted ) {
		m_currentItem = item;
		loadTextColor(m_currNodeId,item->textColor);
		m_currentItem = 0;

		applyStylesheet();
		has_changes = true;
	}
}

void SetupDisplay::setShown( int id )
{
	KreDisplayItem *item = *node_item_map->find( m_currNodeId );
	emit itemVisibilityChanged( item, !popup->isItemChecked( id ) );

	m_currentItem = item;
	loadVisibility(m_currNodeId,!popup->isItemChecked( id ));
	m_currentItem = 0;

	applyStylesheet();
	has_changes = true;
}

void SetupDisplay::setFont()
{
	KreDisplayItem *item = *node_item_map->find( m_currNodeId );
	if ( KFontDialog::getFont( item->font, false, view() ) == QDialog::Accepted ) {
		m_currentItem = item;
		loadFont(m_currNodeId,item->font);
		m_currentItem = 0;

		applyStylesheet();
		has_changes = true;
	}
}

void SetupDisplay::setAlignment( QAction *action )
{
	KreDisplayItem *item = *node_item_map->find( m_currNodeId );

	//TODO: isn't there a simpler way to do this...
	//preserve non-horizontal alignment flags
	if ( item->alignment & Qt::AlignRight )
		item->alignment ^= Qt::AlignRight;
	if ( item->alignment & Qt::AlignHCenter )
		item->alignment ^= Qt::AlignHCenter;
	if ( item->alignment & Qt::AlignLeft )
		item->alignment ^= Qt::AlignLeft;

	if ( action->text() == i18n( "Center" ) )
		item->alignment |= Qt::AlignHCenter;
	else if ( action->text() == i18n( "Left" ) )
		item->alignment |= Qt::AlignLeft;
	else if ( action->text() == i18n( "Right" ) )
		item->alignment |= Qt::AlignRight;

	m_currentItem = item;
	loadAlignment(m_currNodeId,item->alignment);
	m_currentItem = 0;

	applyStylesheet();
	has_changes = true;
}

void SetupDisplay::setItemShown( KreDisplayItem *item, bool visible )
{
	item->show = visible;

	m_styleSheet.insertRule("."+item->nodeId+" { visibility:"+(item->show?"visible":"hidden")+" }",m_styleSheet.cssRules().length());
	applyStylesheet();

	has_changes = true;
}

void SetupDisplay::changeMade( void )
{
	has_changes = true;
}

#include "setupdisplay.moc"
