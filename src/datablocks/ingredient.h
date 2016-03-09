/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2006 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#ifndef INGREDIENT_H
#define INGREDIENT_H

#include <QString>
#include <QtCore/QLinkedList>

#include "datablocks/unit.h"
#include "datablocks/elementlist.h"

#include "deprecated.h"

class MixedNumberRange;

class QDebug;

//###: Is there a better way to get the behavior of a list of Ingredient
//     objects as a data member of Ingredient?
class IngredientData
{
public:
	IngredientData();
	IngredientData( const QString &name, double amount, const Unit &units, int unitID = -1, int ingredientID = -1 );

	int ingredientID;
	QString name;

	/* Deprecated because at some point we should stop to store
	   amounts as a couple of doubles.
	   Please use amountRange() and setAmountRange() instead of
	   accessing this fields directly. */
	/* KRECIPES_DEPRECATED */ double amount;
	/* KRECIPES_DEPRECATED */ double amount_offset;

	Unit units;
	int groupID;
	QString group;
	ElementList prepMethodList;

	/** Compare two elements by their id */
	bool operator==( const IngredientData & ) const;

	MixedNumberRange amountRange() const;
	void setAmountRange( const MixedNumberRange & value );

};
QDebug operator<<( QDebug dbg, const IngredientData & ing );


/**
@author Unai Garro
*/
class Ingredient : public IngredientData
{
public:
	Ingredient();
	Ingredient( const QString &name, double amount, const Unit &units, int unitID = -1, int ingredientID = -1 );
	Ingredient( const IngredientData& );

	typedef QLinkedList<IngredientData> SubstitutesList;
	SubstitutesList substitutes;

	/* Deprecated because at some point we should stop to store
	   amounts as a couple of doubles. */
	/* KRECIPES_DEPRECATED */ void setAmount( const QString &range, bool *ok = 0 );
	/* KRECIPES_DEPRECATED */ QString amountString(bool forceFloatFormat = false ) const;
	QString amountUnitString() const;

	/** Compare two elements by their id and groupID */
	bool operator==( const Ingredient & ) const;

	/** This is used for sorting, and so we compare by name */
	bool operator<( const Ingredient & ) const;

};
QDebug operator<<( QDebug dbg, const Ingredient & ing );

#endif
