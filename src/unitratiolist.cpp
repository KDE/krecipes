/***************************************************************************
*   Copyright (C) 2003 by Unai Garro                                      *
*   ugarro@users.sourceforge.net                                          *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#include "unitratiolist.h"
#include "klocale.h"


UnitRatioList::UnitRatioList()
{}


UnitRatioList::~UnitRatioList()
{}

double UnitRatioList::getRatio( int uid1, int uid2 )
{
	if ( uid1 == uid2 )
		return ( 1.0 );
	else {
		for ( UnitRatioList::const_iterator ur_it = begin();ur_it != end(); ++ur_it ) {
			if ( ( *ur_it ).uID1 == uid1 && ( *ur_it ).uID2 == uid2 )
				return ( ( *ur_it ).ratio );
			else if ( ( *ur_it ).uID1 == uid2 && ( *ur_it ).uID2 == uid1 )
				return ( 1.0 / ( *ur_it ).ratio );
		}
		return ( -1.0 );
	}
}
