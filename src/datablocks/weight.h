/***************************************************************************
*   Copyright © 2006 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef WEIGHT_H
#define WEIGHT_H

#include <QString>
#include <QList>

#include "datablocks/elementlist.h"

class Weight
{
public:
	Weight();

	int id;
	int ingredientID;
	int perAmountUnitID;
	QString perAmountUnit;
	double perAmount;
	int weightUnitID;
	double weight;
	QString weightUnit;
	int prepMethodID;
	QString prepMethod;
};

typedef QList<Weight> WeightList;

#endif
