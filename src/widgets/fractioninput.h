/***************************************************************************
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                  *
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>      *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#ifndef FRACTIONINPUT_H
#define FRACTIONINPUT_H

#include "validators/mixednumberrangevalidator.h"
#include "validators/mixednumbervalidator.h"
#include "datablocks/mixednumber.h"

#include <KLineEdit>

#include "deprecated.h"

class MixedNumberRangeValidator;
class MixedNumberValidator;
class MixedNumberRange;
class MixedNumber;


/** A KLineEdit widget extended to allow input of decimals and fractions or ranges of such.
  * Input is returned as a @ref MixedNumberRange class.
  * @author Jason Kivlighn
  */
class FractionInput : public KLineEdit
{
Q_OBJECT

public:
	explicit FractionInput( QWidget *parent = 0, MixedNumber::Format = MixedNumber::MixedNumberFormat );
	~FractionInput();

	void setAllowRange( bool allowRange );

	void setValueRange( const MixedNumberRange & value );
	MixedNumberRange valueRange() const;

	/* KRECIPES_DEPRECATED */ void setValue( double amount, double amount_offset = 0 );
	/* KRECIPES_DEPRECATED */ void setValue( const MixedNumber &, double amount_offset = 0 );
	/* KRECIPES_DEPRECATED */ void value( MixedNumber &amount, double &amount_offset ) const;
	/* KRECIPES_DEPRECATED */ void value( double &amount, double &amount_offset ) const;
	/* KRECIPES_DEPRECATED */ MixedNumber minValue() const;
	/* KRECIPES_DEPRECATED */ MixedNumber maxValue() const;
	/* KRECIPES_DEPRECATED */ MixedNumber value() const;

	bool isInputValid() const;
	bool isEmpty() const;

private:
	MixedNumberValidator  m_numberValidator;
	MixedNumberRangeValidator m_rangeValidator;

	bool m_allowRange;
	MixedNumber::Format m_format;
};

#endif //FRACTIONINPUT_H

