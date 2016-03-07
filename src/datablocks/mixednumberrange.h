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

class MixedNumberRange: public QPair<MixedNumber,MixedNumber>
{
public:
	MixedNumberRange();
	MixedNumberRange( const MixedNumber &value1, const MixedNumber&value2 );
	~MixedNumberRange();

	/* toString */

	static QValidator::State fromString( const QString &input,
		MixedNumberRange &result, bool locale_aware );

	bool isValid() const;
};

#endif
