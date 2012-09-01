/***************************************************************************
 *   Copyright © 2012 José Manuel Santamaría Lema <panfaust@gmail.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include "mixednumber.h"

#include "datablocks/mixednumber.h"

#include <QtTest/QTest>


//FIXME: Add more tests.

void TestMixedNumber::testFraction()
{
	MixedNumber number;
	QValidator::State state;
	state = MixedNumber::fromString( "2/3", number, false);
	QCOMPARE( state, QValidator::Acceptable );
	QCOMPARE( number.numerator(), 2 );
	QCOMPARE( number.denominator(), 3 );
}


QTEST_MAIN(TestMixedNumber)

#include "mixednumber.moc"
