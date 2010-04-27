/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#include "mixednumber.h"

#include <QRegExp>

#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>

QString beautify( const QString &num )
{
	QString copy( num );
	copy.remove( QRegExp( QString( "(%1){0,1}0+$" ).arg( QRegExp::escape( KGlobal::locale() ->decimalSymbol() ) ) ) );
	return copy;
}

MixedNumber::MixedNumber() :
		m_whole( 0 ),
		m_numerator( 0 ),
		m_denominator( 1 ),
		locale( KGlobal::locale() )
{}

MixedNumber::MixedNumber( int whole, int numerator, int denominator ) :
		m_whole( whole ),
		m_numerator( numerator ),
		m_denominator( denominator ),
		locale( KGlobal::locale() )
{}

MixedNumber::MixedNumber( double decimal, double precision ) :
		locale( KGlobal::locale() )
{
	// find nearest fraction
	int intPart = static_cast<int>( decimal );
	decimal -= static_cast<double>( intPart );

	MixedNumber low( 0, 0, 1 );     // "A" = 0/1
	MixedNumber high( 0, 1, 1 );    // "B" = 1/1

	for ( int i = 0; i < 100; ++i ) {
		double testLow = low.denominator() * decimal - low.numerator();
		double testHigh = high.numerator() - high.denominator() * decimal;

		if ( testHigh < precision * high.denominator() )
			break; // high is answer
		if ( testLow < precision * low.denominator() ) {  // low is answer
			high = low;
			break;
		}

		if ( i & 1 ) {  // odd step: add multiple of low to high
			double test = testHigh / testLow;
			int count = ( int ) test; // "N"
			int num = ( count + 1 ) * low.numerator() + high.numerator();
			int denom = ( count + 1 ) * low.denominator() + high.denominator();

			if ( ( num > 0x8000 ) || ( denom > 0x10000 ) )
				break;

			high.setNumerator( num - low.numerator() ); // new "A"
			high.setDenominator( denom - low.denominator() );
			low.setNumerator( num ); // new "B"
			low.setDenominator( denom );
		}
		else {  // even step: add multiple of high to low
			double test = testLow / testHigh;
			int count = ( int ) test; // "N"
			int num = low.numerator() + ( count + 1 ) * high.numerator();
			int denom = low.denominator() + ( count + 1 ) * high.denominator();

			if ( ( num > 0x10000 ) || ( denom > 0x10000 ) )
				break;

			low.setNumerator( num - high.numerator() ); // new "A"
			low.setDenominator( denom - high.denominator() );
			high.setNumerator( num ); // new "B"
			high.setDenominator( denom );
		}
	}

	m_numerator = high.numerator();
	m_denominator = high.denominator();
	m_whole = intPart;
}

MixedNumber::~MixedNumber()
{}

int MixedNumber::getNumerator( const QString &input, int space_index, int slash_index, bool *ok )
{
	return input.mid( space_index + 1, slash_index - space_index - 1 ).toInt( ok );
}

int MixedNumber::getDenominator( const QString &input, int slash_index, bool *ok )
{
	return input.mid( slash_index + 1, input.length() ).toInt( ok );
}

MixedNumber MixedNumber::fromString( const QString &str, bool *ok, bool locale_aware )
{
	QString input = str.trimmed();
	if ( input.isEmpty() ) {
		if ( ok ) {
			*ok = true;
		}
		return MixedNumber();
	}

	KLocale *locale = KGlobal::locale();

	bool num_ok;

	int whole;
	int numerator;
	int denominator;

	int space_index = input.indexOf( " " );
	int slash_index = input.indexOf( "/" );

	if ( space_index == -1 ) {
		if ( slash_index == -1 )  //input contains no fractional part
		{
			QString decimal_symbol = ( locale_aware ) ? locale->decimalSymbol() : ".";
			if ( input.endsWith( decimal_symbol ) )
			{
				if ( ok ) {
					*ok = false;
				}
				return MixedNumber();
			}

			double decimal = ( locale_aware ) ? locale->readNumber( input, &num_ok ) : input.toDouble( &num_ok );

			if ( !num_ok )
			{
				if ( ok ) {
					*ok = false;
				}
				return MixedNumber();
			}

			if ( ok )
			{
				*ok = true;
			}
			return MixedNumber( decimal );
		}
		else //input just contains a fraction
		{
			whole = 0;

			numerator = MixedNumber::getNumerator( input, space_index, slash_index, &num_ok );
			if ( !num_ok ) {
				if ( ok ) {
					*ok = false;
				}
				return MixedNumber();
			}

			denominator = MixedNumber::getDenominator( input, slash_index, &num_ok );
			if ( !num_ok || denominator == 0 ) {
				if ( ok ) {
					*ok = false;
				}
				return MixedNumber();
			}

			if ( ok ) {
				*ok = true;
			}
			return MixedNumber( whole, numerator, denominator );
		}
		if ( ok ) {
			*ok = false;
		}
		return MixedNumber();
	}

	whole = input.mid( 0, space_index ).toInt( &num_ok );
	if ( !num_ok ) {
		if ( ok ) {
			*ok = false;
		}
		return MixedNumber();
	}

	numerator = MixedNumber::getNumerator( input, space_index, slash_index, &num_ok );
	if ( !num_ok ) {
		if ( ok ) {
			*ok = false;
		}
		return MixedNumber();
	}

	denominator = MixedNumber::getDenominator( input, slash_index, &num_ok );
	if ( !num_ok || denominator == 0 ) {
		if ( ok ) {
			*ok = false;
		}
		return MixedNumber();
	}

	if ( ok ) {
		*ok = true;
	}
	return MixedNumber( whole, numerator, denominator );
}

bool MixedNumber::isFraction( const QString &input )
{
	return input.contains("/");
}

QString MixedNumber::toString( Format format, bool locale_aware ) const
{
	if ( format == DecimalFormat ) {
		if ( locale_aware )
			return beautify( locale->formatNumber( toDouble(), 5 ) );
		else
			return QString::number( toDouble() );
	}

	if ( m_numerator == 0 && m_whole == 0 )
		return QString( "0" );


	QString result;

	if ( m_whole != 0 ) {
		result += QString::number( m_whole );
		if ( m_numerator != 0 )
			result += ' ';
	}

	if ( m_numerator != 0 )
		result += QString::number( m_numerator ) + '/' + QString::number( m_denominator );

	return result;
}

bool MixedNumber::operator!=( const MixedNumber &fraction ) const
{
	return ( fraction.toDouble() != toDouble() );
}

MixedNumber& MixedNumber::operator+=( const MixedNumber &fraction )
{
	m_numerator = ( m_numerator * fraction.m_denominator ) + ( m_denominator * fraction.m_numerator );
	m_denominator = m_denominator * fraction.m_denominator;
	m_whole += fraction.m_whole;
	simplify();

	return *this;
}

MixedNumber& MixedNumber::operator+=( double d )
{
	MixedNumber mn(d);
	*this += mn;
	return *this;
}

void MixedNumber::simplify()
{
	int divisor = gcd( m_numerator, m_denominator );
	m_numerator /= divisor;
	m_denominator /= divisor;
}

double MixedNumber::toDouble() const
{
	return static_cast<double>( m_whole ) + ( static_cast<double>( m_numerator ) / static_cast<double>( m_denominator ) );
}

int MixedNumber::gcd( int n, int m )
{
	int r;
	while ( n != 0 ) {
		r = m % n;
		m = n;
		n = r;
	}

	return m;
}
