/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
 
#ifndef FRACTIONINPUT_H
#define FRACTIONINPUT_H

#include <klineedit.h>

#include "mixednumber.h"

/** A KLineEdit widget extended to allow input of decimals and fractions.
  * Input is returned as a @ref MixedNumber class.
  * @author Jason Kivlighn
  */
class FractionInput : public KLineEdit
{
public:
	FractionInput( QWidget *parent = 0, const char *name = 0 );
	~FractionInput();

	void setValue( double );
	void setValue( const MixedNumber & );

	MixedNumber value() const;
	bool isInputValid() const;
};

#endif //FRACTIONINPUT_H

