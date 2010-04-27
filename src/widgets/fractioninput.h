/***************************************************************************
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef FRACTIONINPUT_H
#define FRACTIONINPUT_H

#include <klineedit.h>

#include "datablocks/mixednumber.h"

class QTimer;

/** A KLineEdit widget extended to allow input of decimals and fractions or ranges of such.
  * Input is returned as a @ref MixedNumber class.
  * @author Jason Kivlighn
  */
class FractionInput : public KLineEdit
{
Q_OBJECT

public:
	explicit FractionInput( QWidget *parent = 0, MixedNumber::Format = MixedNumber::MixedNumberFormat );
	~FractionInput();

	void setAllowRange( bool b ){ m_allowRange = b; }

	void setValue( double amount, double amount_offset = 0 );
	void setValue( const MixedNumber &, double amount_offset = 0 );

	void value( MixedNumber &amount, double &amount_offset ) const;
	void value( double &amount, double &amount_offset ) const;
	MixedNumber minValue() const;
	MixedNumber maxValue() const;
	MixedNumber value() const;

	bool isInputValid() const;
	bool isEmpty() const;

signals:
	void valueChanged( const MixedNumber & );

public slots:
	void validate();

private slots:
	void slotStartValidateTimer();

private:
	bool m_allowRange;
	QTimer *m_validateTimer;
	MixedNumber::Format m_format;
};

#endif //FRACTIONINPUT_H

