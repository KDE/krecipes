/***************************************************************************
 *   Copyright (C) 2003 by krecipes.sourceforge.net authors                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "mixednumber.h"

#include <qregexp.h>

MixedNumber::MixedNumber()
{
	m_has_errors = true;
}

MixedNumber::MixedNumber( int whole, int numerator, int denominator ) :
  m_whole(whole),
  m_numerator(numerator),
  m_denominator(denominator)
{
	m_has_errors = false;
	m_input_type = InputMixedNumber;
	m_decimal = static_cast<double>(m_whole) + (static_cast<double>(m_numerator)/static_cast<double>(m_denominator));
}

MixedNumber::MixedNumber( double decimal ) :
  m_decimal(decimal)
{
	m_input_type = InputDecimal;
	m_has_errors = false;
}

MixedNumber::~MixedNumber()
{
}

int MixedNumber::getNumerator( const QString &input, int space_index, int slash_index, bool *ok )
{
	return input.mid( space_index+1, slash_index-space_index-1 ).toInt(ok);
}

int MixedNumber::getDenominator( const QString &input, int slash_index, bool *ok )
{
	return input.mid( slash_index+1, input.length() ).toInt(ok);
}

MixedNumber MixedNumber::fromString( const QString &input, bool *ok )
{
	if ( input.contains( QRegExp("[a-z]",false) ) )
	{
		if (ok){*ok = false;}
		return MixedNumber();
	}

	bool num_ok;

	int whole;
	int numerator;
	int denominator;

	int space_index = input.find(" ");
	int slash_index = input.find("/");

	if ( space_index == -1 )
	{
		if ( slash_index == -1 ) //input contains no fractional part
		{
			double decimal = input.toDouble(&num_ok);

			if ( !num_ok ){ if (ok){*ok = false;} return MixedNumber(); }

			if(ok){*ok = false;}
			return MixedNumber(decimal);
		}
		else //input just contains a fraction
		{
			whole = 0;

			numerator = MixedNumber::getNumerator( input, space_index, slash_index, &num_ok );
			if ( !num_ok ) {if (ok){*ok = false;} return MixedNumber();;}

			denominator = MixedNumber::getDenominator( input, slash_index, &num_ok );
			if ( !num_ok ) {if (ok){*ok = false;} return MixedNumber();;}

			if (ok){*ok = true;}
			return MixedNumber(whole,numerator,denominator);
		}
		if (ok){*ok = false;}
		return MixedNumber();
	}

	whole = input.mid(0,space_index).toInt(&num_ok);
	if ( !num_ok ) {if (ok){*ok = false;} return MixedNumber();;}

	numerator = MixedNumber::getNumerator( input, space_index, slash_index, &num_ok );
	if ( !num_ok ) {if (ok){*ok = false;} return MixedNumber();;}

	denominator = MixedNumber::getDenominator( input, slash_index, &num_ok );
	if ( !num_ok ) {if (ok){*ok = false;} return MixedNumber();;}

	if (ok){*ok = true;}
	return MixedNumber(whole,numerator,denominator);
}

QString MixedNumber::toString()
{

}

MixedNumber MixedNumber::operator+( const MixedNumber &fraction )
{
	if ( m_input_type == InputMixedNumber )
	{
		m_numerator = (m_numerator * fraction.m_denominator) + (m_denominator * fraction.m_numerator);
		m_denominator = m_denominator * fraction.m_denominator;
		m_whole += fraction.m_whole;
		simplify();
	}

	m_decimal += fraction.m_decimal;

	return *this;
}

void MixedNumber::simplify()
{

}

double MixedNumber::decimal()
{
	return m_decimal;
}

