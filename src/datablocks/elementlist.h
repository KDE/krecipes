/***************************************************************************
*   Copyright (C) 2003-2004 by                                            *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#ifndef ELEMENTLIST_H
#define ELEMENTLIST_H

#include <q3ptrlist.h>
#include <q3valuelist.h>
#include <qregexp.h>

#include "element.h"

/**
@author Unai Garro
*/
class ElementList: public Q3ValueList<Element>
{
public:
	ElementList();
	~ElementList();

	bool containsId( int id ) const;
	bool containsSubSet( ElementList &el );

	Element findByName( const QString & ) const;
	Element findByName( const QRegExp & ) const;

	Element getElement( int index ) const;
};


class IDList: public Q3PtrList <int>
{
public:
	IDList()
	{
		setAutoDelete( true );
	};
	~IDList()
	{}
	;
protected:
	virtual int compareItems( Q3PtrCollection::Item item1, Q3PtrCollection::Item item2 )
	{
		return ( *( ( int* ) item1 ) - *( ( int* ) item2 ) );
	};

};

#endif
