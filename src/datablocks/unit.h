/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef UNIT_H
#define UNIT_H

#include <qstring.h>
#include <qvaluelist.h>

class Unit
{
public:
	Unit();
	Unit( const QString &name, const QString &plural, int id = -1 );

	/** Use @param amount to determine whether to use @param name as the plural or singlular form */
	Unit( const QString &name, double amount );

	bool operator==( const Unit &u ) const;

	int id;
	QString name;
	QString plural;
};

typedef QValueList< Unit > UnitList;

#endif //UNIT_H
