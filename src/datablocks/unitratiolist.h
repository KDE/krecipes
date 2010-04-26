/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#ifndef UNITRATIOLIST_H
#define UNITRATIOLIST_H
#include <QtCore/QList>
#include "unitratio.h"


/**
@author Unai Garro
*/
class UnitRatioList : public QList <UnitRatio>
{
public:
	UnitRatioList();
	~UnitRatioList();

	void add
		( const UnitRatio &r )
	{
		append( r );
	}
	double getRatio( int uid1, int uid2 );
};

#endif
