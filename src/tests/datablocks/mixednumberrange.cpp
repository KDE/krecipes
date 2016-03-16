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


	/*****************************************************************************/
	/* Locale specific tests: check expected different behaviors.                */
	/*****************************************************************************/

	//Probably it doesn't make much sense for Krecipes, but you can have this one
	QTest::newRow("valid3_es")
	/* Number string	locales		validator state		valid range?*/
	<< "1/2 - 0,75"		<< "es"		<< ACC			<< true
	/* whole	numerator	denominator	value		valid?*/
	<< 0		<< 1		<< 2		<< 0.5		<< true
	<< 0		<< 3		<< 4		<< 0.75		<< true;

	QTest::newRow("valid3_es_inv")
	/* Number string	locales		validator state		valid range?*/
	<< "1/2 - 0,75"		<< "C"		<< INV			<< false
	/* whole	numerator	denominator	value		valid?*/
	<< 0		<< 1		<< 2		<< 0.5		<< true
	<< 0		<< 3		<< 4		<< 0.75		<< true;

	QTest::newRow("intermediate5_es")
	/* Number string	locales		validator state		valid range?*/
	<< "1, - "		<< "es"		<< ITM			<< false
	/* whole	numerator	denominator	value		valid?*/
	<< 1		<< 0		<< 1		<< 1.0		<< true
	<< 1		<< 0		<< 1		<< 1.0		<< false;

	QTest::newRow("intermediate5_es_inv")
	/* Number string	locales		validator state		valid range?*/
	<< "1, - "		<< "C"		<< INV			<< false
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
		state = MixedNumberRange::fromString( rangeString, numberRange );

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

void TestMixedNumberRange::testToString_data()
{
	QTest::addColumn<int>("whole1");
	QTest::addColumn<int>("numerator1");
	QTest::addColumn<int>("denominator1");

	QTest::addColumn<int>("whole2");
	QTest::addColumn<int>("numerator2");
	QTest::addColumn<int>("denominator2");

	QTest::addColumn<QString>("fractionString");
	QTest::addColumn<QString>("decimalString");
	QTest::addColumn<bool>("locale_aware");
	QTest::addColumn<QString>("locales");

	QTest::newRow("invalid1")
	/* whole	numerator	denominator */
	<< 0		<< 0		<< 0
	<< 0		<< 0		<< 0
	/* fraction	decimal */
	<< ""		<< ""
	/* locale aware?	locale */
	<< false		<< ANY_LOCALE;

	QTest::newRow("invalid1 locale aware")
	/* whole	numerator	denominator */
	<< 0		<< 0		<< 0
	<< 0		<< 0		<< 0
	/* fraction	decimal */
	<< ""		<< ""
	/* locale aware?	locale */
	<< true			<< ANY_LOCALE;

	QTest::newRow("invalid2")
	/* whole	numerator	denominator */
	<< 0		<< 0		<< 0
	<< 0		<< 1		<< 2
	/* fraction	decimal */
	<< ""		<< ""
	/* locale aware?	locale */
	<< false		<< ANY_LOCALE;

	QTest::newRow("invalid2 locale aware")
	/* whole	numerator	denominator */
	<< 0		<< 0		<< 0
	<< 0		<< 1		<< 2
	/* fraction	decimal */
	<< ""		<< ""
	/* locale aware?	locale */
	<< true			<< ANY_LOCALE;

	QTest::newRow("valid1")
	/* whole	numerator	denominator */
	<< 0		<< 1		<< 2
	<< 0		<< 0		<< 0
	/* fraction	decimal */
	<< "1/2"	<< "0.5"
	/* locale aware?	locale */
	<< false		<< ANY_LOCALE;

	QTest::newRow("valid1_es")
	/* whole	numerator	denominator */
	<< 0		<< 1		<< 2
	<< 0		<< 0		<< 0
	/* fraction	decimal */
	<< "1/2"	<< "0,5"
	/* locale aware?	locale */
	<< true			<< "es";

	QTest::newRow("valid2")
	/* whole	numerator	denominator */
	<< 0		<< 1		<< 2
	<< 1		<< 0		<< 1
	/* fraction	decimal */
	<< "1/2 - 1"	<< "0.5 - 1"
	/* locale aware?	locale */
	<< false		<< ANY_LOCALE;

	QTest::newRow("valid2_es")
	/* whole	numerator	denominator */
	<< 0		<< 1		<< 2
	<< 1		<< 0		<< 1
	/* fraction	decimal */
	<< "1/2 - 1"	<< "0,5 - 1"
	/* locale aware?	locale */
	<< true			<< "es";

}

