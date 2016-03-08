/***************************************************************************
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                  *
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>      *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#include "fractioninput.h"

#include "datablocks/ingredient.h"

#include <QTimer>


FractionInput::FractionInput( QWidget *parent, MixedNumber::Format format ) : KLineEdit( parent ),
	m_allowRange(false),
	m_format(format)
{
	//setAlignment( Qt::AlignRight );

	setValidator( &m_numberValidator );
}

FractionInput::~FractionInput()
{
}

void FractionInput::setAllowRange( bool allowRange )
{
	m_allowRange = allowRange;
	if ( allowRange ) {
		setValidator( &m_rangeValidator );
	} else {
		setValidator( &m_numberValidator );
	}
}

void FractionInput::setValue( double d, double amount_offset )
{
	MixedNumber m( d );
	setValue( m, amount_offset );
}

void FractionInput::setValue( const MixedNumber &m, double amount_offset )
{
	QString text = m.toString( m_format );
	if ( amount_offset > 0 ) {
		text += '-' + MixedNumber(m+amount_offset).toString( m_format );
	}
	setText(text);
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
	int pos = 0;
	QValidator::State state;
	QString text = this->text();
	state = validator()->validate( text, pos );
	return ( state != QValidator::Invalid );
}

bool FractionInput::isEmpty() const
{
	return text().isEmpty();
}

#include "fractioninput.moc"
