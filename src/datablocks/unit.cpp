/***************************************************************************
 *   Copyright (C) 2004 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
 
#include "unit.h"

Unit::Unit() :
  id(-1)
{
}

Unit::Unit( const QString &_name, const QString &_plural, int _id ) :
  id(_id),
  name(_name),
  plural(_plural)
{
}

Unit::Unit( const QString &_name, double amount )
{
	if ( amount > 1 )
		plural = _name;
	else
		name = _name;
}

bool Unit::operator==( const Unit &u ) const
{
	return u.plural.lower() == plural.lower() || u.name.lower() == name.lower();
}
