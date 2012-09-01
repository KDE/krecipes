/***************************************************************************
 *   Copyright © 2012 José Manuel Santamaría Lema <panfaust@gmail.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include "mixednumbervalidator.h"

#include "datablocks/mixednumber.h"


QValidator::State MixedNumberValidator::validate ( QString & input, int & pos ) const
{
	Q_UNUSED(pos)

	MixedNumber number;
	return MixedNumber::fromString( input, number, true );
}
