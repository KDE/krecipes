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
	//treat QString::null and "" as the same
	QString plural_test1 = u.plural.lower();
	if ( plural_test1.isNull() )
		plural_test1 = "";

	QString plural_test2 = name.lower();
	if ( plural_test2.isNull() )
		plural_test2 = "";

	QString single_test1 = u.name.lower();
	if ( single_test1.isNull() )
		single_test1 = "";

	QString single_test2 = name.lower();
	if ( single_test2.isNull() )
		single_test2 = "";

	return plural_test1 == plural_test2 || single_test1 == single_test2;
}
