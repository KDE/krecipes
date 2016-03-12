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
	QTest::addColumn<bool>("isValid");
	QTest::addColumn<int>("whole");
	QTest::addColumn<int>("numerator");
	QTest::addColumn<int>("denominator");
	QTest::addColumn<double>("value");
	QTest::addColumn<bool>("isFraction");

	QTest::newRow("null string")
	/* Number string		validator state		isValid?*/
	<< QString()			<< ITM			<< false
	/* whole	numerator	denominator	value 		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("empty string")
	/* Number string		validator state		isValid?*/
	<< ""				<< ITM			<< false
	/* whole	numerator	denominator	value 		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("fraction")
	/* Number string		validator state		isValid?*/
	<< "2/3"			<< ACC			<< true
	/* whole	numerator	denominator	value 		is fraction? */
	<< 0		<< 2		<< 3		<< 2.0/3.0	<< true;

	QTest::newRow("decimal")
	/* Number string		validator state		isValid?*/
	<< "0.5"			<< ACC			<< true
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 1		<< 2		<< 0.5		<< false;

	QTest::newRow("decimal2")
	/* Number string		validator state		isValid?*/
	<< "2."				<< ACC			<< true
	/* whole	numerator	denominator	value		is fraction? */
	<< 2		<< 0		<< 1		<< 2.0		<< false;

	QTest::newRow("mixed")
	/* Number string		validator state		isValid?*/
	<< "2 3/4"			<< ACC			<< true
	/* whole	numerator	denominator	value		is fraction? */
	<< 2		<< 3		<< 4		<< 2.75		<< true;

	QTest::newRow("invalid1")
	/* Number string		validator state		isValid?*/
	<< "abc"			<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid2")
	/* Number string		validator state		isValid?*/
	<< "3.."			<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid3")
	/* Number string		validator state		isValid?*/
	<< "23/ 4"			<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid4")
	/* Number string		validator state		isValid?*/
	<< "8//"			<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid5")
	/* Number string		validator state		isValid?*/
	<< "4 /3"			<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid6")
	/* Number string		validator state		isValid?*/
	<< "3/0.4"			<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid7")
	/* Number string		validator state		isValid?*/
	<< "3/.4"			<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid8")
	/* Number string		validator state		isValid?*/
	<< "3/0"			<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid9")
	/* Number string		validator state		isValid?*/
	<< "-"				<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid10")
	/* Number string		validator state		isValid?*/
	<< "-3"				<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid11")
	/* Number string		validator state		isValid?*/
	<< "-2/3"				<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid12")
	/* Number string		validator state		isValid?*/
	<< "2/-"				<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid13")
	/* Number string		validator state		isValid?*/
	<< "2/-3"				<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("intermediate1")
	/* Number string		validator state		isValid?*/
	<< "1/"				<< ITM			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("intermediate2")
	/* Number string		validator state		isValid?*/
	<< "4 3/"			<< ITM			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("intermediate3")
	/* Number string		validator state		isValid?*/
	<< "5 2"			<< ITM			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

}

void TestMixedNumber::testFromString()
{
	//FIXME: Test this with other locales
	QLocale::setDefault(QLocale("C"));

	//Fetch data
	QFETCH( QString, numberString );
	QFETCH( QValidator::State, validatorState );
	QFETCH( bool, isValid );
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
	QCOMPARE( number.isValid(), isValid );
	if ( isValid ) {
		QCOMPARE( number.whole(), whole );
		QCOMPARE( number.numerator(), numerator );
		QCOMPARE( number.denominator(), denominator );
		QCOMPARE( number.toDouble(), value );
		QCOMPARE( MixedNumber::isFraction(numberString), isFraction );
	}
}


QTEST_MAIN(TestMixedNumber)

#include "mixednumber.moc"
