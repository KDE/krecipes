/***************************************************************************
 *   Copyright (C) 2003 by krecipes.sourceforge.net authors                *
 *                                                                         *
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
  * Input is returned as a @ref Fraction class.
  * @author Jason Kivlighn
  */
class FractionInput : public KLineEdit
{
Q_OBJECT

public:
	FractionInput( QWidget *parent = 0, const char *name = 0 );
	~FractionInput();

	void setValue( double );
	void setValue( MixedNumber & );

	MixedNumber value();
	bool isInputValid();

protected slots:
	void parseText( bool *ok = 0 );

private:
	MixedNumber m_fraction;
};

#endif //FRACTIONINPUT_H

