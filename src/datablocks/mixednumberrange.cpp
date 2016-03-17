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

MixedNumberRange::MixedNumberRange( double amount, double offset )
{
	if ( amount == 0.0 ) {
		this->first = MixedNumber(0,0,0);
	} else {
		this->first = MixedNumber( amount );
	}
	if ( offset == 0.0 ) {
		this->second = MixedNumber(0,0,0);
	} else {
		this->second = MixedNumber( amount+offset );
	}
}

MixedNumberRange::~MixedNumberRange()
{
}

QValidator::State MixedNumberRange::fromString( const QString & input,
	MixedNumberRange & result )
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
			number1, result.first );
		QValidator::State state2 = MixedNumber::fromString(
			number2, result.second );
		//Check the results
		if ( state1 != QValidator::Acceptable ) {
			return QValidator::Invalid;
		} else {
			if ( state2 == QValidator::Acceptable ) {
				if ( !result.isValid() ) {
					//Mark the range as invalid invalidating
					//the first number
					result.first = MixedNumber(0,0,0);
					return QValidator::Intermediate;
				} else {
					return QValidator::Acceptable;
				}
			} else if ( state2 == QValidator::Intermediate ) {
				//Mark the range as invalid invalidating
				//the first number
				result.first = MixedNumber(0,0,0);
				return QValidator::Intermediate;
			} else {
				result.first = MixedNumber(0,0,0);
				result.second = MixedNumber(0,0,0);
				return QValidator::Invalid;
			}
		}
	} else if ( tokensCount == 1 ) {
		//Get the token
		number1 = tokens[0].trimmed();
		//Convert the token to MixedNumber
		QValidator::State state1 = MixedNumber::fromString(
			number1, result.first );
		result.second = MixedNumber(0,0,0); //invalid mixednumber
		//Return the validator state
		return state1;
	}

	return QValidator::Invalid;
}

QString MixedNumberRange::toString( MixedNumber::Format format,
	bool locale_aware ) const
{
	if ( !this->isValid() ) {
		return "";
	}

	QString result = this->first.toString( format, locale_aware );
	if ( this->second.isValid() ) {
		result += " - " + this->second.toString( format, locale_aware );
	}
	return result;
}

QString MixedNumberRange::toString( bool locale_aware ) const
{
	return toString( MixedNumber::AutoFormat, locale_aware );
}

void MixedNumberRange::toAmountAndOffset( double * amount, double * offset ) const
{
	*amount = this->first.toDouble();
	if ( this->second.isValid() ) {
		*offset = this->second.toDouble() - *amount;
	} else {
		*offset = 0.0;
	}
}

bool MixedNumberRange::isValid() const
{
	bool result = this->first.isValid()
		&& ( !this->second.isValid() || (this->second > this->first) );
	return result;
}


bool MixedNumberRange::isPlural() const
{
	if ( this->second.isValid() ) {
		return true;
	} else {
		return this->first.isPlural();
	}
}

