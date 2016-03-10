/***************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>      *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#ifndef MIXEDNUMBERRANGE_H
#define MIXEDNUMBERRANGE_H

#include "mixednumber.h"

#include <QPair>
#include <QValidator>

#include "deprecated.h"

class MixedNumberRange: public QPair<MixedNumber,MixedNumber>
{
public:
	MixedNumberRange();
	MixedNumberRange( const MixedNumber &value1, const MixedNumber&value2 );

	/* Mark as deprecated because at some point the amount ranges won't be
	   a couple of (amount+offset) doubles */
	/* KRECIPES_DEPRECATED */ MixedNumberRange( double amount, double offset );

	~MixedNumberRange();


	static QValidator::State fromString( const QString &input,
		MixedNumberRange &result, bool locale_aware );


	QString toString( MixedNumber::Format format, bool locale_aware ) const;
	QString toString( bool locale_aware ) const;

	/* Mark as deprecated because at some point the amount ranges won't be
	   a couple of (amount+offset) doubles */
	/* KRECIPES_DEPRECATED */ void toAmountAndOffset( double * amount,
		double * offset ) const;


	bool isValid() const;
};

#endif
