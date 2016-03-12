/****************************************************************************
 *   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>      *
 *                                                                          *
 *   This program is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 ****************************************************************************/

#include "mixednumberrange.h"

#include "datablocks/mixednumberrange.h"
#include "datablocks/mixednumber.h"
#include "datablocks/ingredient.h"

#include <QtTest/QTest>
#include <QDebug>
#include <QtGlobal>


Q_DECLARE_METATYPE(QValidator::State);
#define INV QValidator::Invalid
#define ITM QValidator::Intermediate
#define ACC QValidator::Acceptable

#define ANY_LOCALE "C,es"

void TestMixedNumberRange::testFromString_data()
{
	QTest::addColumn<QString>("rangeString");
	QTest::addColumn<QString>("locales");
	QTest::addColumn<QValidator::State>("validatorState");
	QTest::addColumn<bool>("isValid");

	QTest::addColumn<int>("whole1");
	QTest::addColumn<int>("numerator1");
	QTest::addColumn<int>("denominator1");
	QTest::addColumn<double>("value1");
	QTest::addColumn<bool>("isValid1");

	QTest::addColumn<int>("whole2");
	QTest::addColumn<int>("numerator2");
	QTest::addColumn<int>("denominator2");
	QTest::addColumn<double>("value2");
	QTest::addColumn<bool>("isValid2");

	QTest::newRow("null string")
	/* Number string	locales		validator state		valid range?*/
	<< QString()		<< ANY_LOCALE	<< ITM			<< false
	/* whole	numerator	denominator	value 		valid? */
	<< 0		<< 0		<< 0		<< 0.0		<< false
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("empty string")
	/* Number string	locales		validator state		valid range?*/
	<< ""			<< ANY_LOCALE	<< ITM			<< false
	/* whole	numerator	denominator	value 		valid? */
	<< 0		<< 0		<< 0		<< 0.0		<< false
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("valid1")
	/* Number string	locales		validator state		valid range?*/
	<< "1/2 - 1"		<< ANY_LOCALE	<< ACC			<< true
	/* whole	numerator	denominator	value		valid?*/
	<< 0		<< 1		<< 2		<< 0.5		<< true
	<< 1		<< 0		<< 1		<< 1.0		<< true;

	QTest::newRow("valid2")
	/* Number string	locales		validator state		valid range?*/
	<< "1/2-1"		<< ANY_LOCALE	<< ACC			<< true
	/* whole	numerator	denominator	value		valid?*/
	<< 0		<< 1		<< 2		<< 0.5		<< true
	<< 1		<< 0		<< 1		<< 1.0		<< true;

	//Probably it doesn't make much sense for Krecipes, but you can have this one
	QTest::newRow("valid3")
	/* Number string	locales		validator state		valid range?*/
	<< "1/2 - 0.75"		<< ANY_LOCALE	<< ACC			<< true
	/* whole	numerator	denominator	value		valid?*/
	<< 0		<< 1		<< 2		<< 0.5		<< true
	<< 0		<< 3		<< 4		<< 0.75		<< true;

	QTest::newRow("valid4")
	/* Number string	locales		validator state		valid range?*/
	<< "1/2"		<< ANY_LOCALE	<< ACC			<< true
	/* whole	numerator	denominator	value		valid?*/
	<< 0		<< 1		<< 2		<< 0.5		<< true
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid1")
	/* Number string	locales		validator state		valid range?*/
	<< "1/ - 3"		<< ANY_LOCALE	<< INV			<< false
	/* whole	numerator	denominator	value		valid?*/
	<< 0		<< 1		<< 2		<< 0.5		<< true
	<< 1		<< 0		<< 1		<< 1.0		<< true;

	QTest::newRow("invalid2")
	/* Number string	locales		validator state		valid range?*/
	<< "1/-"		<< ANY_LOCALE	<< INV			<< false
	/* whole	numerator	denominator	value		valid?*/
	<< 0		<< 1		<< 2		<< 0.5		<< true
	<< 1		<< 0		<< 1		<< 1.0		<< true;

	QTest::newRow("invalid3")
	/* Number string	locales		validator state		valid range?*/
	<< "-"			<< ANY_LOCALE	<< INV			<< false
	/* whole	numerator	denominator	value		valid?*/
	<< 0		<< 1		<< 2		<< 0.5		<< true
	<< 1		<< 0		<< 1		<< 1.0		<< true;

	QTest::newRow("invalid4")
	/* Number string	locales		validator state		valid range?*/
	<< "1/2 -- 1"		<< ANY_LOCALE	<< INV			<< false
	/* whole	numerator	denominator	value		valid?*/
	<< 0		<< 1		<< 2		<< 0.5		<< true
	<< 1		<< 0		<< 1		<< 1.0		<< true;

	QTest::newRow("invalid6")
	/* Number string	locales		validator state		valid range?*/
	<< "-"			<< ANY_LOCALE	<< INV			<< false
	/* whole	numerator	denominator	value		valid?*/
	<< 0		<< 1		<< 2		<< 0.5		<< true
	<< 1		<< 0		<< 1		<< 1.0		<< true;

	QTest::newRow("intermediate1")
	/* Number string	locales		validator state		valid range?*/
	<< "1/2 -"		<< ANY_LOCALE	<< ITM			<< false
	/* whole	numerator	denominator	value		valid?*/
	<< 0		<< 1		<< 2		<< 0.5		<< true
	<< 1		<< 0		<< 1		<< 1.0		<< true;

	QTest::newRow("intermediate2")
	/* Number string	locales		validator state		valid range?*/
	<< "1/2 - 3/"		<< ANY_LOCALE	<< ITM			<< false
	/* whole	numerator	denominator	value		valid?*/
	<< 0		<< 1		<< 2		<< 0.5		<< true
	<< 1		<< 0		<< 1		<< 1.0		<< true;

	QTest::newRow("intermediate3")
	/* Number string	locales		validator state		valid range?*/
	<< "1/2 - 1/3"		<< ANY_LOCALE	<< ITM			<< false
	/* whole	numerator	denominator	value		valid?*/
	<< 0		<< 1		<< 2		<< 0.5		<< true
	<< 0		<< 1		<< 3		<< 1.0/3.0	<< true;

	QTest::newRow("intermediate4")
	/* Number string	locales		validator state		valid range?*/
	<< "1-"			<< ANY_LOCALE	<< ITM			<< false
	/* whole	numerator	denominator	value		valid?*/
	<< 0		<< 1		<< 2		<< 0.5		<< true
	<< 0		<< 1		<< 3		<< 1.0/3.0	<< true;

	QTest::newRow("intermediate5")
	/* Number string	locales		validator state		valid range?*/
	<< "1. - "		<< ANY_LOCALE	<< ITM			<< false
	/* whole	numerator	denominator	value		valid?*/
	<< 1		<< 0		<< 1		<< 1.0		<< true
	<< 1		<< 0		<< 1		<< 1.0		<< false;


}

