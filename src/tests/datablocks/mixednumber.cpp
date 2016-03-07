/****************************************************************************
 *   Copyright © 2012-2016 José Manuel Santamaría Lema <panfaust@gmail.com> *
 *                                                                          *
 *   This program is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 ****************************************************************************/


#include "mixednumber.h"

#include "datablocks/mixednumber.h"

#include <QtTest/QTest>
#include <QDebug>

Q_DECLARE_METATYPE(QValidator::State);
#define INV QValidator::Invalid
#define ITM QValidator::Intermediate
#define ACC QValidator::Acceptable

void TestMixedNumber::testFromString_data()
{
	QTest::addColumn<QString>("numberString");
	QTest::addColumn<QValidator::State>("validatorState");
	QTest::addColumn<int>("whole");
	QTest::addColumn<int>("numerator");
	QTest::addColumn<int>("denominator");
	QTest::addColumn<double>("value");
	QTest::addColumn<bool>("isFraction");

	QTest::newRow("fraction")
	/* Number string		validator state*/
	<< "2/3"			<< ACC
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 2		<< 3		<< 2.0/3.0	<< true;

	QTest::newRow("decimal")
	/* Number string		validator state*/
	<< "0.5"			<< ACC
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 1		<< 2		<< 0.5		<< false;

	QTest::newRow("mixed")
	/* Number string		validator state*/
	<< "2 3/4"			<< ACC
	/* whole	numerator	denominator	value		is fraction? */
	<< 2		<< 3		<< 4		<< 2.75		<< true;

	QTest::newRow("invalid1")
	/* Number string		validator state*/
	<< "abc"			<< INV
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid2")
	/* Number string		validator state*/
	<< "3.."			<< INV
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid3")
	/* Number string		validator state*/
	<< "23/ 4"			<< INV
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid4")
	/* Number string		validator state*/
	<< "8//"			<< INV
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid5")
	/* Number string		validator state*/
	<< "4 /3"			<< INV
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid6")
	/* Number string		validator state*/
	<< "3/0.4"			<< INV
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid7")
	/* Number string		validator state*/
	<< "3/.4"			<< INV
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid8")
	/* Number string		validator state*/
	<< "3/0"			<< INV
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;


	QTest::newRow("intermediate1")
	/* Number string		validator state*/
	<< "1/"			<< ITM
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("intermediate2")
	/* Number string		validator state*/
	<< "4 3/"			<< ITM
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("intermediate3")
	/* Number string		validator state*/
	<< "5 2"			<< ITM
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("intermediate4")
	/* Number string		validator state*/
	<< "2."			<< ITM
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

}

void TestMixedNumber::testFromString()
{
	//Fetch data
	QFETCH( QString, numberString );
	QFETCH( QValidator::State, validatorState );
	QFETCH( int, whole );
	QFETCH( int, numerator );
	QFETCH( int, denominator );
	QFETCH( double, value );
	QFETCH( bool, isFraction );

	//Convert to MixedNumber
	MixedNumber number;
	QValidator::State state;
	state = MixedNumber::fromString( numberString, number, false );

	//Check the results
	qDebug() << "Validator state: " << state;
	QCOMPARE( state, validatorState );
	if ( state == QValidator::Acceptable ) {
		QCOMPARE( number.whole(), whole );
		QCOMPARE( number.numerator(), numerator );
		QCOMPARE( number.denominator(), denominator );
		QCOMPARE( number.toDouble(), value );
		QCOMPARE( MixedNumber::isFraction(numberString), isFraction );
	}
}


QTEST_MAIN(TestMixedNumber)

#include "mixednumber.moc"
