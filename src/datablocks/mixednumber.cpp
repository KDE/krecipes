/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                         *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>               *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                  *
*   Copyright © 2009-2016 José Manuel Santamaría Lema <panfaust@gmail.com> *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/
#include "mixednumber.h"

#include <QRegExp>

#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>
#include <KConfigGroup>

#include <QIntValidator>
#include <QDoubleValidator>


QString beautify( const QString &num )
{
	QString copy( num );
	copy.remove( QRegExp( QString( "(%1){0,1}0+$" ).arg( QRegExp::escape( KGlobal::locale() ->decimalSymbol() ) ) ) );
	return copy;
}

MixedNumber::MixedNumber() :
		m_isValid( true ),
		m_whole( 0 ),
		m_numerator( 0 ),
		m_denominator( 1 ),
		locale( KGlobal::locale() )
{}

MixedNumber::MixedNumber( int whole, int numerator, int denominator ) :
		m_whole( whole ),
		m_numerator( numerator ),
		locale( KGlobal::locale() )
{
	setDenominator( denominator );
}

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

	m_isValid = true;
	m_numerator = high.numerator();
	m_denominator = high.denominator();
	m_whole = intPart;
}

MixedNumber::~MixedNumber()
{}

bool MixedNumber::isValid() const
{
	return m_isValid;
}

int MixedNumber::whole() const
{
	return m_whole;
}

void MixedNumber::setWhole( int n )
{
	m_whole = n;
}

int MixedNumber::numerator() const
{
	return m_numerator;
}

void MixedNumber::setNumerator( int n )
{
	m_numerator = n;
}

int MixedNumber::denominator() const
{
	return m_denominator;
}

void MixedNumber::setDenominator( int n )
{
	if ( n == 0 ) {
		m_isValid = false;
	} else {
		m_isValid = true;
	}
	m_denominator = n;
}

QValidator::State MixedNumber::getNumerator( const QString &input, int space_index, int slash_index, int &result )
{
	//Prepare the int validator.
	QIntValidator int_validator;
	int_validator.setBottom( 0 );
	/*QLocale validatorLocale = int_validator.locale();
	validatorLocale.setNumberOptions( QLocale::RejectGroupSeparator | QLocale::OmitGroupSeparator );
	int_validator.setLocale( validatorLocale );*/
	int_validator.setLocale( QLocale::c() ); //Workaround: the code above doesn't work

	//Read the number.
	QString buffer = input.mid( space_index + 1, slash_index - space_index - 1 );
	result = buffer.toInt();

	//Return the validator state.
	int pos = 0; //we are going to ignore this parameter
	return int_validator.validate(buffer, pos);
}

QValidator::State MixedNumber::getDenominator( const QString &input, int slash_index, int &result)
{
	//Prepare the int validator.
	QIntValidator int_validator;
	int_validator.setBottom( 0 );
	/*QLocale validatorLocale = int_validator.locale();
	validatorLocale.setNumberOptions( QLocale::RejectGroupSeparator | QLocale::OmitGroupSeparator );
	int_validator.setLocale( validatorLocale );*/
	int_validator.setLocale( QLocale::c() ); //Workaround: the code above doesn't work

	//Read the number.
	QString buffer = input.mid( slash_index + 1 );
	result = buffer.toInt();

	//Return the validator state.
	int pos = 0; //we are going to ignore this parameter
	return int_validator.validate(buffer, pos);
}

