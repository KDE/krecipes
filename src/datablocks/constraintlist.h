/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#ifndef CONSTRAINTLIST_H
#define CONSTRAINTLIST_H

#include <QString>
#include <QList>

#include "unit.h"

/**
@author Unai Garro
*/


class Constraint
{
public:
	Constraint()
	{
		max = 0.0;
		min = 0.0;
		id = -1;
		enabled = false;
	}

	~Constraint(){}

	int id;
	QString name;
	QString units;
	Unit perUnit; // stores the unit ID, name, and type
	double max;
	double min;
	bool enabled;
};

typedef QList< Constraint > ConstraintList;

#endif
