/***************************************************************************
 *   Copyright (C) 2003 by krecipes.sourceforge.net authors                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef FRACTION_H
#define FRACTION_H

#include <qstring.h>

/** A class to hold a mixed number.
  * @author Jason Kivlighn
  */
class MixedNumber
{
public:
	MixedNumber( int whole, int numerator, int denominator );
	MixedNumber( double decimal );
	MixedNumber();
	~MixedNumber();

	MixedNumber operator+( const MixedNumber &fraction );

	enum Format { DecimalFormat, MixedNumberFormat };

	/** The input as a decimal.  This can be used with either @ref InputType. */
	double toDouble();

	/** Returns the fraction as a string */
	QString toString( Format );

	/** The whole part of the input when the inputType() is a fraction. */
	int whole(){ return m_whole; }

	/** The numerator of the fractional part of the input when @ref inputType() is a fraction. */
	int numerator(){ return m_numerator; }

	/** The denominator of the fractional part of the input when @ref inputType() is a fraction. */
	int denominator(){ return m_denominator; }

	/** Simplify the fraction to its lowest terms */
	void simplify();

	static MixedNumber fromString( const QString &input, bool *ok = 0 );

private:
	static int getNumerator( const QString &input, int space_index, int slash_index, bool *ok );
	static int getDenominator( const QString &input, int slash_index, bool *ok );

	int m_whole;
	int m_numerator;
	int m_denominator;
};

#endif //FRACTION_H