QValidator::State MixedNumber::fromString( const QString &str, MixedNumber &result, bool locale_aware )
{
	QString input = str.trimmed();
	if ( input.isEmpty() ) {
		result.m_isValid = false;
		return QValidator::Intermediate;
	}

	bool num_ok;

	int whole;
	int numerator;
	int denominator;

	if ( input.count(" ") > 1 ) {
		result.m_isValid = false;
		return QValidator::Invalid;
	}
	if ( input.count("/") > 1 ) {
		result.m_isValid = false;
		return QValidator::Invalid;
	}

	int space_index = input.indexOf( " " );
	int slash_index = input.indexOf( "/" );

	if ( (space_index == -1) && (slash_index == -1) ) {  //input contains no fractional part

		QDoubleValidator double_validator(0);
		double_validator.setBottom( 0.0 );
		double_validator.setNotation( QDoubleValidator::StandardNotation );
		//Should return Intermediate or Invalid, whichever it's appropiate.
		int pos = 0; //we are going to ignore this parameter
		QValidator::State state = double_validator.validate( input, pos );
		result.m_isValid = (state == QValidator::Acceptable);

		//If the string input is fine, read the number
		if ( state == QValidator::Acceptable ) {
			double value = input.toDouble( &num_ok );
			if ( !num_ok ) {
				result.m_isValid = false;
				return QValidator::Invalid;
			}
			result = MixedNumber( value );
		}
		return state;

	} else if ( (space_index == -1) && (slash_index != -1) ) {  //input just contains a fraction
		if ( input.endsWith( "/" ) )
		{
			result.m_isValid = false;
			return QValidator::Intermediate;
		}

		whole = 0;

		QValidator::State num_state;
		num_state = MixedNumber::getNumerator( input, space_index, slash_index, numerator );
		kDebug() << num_state;
		if ( num_state != QValidator::Acceptable ) {
			result.m_isValid = false;
			return QValidator::Invalid;
		}

		QValidator::State den_state;
		den_state = MixedNumber::getDenominator( input, slash_index, denominator );
		qDebug() << den_state;
		if ( den_state == QValidator::Acceptable ) {
			if ( denominator == 0 ) {
				result.m_isValid = false;
				return QValidator::Invalid;
			} else {
				result = MixedNumber( whole, numerator, denominator );
				return QValidator::Acceptable;
			}
		} else {
			result.m_isValid = false;
			return den_state;
		}
	} else if ( (space_index != -1) && (slash_index == -1) ) {  //input contains an incomplete mixed number
		whole = input.mid( 0, space_index ).toInt( &num_ok );
		if ( !num_ok ) {
			result.m_isValid = false;
			return QValidator::Invalid;
		} else {
			QIntValidator int_validator;
			QValidator::State state;
			int pos = 0; //we are going to ignore this parameter
			QString buffer = input.mid(space_index+1);
			state = int_validator.validate( buffer, pos );
			if ( state == QValidator::Invalid ){
				result.m_isValid = false;
				return QValidator::Invalid;
			} else {
				result.m_isValid = false;
				return QValidator::Intermediate;
			}
		}
	} else if ( (space_index != -1) && (slash_index != -1) ) { //input contains a mixed number

		if ( space_index > slash_index ) {
			result.m_isValid = false;
			return QValidator::Invalid;
		}

		if ( input.endsWith( "/" ) )
		{
			result.m_isValid = false;
			return QValidator::Intermediate;
		}

		whole = input.mid( 0, space_index ).toInt( &num_ok );
		if ( !num_ok ) {
			result.m_isValid = false;
			return QValidator::Invalid;
		}

		QValidator::State num_state;
		num_state = MixedNumber::getNumerator( input, space_index, slash_index, numerator );
		if ( num_state != QValidator::Acceptable ) {
			result.m_isValid = false;
			return QValidator::Invalid;
		}

		QValidator::State den_state;
		den_state = MixedNumber::getDenominator( input, slash_index, denominator );
		if ( denominator == 0 ) {
			result.m_isValid = false;
			return QValidator::Invalid;
		} else {
			result = MixedNumber( whole, numerator, denominator );
			return den_state;
		}
	}
	result.m_isValid = false;
	return QValidator::Invalid;
}

bool MixedNumber::isFraction( const QString &input )
{
	return input.contains("/");
}

QString MixedNumber::toString( Format format, bool locale_aware ) const
{
	if ( format == AutoFormat ) {
		format = configuredFormat();
	}

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

QString MixedNumber::toString( bool locale_aware ) const
{
	return toString( MixedNumber::AutoFormat, locale_aware );
}

MixedNumber::Format MixedNumber::configuredFormat()
{
	KConfigGroup config = KGlobal::config()->group( "Formatting" );
	bool useFraction = config.readEntry( "Fraction", false );
	MixedNumber::Format result;
	if ( useFraction ) {
		result = MixedNumber::MixedNumberFormat;
	} else {
		result = MixedNumber::DecimalFormat;
	}
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

bool MixedNumber::operator>( const MixedNumber & other ) const
{
	return ( toDouble() > other.toDouble() );
}

void MixedNumber::simplify()
{
	int divisor = gcd( m_numerator, m_denominator );
	m_numerator /= divisor;
	m_denominator /= divisor;
}

double MixedNumber::toDouble() const
{
	if ( m_isValid ) {
		return static_cast<double>( m_whole )
		+ ( static_cast<double>( m_numerator ) / static_cast<double>( m_denominator ) );
	} else {
		return 0.0;
	}
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
