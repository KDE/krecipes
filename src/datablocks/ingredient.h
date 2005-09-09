/***************************************************************************
*   Copyright (C) 2003 by Unai Garro                                      *
*   ugarro@users.sourceforge.net                                          *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#ifndef INGREDIENT_H
#define INGREDIENT_H

#include <qstring.h>

#include "datablocks/unit.h"
#include "datablocks/elementlist.h"

/**
@author Unai Garro
*/
class Ingredient
{
public:
	Ingredient();
	Ingredient( const QString &name, double amount, const Unit &units, int unitID = -1, int ingredientID = -1 );
	~Ingredient();

	int ingredientID;
	QString name;
	double amount;
	double amount_offset;
	int unitID;
	Unit units;
	int groupID;
	QString group;

	ElementList prepMethodList;

	void setAmount( const QString &range, bool *ok = 0 );

	/** Compare two elements by their id */
	bool operator==( const Ingredient & ) const;

	/** This is used for sorting, and so we compare by name */
	bool operator<( const Ingredient & ) const;
};

#endif
