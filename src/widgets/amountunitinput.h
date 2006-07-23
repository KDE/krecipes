/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*                                                                         *
*   Copyright (C) 2006 Jason Kivlighn (jkivlighn@gmail.com)               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef AMOUNTUNITINPUT_H
#define AMOUNTUNITINPUT_H

#include <qhbox.h>

#include "datablocks/unit.h"
#include "datablocks/mixednumber.h"

class QListViewItem;

class RecipeDB;
class FractionInput;
class UnitComboBox;

class AmountUnitInput : public QHBox
{
Q_OBJECT

public:
	AmountUnitInput( QWidget *parent, RecipeDB *database, Unit::Type type = Unit::All, MixedNumber::Format f = MixedNumber::MixedNumberFormat );

	void setAmount( const MixedNumber &amount );
	void setUnit( const Unit &unit );

	MixedNumber amount() const;
	Unit unit() const;
	QListViewItem *item() const { return m_item; }
	void setItem( QListViewItem *it ){ m_item = it; }

	void insertIntoListview( QListViewItem *it, int col );

public slots:
	void emitValueChanged();

signals:
	void valueChanged( const MixedNumber &, const Unit &unit );
	void doneEditing();

private:
	FractionInput *amountInput;
	UnitComboBox *unitBox;

	QListViewItem *m_item;

	RecipeDB *m_database;
};
#endif