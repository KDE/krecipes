/***************************************************************************
*   Copyright (C) 2003 by Unai Garro                                      *
*   ugarro@users.sourceforge.net                                          *
*                                                                         *
*   Copyright (C) 2006 Jason Kivlighn <jkivlighn@gmail.com>               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#ifndef INGREDIENT_H
#define INGREDIENT_H

#include <QString>
#include <q3valuelist.h>

#include "datablocks/unit.h"
#include "datablocks/elementlist.h"

//###: Is there a better way to get the behavior of a list of Ingredient
//     objects as a data member of Ingredient?
class IngredientData
{
public:
	IngredientData();
	IngredientData( const QString &name, double amount, const Unit &units, int unitID = -1, int ingredientID = -1 );

	int ingredientID;
	QString name;
	double amount;
	double amount_offset;
	Unit units;
	int groupID;
	QString group;
	ElementList prepMethodList;

	/** Compare two elements by their id */
	bool operator==( const IngredientData & ) const;
};

/**
@author Unai Garro
*/
class Ingredient : public IngredientData
{
public:
	Ingredient();
	Ingredient( const QString &name, double amount, const Unit &units, int unitID = -1, int ingredientID = -1 );
	Ingredient( const IngredientData& );

	Q3ValueList<IngredientData> substitutes;

	void setAmount( const QString &range, bool *ok = 0 );

	/** Compare two elements by their id and groupID */
	bool operator==( const Ingredient & ) const;

	/** This is used for sorting, and so we compare by name */
	bool operator<( const Ingredient & ) const;
};

#endif
