/***************************************************************************
*   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#include "datablocks/elementlist.h"
#include <QStringList>


ElementList::ElementList() : QList <Element>()
{}

ElementList::~ElementList()
{}

Element ElementList::getElement( int index ) const
{
	return at( index );
}

Element ElementList::findByName( const QString &name ) const
{
	ElementList::const_iterator it_end = end();
	for ( ElementList::const_iterator it = begin(); it != it_end; ++it ) {
		if ( ( *it ).name == name )
			return * it;
	}

	Element el;
	el.id = -1;
	return el;
}

Element ElementList::findByName( const QRegExp &rx ) const
{
	ElementList::const_iterator it_end = end();
	for ( ElementList::const_iterator it = begin(); it != it_end; ++it ) {
		if ( rx.exactMatch( it->name ) )
			return * it;
	}

	Element el;
	el.id = -1;
	return el;
}

bool ElementList::containsId( int id ) const // Search by id (which uses search by item, with comparison defined on header)
{
	if ( id == -1 ) {
		return count() == 0;
	}

	Element i;
	i.id = id;
	return contains(i);
}

bool ElementList::containsSubSet( ElementList &el )
{
	ElementList::const_iterator it_end = el.constEnd();
	ElementList::const_iterator it;

	for ( it = el.constBegin(); it != it_end; ++it ) {
		if ( !containsId( ( *it ).id ) )
			return false;
	}
	return true;
}

QString ElementList::join( const QString &sep ) const
{
	QString ret;

	ElementList::const_iterator it_end = end();
	ElementList::const_iterator it;

	for ( it = begin(); it != it_end; ++it ) {
		if ( it != begin() )
			ret += sep;
		ret += (*it).name;
	}

	return ret;
}

ElementList ElementList::split( const QString &sep, const QString &str )
{
	ElementList ret;
	QStringList list;
   if (str.isEmpty())
      list = QStringList();
   else
      list = str.split( sep, QString::SkipEmptyParts);

	QStringList::const_iterator it_end = list.constEnd();
	QStringList::const_iterator it;

	for ( it = list.constBegin(); it != it_end; ++it ) {
		ret.append( Element((*it).trimmed()) );
	}

	return ret;
}
