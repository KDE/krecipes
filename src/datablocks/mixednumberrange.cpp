/***************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>      *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#include "mixednumberrange.h"

#include <QStringList>

MixedNumberRange::MixedNumberRange():
	QPair<MixedNumber,MixedNumber>()
{
}

MixedNumberRange::MixedNumberRange( const MixedNumber &value1, 
		const MixedNumber &value2 ):
	QPair<MixedNumber,MixedNumber>(value1,value2)
{
}

MixedNumberRange::~MixedNumberRange()
{
}

QValidator::State MixedNumberRange::fromString( const QString & input,
	MixedNumberRange & result, bool locale_aware )
{
	//Split the input string
	QStringList tokens = input.split('-');
	int tokensCount = tokens.count();
	QString number1, number2;

	if ( tokensCount == 2 ) {
		//Get the tokens
		number1 = tokens[0].trimmed();
		number2 = tokens[1].trimmed();
		//Convert each token to MixedNumber
		QValidator::State state1 = MixedNumber::fromString(
			number1, result.first, locale_aware );
		QValidator::State state2 = MixedNumber::fromString(
			number2, result.second, locale_aware );
		//Check the results
		if ( state1 != QValidator::Acceptable ) {
			return QValidator::Invalid;
		} else {
			if ( (state2 == QValidator::Acceptable)
			&& !result.isValid() ) {
				return QValidator::Intermediate;
			} else {
				return state2;
			}
		}
	} else if ( tokensCount == 1 ) {
		//Get the token
		number1 = tokens[0].trimmed();
		//Convert the token to MixedNumber
		QValidator::State state1 = MixedNumber::fromString(
			number1, result.first, locale_aware );
		//Return the validator state
		return state1;
	}

	return QValidator::Invalid;
}

QString MixedNumberRange::toString( MixedNumber::Format format,
	bool locale_aware ) const
{
	QString result = this->first.toString( format, locale_aware );
	if ( this->second.isValid() ) {
		result += " - " + this->second.toString( format, locale_aware );
	}
	return result;
}

bool MixedNumberRange::isValid() const
{
	bool result = this->first.isValid()
		&& (this->second > this->first);
	return result;
}
