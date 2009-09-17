/***************************************************************************
*   Copyright © 2006 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "klomanager.h"

#include <kdebug.h>

#include <qdom.h>
#include <QStringList>

KLOManager::KLOManager()
{
}

KLOManager::~KLOManager()
{

}

QStringList KLOManager::classes()
{
	QStringList classesList;
	classesList << "title" << "instructions" << "yield" << "prep_time" << "photo" << "authors" <<
		"categories" << "header" << "ingredients" << "properties" << "ratings";
	return classesList;
}

void KLOManager::processDocument( const QDomDocument &doc )
{
	QDomElement layout = doc.documentElement();

	if ( layout.tagName() != "krecipes-layout" ) {
		kDebug() << "This file does not appear to be a valid Krecipes layout file." ;
		return ;
	}

	QDomNodeList l = layout.childNodes();
	for ( int i = 0 ; i < l.count(); i++ ) {
		QDomElement el = l.item( i ).toElement();
		QString tagName = el.tagName();
		QDomNodeList subList = el.childNodes();
		/*if ( !*/beginObject( tagName )/* ) {*/; //###: just a thought....
		for ( int j = 0 ; j < subList.count(); j++ ) {
			QDomElement subEl = subList.item( j ).toElement();
			QString subTagName = subEl.tagName();

			if ( subTagName == "background-color" )
				loadBackgroundColor( tagName, getColorAttribute(el,subTagName) );
			else if ( subTagName == "font" )
				loadFont( tagName, getFontAttribute(el,subTagName) );
			else if ( subTagName == "text-color" )
				loadTextColor( tagName, getColorAttribute(el,subTagName) );
			else if ( subTagName == "visible" )
				loadVisibility( tagName, getBoolAttribute(el,subTagName) );
			else if ( subTagName == "alignment" )
				loadAlignment( tagName, getIntAttribute(el,subTagName) );
			else if ( subTagName == "border" )
				loadBorder( tagName, getBorderAttribute(el,subTagName) );
			else if ( subTagName == "columns" )
				loadColumns( tagName, getIntAttribute(el,subTagName) );
			else if ( subTagName == "size" )
				loadSize( tagName, getSizeAttribute(el,subTagName) );
			else
				kDebug() << "Warning: Unknown tag within <" << tagName << ">: " << subTagName ;
		}
		endObject();
	}
}

QDomElement KLOManager::getLayoutAttribute( const QDomElement &object, const QString &attribute ) const
{
	QDomNodeList l = object.childNodes();
	for ( int i = 0; i < l.count(); i++ ) {
		QDomElement el = l.item( i ).toElement();

		if ( el.tagName() == attribute )
			return el;
	}

	kDebug() << "Warning: Requested attribute \"" << attribute << "\" not found." ;
	return QDomElement();
}

bool KLOManager::getBoolAttribute( const QDomElement &object, const QString &attribute, bool defaultValue ) const
{
	QDomElement result = getLayoutAttribute( object, attribute );
	if ( result.isNull() ) {
		return defaultValue;
	}
	else {
		return result.text() == "true";
	}
}

QColor KLOManager::getColorAttribute( const QDomElement &object, const QString &attribute, const QColor &defaultValue ) const
{
	QDomElement result = getLayoutAttribute( object, attribute );
	if ( result.isNull() ) {
		return defaultValue;
	}
	else {
		return QColor(result.text());
	}
}

QString KLOManager::getTextAttribute( const QDomElement &object, const QString &attribute, const QString &defaultValue ) const
{
	QDomElement result = getLayoutAttribute( object, attribute );
	if ( result.isNull() ) {
		return defaultValue;
	}
	else {
		return result.text();
	}
}

int KLOManager::getIntAttribute( const QDomElement &object, const QString &attribute, int defaultValue ) const
{
	QDomElement result = getLayoutAttribute( object, attribute );
	if ( result.isNull() ) {
		return defaultValue;
	}
	else {
		return result.text().toInt();
	}
}

KreBorder KLOManager::getBorderAttribute( const QDomElement &object, const QString &attribute, const KreBorder &defaultValue ) const
{
	QDomElement result = getLayoutAttribute( object, attribute );
	if ( result.isNull() ) {
		return defaultValue;
	}
	else {
		return KreBorder( result.attribute( "width" ).toInt(), result.attribute( "style" ), QColor(result.attribute( "color" )) );
	}
}

QFont KLOManager::getFontAttribute( const QDomElement &object, const QString &attribute, const QFont &defaultValue ) const
{
	QDomElement result = getLayoutAttribute( object, attribute );
	if ( result.isNull() ) {
		return defaultValue;
	}
	else {
		QFont font;
		font.fromString(result.text());
		return font;
	}
}

QSize KLOManager::getSizeAttribute( const QDomElement &object, const QString &attribute, const QSize &defaultValue ) const
{
	QDomElement result = getLayoutAttribute( object, attribute );
	if ( result.isNull() ) {
		return defaultValue;
	}
	else {
		return QSize( result.attribute("width").toInt(), result.attribute("height").toInt() );
	}
}

QString KLOManager::alignmentAsCSS( int alignment )
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

	return text;
}

QString KLOManager::borderAsCSS( const KreBorder &border )
{
	return QString( "border: %1px %2 %3;\n" ).arg(border.width).arg(border.style).arg(border.color.name());
}

QString KLOManager::bgColorAsCSS( const QColor &color )
{
	return QString( "background-color: %1;\n" ).arg( color.name() );
}

QString KLOManager::fontAsCSS( const QFont &font )
{
	QString text;

	text += QString( "font-family: %1;\n" ).arg( font.family() );
	text += QString( "font-weight: %1;\n" ).arg( font.weight() );
	text += QString( "font-size: %1pt;\n" ).arg( font.pointSize() );
	if ( font.underline() )
		text += "text-decoration: underline;\n";
	if ( font.strikeOut() )
		text += "text-decoration: line-through;\n";
	if ( font.bold() )
		text += "font-weight: bold;\n";
	if ( font.italic() )
		text += "font-style: italic;\n";

	return text;
}

QString KLOManager::textColorAsCSS( const QColor &color )
{
	return QString( "color: %1;\n" ).arg( color.name() );
}

QString KLOManager::visibilityAsCSS( bool visible )
{
	if ( visible )
		return "visibility: visible;\n";
	else
		return "visibility: hidden;\n";
}

QString KLOManager::sizeAsCSS( const QSize &size )
{
	QString text;

	if ( size.height() != 0 )
		text += QString("height: %1;\n").arg(size.height());
	if ( size.width() != 0 )
		text += QString("width: %1;\n").arg(size.width());

	return text;
}

