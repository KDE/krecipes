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
#include "datablocks/mixednumberrange.h"

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

void FractionInput::setValueRange( const MixedNumberRange & value )
{
	setText( value.toString( m_format, true ) );
}

MixedNumberRange FractionInput::valueRange() const
{
	MixedNumberRange result;
	MixedNumberRange::fromString( text(), result, true );
	return result;
}

void FractionInput::setValue( double d, double amount_offset )
{
	MixedNumberRange range ( d, amount_offset );
	setValueRange( range );
}

void FractionInput::setValue( const MixedNumber &m, double amount_offset )
{
	MixedNumber second = m + amount_offset;
	MixedNumberRange range;
	range.first = m;
	range.second = range.first + amount_offset;
	setValueRange( range );
}

void FractionInput::value( MixedNumber &amount, double &amount_offset ) const
{
	MixedNumberRange range = valueRange();
	amount = range.first;
	double dummy;
	range.toAmountAndOffset( &dummy, &amount_offset );
}

void FractionInput::value( double &amount, double &amount_offset ) const
{
	MixedNumberRange range = valueRange();
	range.toAmountAndOffset( &amount, &amount_offset );
}

MixedNumber FractionInput::value() const
{
	return valueRange().first;
}

MixedNumber FractionInput::minValue() const
{
	return valueRange().first;
}

MixedNumber FractionInput::maxValue() const
{
	return valueRange().second;
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