void TestMixedNumberRange::testFromString()
{
	//Fetch data
	QFETCH( QString, rangeString );
	QFETCH( QString, locales );
	QFETCH( QValidator::State, validatorState );
	QFETCH( bool, isValid );

	QFETCH( int, whole1 );
	QFETCH( int, numerator1 );
	QFETCH( int, denominator1 );
	QFETCH( double, value1 );
	QFETCH( bool, isValid1 );

	QFETCH( int, whole2 );
	QFETCH( int, numerator2 );
	QFETCH( int, denominator2 );
	QFETCH( double, value2 );
	QFETCH( bool, isValid2 );

	QStringList localeList = locales.split(",");
	foreach( QString locale, localeList ) {

		//Set locale
		QLocale::setDefault(QLocale(locale));

		//Convert to MixedNumberRange
		MixedNumberRange numberRange;
		QValidator::State state;
		state = MixedNumberRange::fromString( rangeString, numberRange, false );

		//Check the validator state
		qDebug() << "String:" << rangeString << "Locale:" << locale
			<< "Validator state: " << state;
		QCOMPARE( state, validatorState );

		//Check if the range is valid
		QCOMPARE( numberRange.isValid(), isValid );

		//If the tested data is valid, check each number of the range
		if ( isValid ) {
			//Number 1
			if ( isValid1 ) {
				QCOMPARE( numberRange.first.whole(), whole1 );
				QCOMPARE( numberRange.first.numerator(), numerator1 );
				QCOMPARE( numberRange.first.denominator(), denominator1 );
				QCOMPARE( numberRange.first.toDouble(), value1 );
				QCOMPARE( numberRange.first.isValid(), isValid1 );
			}
			//Number 2
			if ( isValid2 ) {
				QCOMPARE( numberRange.second.whole(), whole2 );
				QCOMPARE( numberRange.second.numerator(), numerator2 );
				QCOMPARE( numberRange.second.denominator(), denominator2 );
				QCOMPARE( numberRange.second.toDouble(), value2 );
				QCOMPARE( numberRange.second.isValid(), isValid2 );
			}

		} //end if

	} //end foreach
}


QTEST_MAIN(TestMixedNumberRange)

#include "mixednumberrange.moc"