void TestMixedNumberRange::testToString()
{
	QFETCH( int, whole1 );
	QFETCH( int, numerator1 );
	QFETCH( int, denominator1 );

	QFETCH( int, whole2 );
	QFETCH( int, numerator2 );
	QFETCH( int, denominator2 );

	QFETCH( QString, fractionString );
	QFETCH( QString, decimalString );
	QFETCH( bool, locale_aware );
	QFETCH( QString, locales );

	MixedNumber number1( whole1, numerator1, denominator1 );
	MixedNumber number2( whole2, numerator2, denominator2 );

	MixedNumberRange range;
	range.first = number1;
	range.second = number2;

	QStringList localeList = locales.split(",");
	foreach( QString locale, localeList ) {

		//Set locale
		QLocale::setDefault(QLocale(locale));

		//Check the mixed number string
		QCOMPARE( range.toString( MixedNumber::MixedNumberFormat, locale_aware ),
			fractionString );

		//Check the decimal string
		QCOMPARE( range.toString( MixedNumber::DecimalFormat, locale_aware ),
			decimalString );

	}
}

void TestMixedNumberRange::testToFromString_data()
{
	testToString_data();
}

/* Make sure that the string produced with MixedNumberRange::toString()
   are parseable with MixedNumberRange::fromString() */
void TestMixedNumberRange::testToFromString()
{
	QFETCH( int, whole1 );
	QFETCH( int, numerator1 );
	QFETCH( int, denominator1 );

	QFETCH( int, whole2 );
	QFETCH( int, numerator2 );
	QFETCH( int, denominator2 );

	QFETCH( QString, fractionString );
	QFETCH( QString, decimalString );
	QFETCH( bool, locale_aware );
	QFETCH( QString, locales );

	MixedNumber origNumber1( whole1, numerator1, denominator1 );
	MixedNumber origNumber2( whole2, numerator2, denominator2 );

	MixedNumberRange origRange;
	origRange.first = origNumber1;
	origRange.second = origNumber2;

	MixedNumberRange parsedRange1;
	MixedNumberRange parsedRange2;

	QStringList localeList = locales.split(",");
	foreach( QString locale, localeList ) {

		//Set locale
		QLocale::setDefault(QLocale(locale));

		//Test with MixedNumber::MixedNumberFormat
		MixedNumberRange::fromString(
			origRange.toString( MixedNumber::MixedNumberFormat, locale_aware ),
			parsedRange1 );

		QCOMPARE( parsedRange1.isValid(), origRange.isValid() );

		if ( origRange.isValid() ) {
			QCOMPARE( parsedRange1.first.isValid(), origRange.first.isValid() );
			if ( origRange.first.isValid() ) {
				QCOMPARE( parsedRange1.first.toDouble(), origRange.first.toDouble() );
			}
			QCOMPARE( parsedRange1.second.isValid(), origRange.second.isValid() );
			if ( origRange.second.isValid() ) {
				QCOMPARE( parsedRange1.second.toDouble(), origRange.second.toDouble() );
			}
		}

		//Test with MixedNumber::DecimalFormat
		MixedNumberRange::fromString(
			origRange.toString( MixedNumber::DecimalFormat, locale_aware ),
			parsedRange2 );

		QCOMPARE( parsedRange2.isValid(), origRange.isValid() );

		if ( origRange.isValid() ) {
			QCOMPARE( parsedRange2.first.isValid(), origRange.first.isValid() );
			if ( origRange.first.isValid() ) {
				QCOMPARE( parsedRange2.first.toDouble(), origRange.first.toDouble() );
			}
			QCOMPARE( parsedRange2.second.isValid(), origRange.second.isValid() );
			if ( origRange.second.isValid() ) {
				QCOMPARE( parsedRange2.second.toDouble(), origRange.second.toDouble() );
			}
		}

	}

}


QTEST_MAIN(TestMixedNumberRange)

#include "mixednumberrange.moc"
