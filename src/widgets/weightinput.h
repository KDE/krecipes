/***************************************************************************
*   Copyright © 2006 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef WEIGHTINPUT_H
#define WEIGHTINPUT_H

#include "widgets/amountunitinput.h"
#include "datablocks/unit.h"

class RecipeDB;
class PrepMethodComboBox;
class Element;
class Weight;

class WeightInput : public AmountUnitInput
{
Q_OBJECT

public:
	WeightInput( QWidget *parent, RecipeDB *database, Unit::Type type = Unit::All, MixedNumber::Format f = MixedNumber::MixedNumberFormat );

	Element prepMethod() const;
	void setPrepMethod( const Element & );

public slots:
	void emitValueChanged();

signals:
	void valueChanged( const Weight & );

private:
	PrepMethodComboBox *prepMethodBox;

	RecipeDB *m_database;
};
#endif
