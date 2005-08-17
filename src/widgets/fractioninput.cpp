/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "fractioninput.h"

#include "datablocks/ingredient.h"

FractionInput::FractionInput( QWidget *parent, const char *name ) : KLineEdit( parent, name ),
	m_allow_range(false)
{}

FractionInput::~FractionInput()
{}

void FractionInput::setValue( double d, double amount_offset )
{
	MixedNumber m( d );
	QString offset_str = MixedNumber(m+amount_offset).toString(MixedNumber::MixedNumberFormat);
	setText( m.toString( MixedNumber::MixedNumberFormat ) + "-" + offset_str );
}

void FractionInput::setValue( const MixedNumber &m, double amount_offset )
{
	QString offset_str = MixedNumber(m+amount_offset).toString(MixedNumber::MixedNumberFormat);
	setText( m.toString( MixedNumber::MixedNumberFormat ) + "-" + offset_str );
}

void FractionInput::value( MixedNumber &amount, double &amount_offset ) const
{
	Ingredient i; i.setAmount( text() );

	amount = MixedNumber(i.amount);
	amount_offset = i.amount_offset;
}

void FractionInput::value( double &amount, double &amount_offset ) const
{
	Ingredient i; i.setAmount( text() );

	amount = i.amount;
	amount_offset = i.amount_offset;
}

MixedNumber FractionInput::value() const
{
	Ingredient i; i.setAmount( text() );

	return MixedNumber(i.amount);
}

MixedNumber FractionInput::minValue() const
{
	Ingredient i; i.setAmount( text() );

	return MixedNumber(i.amount);
}

MixedNumber FractionInput::maxValue() const
{
	Ingredient i; i.setAmount( text() );

	return MixedNumber(i.amount_offset+i.amount);
}

bool FractionInput::isInputValid() const
{
	if ( !m_allow_range && text().contains("-") )
		return false;

	bool ok;
	Ingredient i; i.setAmount( text(), &ok );

	return ok;
}
