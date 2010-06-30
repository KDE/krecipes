/***************************************************************************
*   Copyright © 2006 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "amountunitinput.h"

#include <q3header.h>
#include <q3listview.h>

#include "fractioninput.h"
#include "unitcombobox.h"
#include "backends/recipedb.h"
#include "datablocks/mixednumber.h"

AmountUnitInput::AmountUnitInput( QWidget *parent, RecipeDB *database, Unit::Type type, MixedNumber::Format format )
	: KHBox(parent),
	m_item(0), m_database(database)
{
	amountInput = new FractionInput(this,format);
	unitBox = new UnitComboBox(this,database,type);
	unitBox->reload();

	connect( amountInput, SIGNAL(valueChanged(const MixedNumber &)), SLOT(emitValueChanged()) );
	connect( unitBox, SIGNAL(activated(int)), SLOT(emitValueChanged()) );
	connect( amountInput, SIGNAL(returnPressed()), SIGNAL(doneEditing()) );
}

void AmountUnitInput::emitValueChanged()
{
	emit valueChanged( amount(), unit() );
}

void AmountUnitInput::setAmount( const MixedNumber &amount )
{
	amountInput->disconnect( this );
	if ( amount.toDouble() < 0 )
		amountInput->clear();
	else
		amountInput->setValue( amount, 0 );
	connect( amountInput, SIGNAL(valueChanged(const MixedNumber &)), SLOT(emitValueChanged()) );
}

void AmountUnitInput::setUnit( const Unit &unit )
{
	if ( unit.id() == -1 )
		unitBox->setCurrentItem(0);
	else
		unitBox->setSelected( unit.id() );

}

MixedNumber AmountUnitInput::amount() const
{
	return amountInput->value();
}

Unit AmountUnitInput::unit() const
{
	//TODO Potential for optimization here... avoid the database call
	return m_database->unitName( unitBox->id( unitBox->currentIndex() ) );
}

bool AmountUnitInput::isInputValid() const
{
	return amountInput->isInputValid();
}

#include "amountunitinput.moc"
