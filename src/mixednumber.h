/***************************************************************************
 *   Copyright (C) 2003 by krecipes.sourceforge.net authors                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef MIXEDNUMBER_H
#define MIXEDNUMBER_H

#include <qstring.h>

/** A class to hold a mixed number.
  * @author Jason Kivlighn
  */
class MixedNumber
{
public:
	MixedNumber( int whole, int numerator, int denominator );

	/** Create a fraction from the given @param decimal.  This uses a method that will
	  * approximate the actual fraction with precision equal to @param precision.
	  * The closer @param precision is to zero without being zero, the more precisely
	  * it will try to approximate the fraction.
	  */
	MixedNumber( double decimal, double precision = 1e-6 );
	MixedNumber();
	~MixedNumber();

	MixedNumber operator+( const MixedNumber &fraction );

	enum Format { DecimalFormat, MixedNumberFormat };

	/** The input as a decimal. */
	double toDouble();

	/** Returns the fraction as a string */
	QString toString( Format = MixedNumberFormat );

	/** The whole part of the input */
	int whole(){ return m_whole; }

	/** The numerator of the fractional part of the input. */
	int numerator(){ return m_numerator; }

	/** The denominator of the fractional part of the input. */
	int denominator(){ return m_denominator; }

	void setNumerator( int n ){ m_numerator = n; }
	void setDenominator( int d ){ m_denominator = d; }

	/** Simplify the fraction to its lowest terms */
	void simplify();

	static MixedNumber fromString( const QString &input, bool *ok = 0 );

private:
	static int getNumerator( const QString &input, int space_index, int slash_index, bool *ok );
	static int getDenominator( const QString &input, int slash_index, bool *ok );

	int gcd( int, int );

	int m_whole;
	int m_numerator;
	int m_denominator;
};

#endif //MIXEDNUMBER_H
