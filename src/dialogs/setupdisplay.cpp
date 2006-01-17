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

KreDisplayItem::KreDisplayItem( const QString &n ) : nodeId(n)
{
	alignment = Qt::AlignHCenter;
	show = true;
	backgroundColor = QColor(255,255,255);
	textColor = QColor(0,0,0);
}


SetupDisplay::SetupDisplay( const Recipe &sample, QWidget *parent ) : KHTMLPart(parent),
		box_properties( new PropertiesMap ),
		node_item_map( new QMap<QString, KreDisplayItem*> ),
		has_changes( false ),
		popup(0)
{
	connect( this, SIGNAL( popupMenu(const QString &,const QPoint &) ), SLOT( nodeClicked(const QString &,const QPoint &) ) );

	QString tmp_filename = locateLocal( "tmp", "krecipes_recipe_view" );
	HTMLExporter exporter( 0, tmp_filename + ".html", "html" );

	RecipeList recipeList;
	recipeList.append(sample);

	QFile file(tmp_filename + ".html");
	if ( file.open( IO_WriteOnly ) ) {
		QTextStream stream(&file);
		exporter.writeStream(stream,recipeList);
	}
	else {
		printf("Unable to open file for writing\n");
	}
	file.close();


	KURL url;
	url.setPath( tmp_filename + ".html" );
	openURL( url );
	show();
	kdDebug() << "Opening URL: " << url.htmlURL() << endl;

	createItem( "background", BackgroundColor );
	createItem( "title", Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
	createItem( "instructions", Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
	createItem( "prep_time", Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
	createItem( "photo", Visibility | Border );
	createItem( "authors", Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
	createItem( "categories", Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
	createItem( "header", Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
	createItem( "ingredients", Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
	createItem( "properties", Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
	createItem( "ratings", Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
	createItem( "yield", Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
}

SetupDisplay::~SetupDisplay()
{
	delete box_properties;
	delete node_item_map;
}

void SetupDisplay::createItem( const QString &node, unsigned int properties )
{
	KreDisplayItem * item = new KreDisplayItem( node );
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

		QDomElement layout = doc.documentElement();

		if ( layout.tagName() != "krecipes-layout" ) {
			kdDebug() << "This file does not appear to be a valid Krecipes layout file." << endl;
			return ;
		}

		QDomNodeList l = layout.childNodes();
		for ( unsigned i = 0 ; i < l.count(); i++ ) {
			QDomElement el = l.item( i ).toElement();
			QString tagName = el.tagName();
			QMap<QString, KreDisplayItem*>::iterator map_it = node_item_map->find( tagName );
			if ( map_it != node_item_map->end() ) {
				QDomNodeList l = el.childNodes();
				for ( unsigned i = 0 ; i < l.count(); i++ ) {
					QDomElement el = l.item( i ).toElement();
					QString subTagName = el.tagName();

					if ( subTagName == "background-color" )
						loadBackgroundColor( *map_it, el );
					else if ( subTagName == "font" )
						loadFont( *map_it, el );
					else if ( subTagName == "text-color" )
						loadTextColor( *map_it, el );
					else if ( subTagName == "visible" )
						loadVisibility( *map_it, el );
					else if ( subTagName == "alignment" )
						loadAlignment( *map_it, el );
					else if ( subTagName == "border" )
						loadBorder( *map_it, el );
					else
						kdDebug() << "Warning: Unknown tag within <" << tagName << ">: " << subTagName << endl;
				}
			}
			else
				kdDebug() << "Warning: Unknown tag within <krecipes-layout>: " << tagName << endl;
		}
		applyStylesheet();
		has_changes = false;
	}
	else
		kdDebug() << "Unable to open file: " << filename << endl;
}


void SetupDisplay::loadBackgroundColor( KreDisplayItem *item, const QDomElement &tag )
{
	item->backgroundColor.setNamedColor( tag.text() );
	setBackgroundColor(item->nodeId,item->backgroundColor);
}

void SetupDisplay::loadFont( KreDisplayItem *item, const QDomElement &tag )
{
	QFont f;
	if ( f.fromString( tag.text() ) ) {
		item->font = f;
		setFont(item->nodeId,item->font);
	}
}

void SetupDisplay::loadTextColor( KreDisplayItem *item, const QDomElement &tag )
{
	item->textColor.setNamedColor( tag.text() );
	setTextColor(item->nodeId,item->textColor);
}

void SetupDisplay::loadVisibility( KreDisplayItem *item, const QDomElement &tag )
{
	bool visible = ( tag.text() == "false" ) ? false : true;
	item->show = visible;
	emit itemVisibilityChanged( item, visible );

	setShown(item->nodeId,item->show);
}

void SetupDisplay::loadAlignment( KreDisplayItem *item, const QDomElement &tag )
{
	item->alignment = tag.text().toInt();
	setAlignment(item->nodeId,item->alignment);
}

void SetupDisplay::loadBorder( KreDisplayItem *item, const QDomElement &tag )
{
	QColor c;
	c.setNamedColor( tag.attribute( "color" ) );
	item->border = KreBorder( tag.attribute( "width" ).toInt(), tag.attribute( "style" ), c );
	setBorder(item->nodeId,item->border);
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
	}
	else
		kdDebug()<<"damn it, why is this null?"<<endl;
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
		if ( element.hasAttribute("id") ) {
			QString id = element.getAttribute("id").string();
			if ( node_item_map->keys().contains(id) )
				break;
		}

		element = (DOM::Element)element.parentNode();
	}

	m_currNodeId = element.getAttribute("id").string();
	KreDisplayItem *item = *node_item_map->find( m_currNodeId );
	
	delete popup;
	popup = new KPopupMenu( view() );
	popup->insertTitle( m_currNodeId );

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
		setBackgroundColor(m_currNodeId,item->backgroundColor);
		applyStylesheet();
	}
}

void SetupDisplay::setBackgroundColor( const QString &nodeId, const QColor &color )
{
	m_styleSheet.insertRule("#"+nodeId+" { background-color:"+color.name()+"; }",m_styleSheet.cssRules().length());

	has_changes = true;
}

void SetupDisplay::setBorder()
{
	KreDisplayItem *item = *node_item_map->find( m_currNodeId );
	BorderDialog borderDialog( item->border, view() );
	if ( borderDialog.exec() == QDialog::Accepted ) {
		item->border = borderDialog.border();
		setBorder( m_currNodeId, item->border );
		applyStylesheet();
	}
}
void SetupDisplay::setBorder( const QString &nodeId, const KreBorder& border )
{
	m_styleSheet.insertRule("#"+nodeId+" { border:"+QString::number(border.width)+"px "+border.style+" "+border.color.name()+"; }",m_styleSheet.cssRules().length());

	has_changes = true;
}

void SetupDisplay::setTextColor()
{
	KreDisplayItem *item = *node_item_map->find( m_currNodeId );
	if ( KColorDialog::getColor( item->textColor, view() ) == QDialog::Accepted ) {
		setTextColor(m_currNodeId,item->textColor);
		applyStylesheet();
	}
}

void SetupDisplay::setTextColor( const QString &nodeId, const QColor &color )
{
	m_styleSheet.insertRule("#"+nodeId+" { color:"+color.name()+"; }",m_styleSheet.cssRules().length());

	has_changes = true;
}

void SetupDisplay::setFont()
{
	KreDisplayItem *item = *node_item_map->find( m_currNodeId );
	if ( KFontDialog::getFont( item->font, false, view() ) == QDialog::Accepted ) {
		setFont(m_currNodeId,item->font);
		applyStylesheet();
	}
}

void SetupDisplay::setFont( const QString &nodeId, const QFont &font )
{
	QString text;
	text += QString( "font-family: %1;\n" ).arg( font.family() );
	text += QString( "font-weight: %1;\n" ).arg( font.weight() );
	text += QString( "font-size: %1pt;\n" ).arg( font.pointSize() );

	m_styleSheet.insertRule("#"+nodeId+" { "+text+" }",m_styleSheet.cssRules().length());
	has_changes = true;
}

void SetupDisplay::setShown( int id )
{
	KreDisplayItem *item = *node_item_map->find( m_currNodeId );
	item->show = !popup->isItemChecked( id );
	emit itemVisibilityChanged( item, !popup->isItemChecked( id ) );
	setShown(m_currNodeId,item->show);
	applyStylesheet();
}

void SetupDisplay::setShown( const QString &nodeId, bool show )
{
	m_styleSheet.insertRule("#"+nodeId+" { visibility:"+(show?"visible":"hidden")+" }",m_styleSheet.cssRules().length());
	has_changes = true;
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

	setAlignment(m_currNodeId,item->alignment);
	applyStylesheet();
}

void SetupDisplay::setAlignment( const QString &nodeId, int alignment )
{
	QString text;
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

	if ( !text.isEmpty() ) {
		m_styleSheet.insertRule("#"+nodeId+" { "+text+" }",m_styleSheet.cssRules().length());

		has_changes = true;
	}
}

void SetupDisplay::setItemShown( KreDisplayItem *item, bool visible )
{
	item->show = visible;

	m_styleSheet.insertRule("#"+item->nodeId+" { visibility:"+(item->show?"visible":"hidden")+" }",m_styleSheet.cssRules().length());
	applyStylesheet();

	has_changes = true;
}

void SetupDisplay::changeMade( void )
{
	has_changes = true;
}

#include "setupdisplay.moc"
