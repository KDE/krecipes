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

#include <cmath>

#include <qregexp.h>

#define ROUND(a) (floor((a)) - (a) < ceil((a)) - (a)) ? floor((a)) : ceil((a))

MixedNumber::MixedNumber()
{
}

MixedNumber::MixedNumber( int whole, int numerator, int denominator ) :
  m_whole(whole),
  m_numerator(numerator),
  m_denominator(denominator)
{
}

MixedNumber::MixedNumber( double decimal )
{
	QString as_string = QString::number(decimal);
	int decimal_index = as_string.find('.');
	m_whole = as_string.left( decimal_index ).toInt();

	if ( (decimal_index != -1) && (fabs(decimal-ROUND(decimal)) > 0.000001) )
	{
		QString decimal_part = as_string.mid( decimal_index+1, as_string.length() );
		qDebug("decimal_part: %s",decimal_part.latin1());
		m_numerator = decimal_part.toInt();
		m_denominator = 10 * decimal_part.length();
		simplify();
	}
	else
	{
		m_numerator = 0;
		m_denominator = 1;
	}
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
	qDebug("input: %s",input.latin1());
	//is this right?
	if ( input.contains( QRegExp("^(\\d|\\s|/|.)",false) ) ) //change ".", to "decimal" of current locale
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
qDebug("%f",decimal);
			if ( !num_ok ){ if (ok){*ok = false;} return MixedNumber(); }

			if (ok){*ok = true;}
			return MixedNumber(decimal);
		}
		else //input just contains a fraction
		{
			whole = 0;

			numerator = MixedNumber::getNumerator( input, space_index, slash_index, &num_ok );
			if ( !num_ok ) {if (ok){*ok = false;} return MixedNumber();}

			denominator = MixedNumber::getDenominator( input, slash_index, &num_ok );
			if ( !num_ok ) {if (ok){*ok = false;} return MixedNumber();}

			if (ok){*ok = true;}
			return MixedNumber(whole,numerator,denominator);
		}
		if (ok){*ok = false;}
		return MixedNumber();
	}

	whole = input.mid(0,space_index).toInt(&num_ok);
	if ( !num_ok ) {if (ok){*ok = false;} return MixedNumber();}

	numerator = MixedNumber::getNumerator( input, space_index, slash_index, &num_ok );
	if ( !num_ok ) {if (ok){*ok = false;} return MixedNumber();}

	denominator = MixedNumber::getDenominator( input, slash_index, &num_ok );
	if ( !num_ok ) {if (ok){*ok = false;} return MixedNumber();}

	if (ok){*ok = true;}
	return MixedNumber(whole,numerator,denominator);
}

QString MixedNumber::toString( Format format )
{
	QString result;

	if ( format == DecimalFormat )
		return QString::number(toDouble());

	if ( m_numerator == 0 && m_whole == 0 )
		return QString("0");

	if ( m_whole != 0 )
		result += QString::number(m_whole)+" ";

	if ( m_numerator != 0 )
		result += QString::number(m_numerator)+"/"+QString::number(m_denominator);

	return result;
}

MixedNumber MixedNumber::operator+( const MixedNumber &fraction )
{
	m_numerator = (m_numerator * fraction.m_denominator) + (m_denominator * fraction.m_numerator);
	m_denominator = m_denominator * fraction.m_denominator;
	m_whole += fraction.m_whole;
	simplify();

	return *this;
}

void MixedNumber::simplify()
{

}

double MixedNumber::toDouble()
{
	return static_cast<double>(m_whole)+(static_cast<double>(m_numerator)/static_cast<double>(m_denominator));
}

