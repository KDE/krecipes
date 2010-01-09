/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "element.h"

Element::Element() :
		id( -1 )
{}

Element::Element( const QString &_name, int _id ) :
		name( _name ),
		id( _id )
{}

Element::Element( const Element &el )
{
	id = el.id;
	name = el.name;
}

Element::~Element()
{}

Element& Element::operator=( const Element &el )
{
	id = el.id;
	name = el.name;
	return *this;
}

bool Element::operator==( const Element &el ) const
{
	return ( el.id == id );
}
