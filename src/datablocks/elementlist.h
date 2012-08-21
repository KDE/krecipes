/***************************************************************************
*   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#ifndef ELEMENTLIST_H
#define ELEMENTLIST_H

#include <QList>
#include <QRegExp>
#include <QMetaType>

#include "element.h"

/**
@author Unai Garro
*/
class ElementList: public QList<Element>
{
public:
	ElementList();
	~ElementList();

	bool containsId( int id ) const;
	bool containsSubSet( ElementList &el );

	Element findByName( const QString & ) const;
	Element findByName( const QRegExp & ) const;

	Element getElement( int index ) const;

	QString join( const QString &sep ) const;

	static ElementList split( const QString &sep, const QString &str );
};

Q_DECLARE_METATYPE(ElementList);

typedef QList<int> IDList;

#endif
