/***************************************************************************
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "fractioninput.h"

#include <QTimer>

#include <kglobalsettings.h>

#include "datablocks/ingredient.h"

FractionInput::FractionInput( QWidget *parent, MixedNumber::Format format ) : KLineEdit( parent ),
	m_allowRange(false),
	m_validateTimer(new QTimer(this)),
	m_format(format)
{
	//setAlignment( Qt::AlignRight );

	connect( this, SIGNAL(textChanged(const QString&)), this, SLOT(slotStartValidateTimer()) );
	connect( m_validateTimer, SIGNAL(timeout()), this, SLOT(validate()) );
}

FractionInput::~FractionInput()
{
	delete m_validateTimer;
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
	if ( !m_allowRange && text().contains("-") )
		return false;

	bool ok;
	Ingredient i; i.setAmount( text(), &ok );

	return ok;
}

void FractionInput::slotStartValidateTimer()
{
	if ( !m_validateTimer->isActive() )
		m_validateTimer->setSingleShot( true );
		m_validateTimer->start( 1000 );

	if ( isInputValid() )
		emit valueChanged( value() );
}

void FractionInput::validate()
{
	QPalette p = palette();

	if ( isInputValid() ) {
		//KDE4 port setPaletteForegroundColor( KGlobalSettings::textColor() );
		p.setColor( foregroundRole(), QPalette::Text );	
	}
	else
		p.setColor( foregroundRole(), Qt::red );	
	setPalette(p);
}

bool FractionInput::isEmpty() const
{
	return text().isEmpty();
}

#include "fractioninput.moc"
