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
#include <QtGlobal>

Q_DECLARE_METATYPE(QValidator::State);
#define INV QValidator::Invalid
#define ITM QValidator::Intermediate
#define ACC QValidator::Acceptable

#define ANY_LOCALE "C,es"

void TestMixedNumber::testFromString_data()
{
	QTest::addColumn<QString>("numberString");
	QTest::addColumn<QString>("locales");
	QTest::addColumn<QValidator::State>("validatorState");
	QTest::addColumn<bool>("isValid");
	QTest::addColumn<int>("whole");
	QTest::addColumn<int>("numerator");
	QTest::addColumn<int>("denominator");
	QTest::addColumn<double>("value");
	QTest::addColumn<bool>("isFraction");

	QTest::newRow("null string")
	/* Number string		locales		validator state		isValid?*/
	<< QString()			<< ANY_LOCALE	<< ITM			<< false
	/* whole	numerator	denominator	value 		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("empty string")
	/* Number string		locales		validator state		isValid?*/
	<< ""				<< ANY_LOCALE	<< ITM			<< false
	/* whole	numerator	denominator	value 		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("fraction")
	/* Number string		locales		validator state		isValid?*/
	<< "2/3"			<< ANY_LOCALE	<< ACC			<< true
	/* whole	numerator	denominator	value 		is fraction? */
	<< 0		<< 2		<< 3		<< 2.0/3.0	<< true;

	QTest::newRow("decimal")
	/* Number string		locales		validator state		isValid?*/
	<< "0.5"			<< ANY_LOCALE	<< ACC			<< true
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 1		<< 2		<< 0.5		<< false;

	QTest::newRow("decimal2")
	/* Number string		locales		validator state		isValid?*/
	<< "2."				<< ANY_LOCALE	<< ACC			<< true
	/* whole	numerator	denominator	value		is fraction? */
	<< 2		<< 0		<< 1		<< 2.0		<< false;

	QTest::newRow("mixed")
	/* Number string		locales		validator state		isValid?*/
	<< "2 3/4"			<< ANY_LOCALE	<< ACC			<< true
	/* whole	numerator	denominator	value		is fraction? */
	<< 2		<< 3		<< 4		<< 2.75		<< true;

	QTest::newRow("decimal, scientific notation") /* this should be forbidden */
	/* Number string		locales		validator state		isValid?*/
	<< "1e"				<< ANY_LOCALE	<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid1")
	/* Number string		locales		validator state		isValid?*/
	<< "abc"			<< ANY_LOCALE	<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid2")
	/* Number string		locales		validator state		isValid?*/
	<< "3.."			<< ANY_LOCALE	<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid3")
	/* Number string		locales		validator state		isValid?*/
	<< "23/ 4"			<< ANY_LOCALE	<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid4")
	/* Number string		locales		validator state		isValid?*/
	<< "8//"			<< ANY_LOCALE	<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid5")
	/* Number string		locales		validator state		isValid?*/
	<< "4 /3"			<< ANY_LOCALE	<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid6")
	/* Number string		locales		validator state		isValid?*/
	<< "3/0.4"			<< ANY_LOCALE	<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid7")
	/* Number string		locales		validator state		isValid?*/
	<< "3/.4"			<< ANY_LOCALE	<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid8")
	/* Number string		locales		validator state		isValid?*/
	<< "3/0"			<< ANY_LOCALE	<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid9")
	/* Number string		locales		validator state		isValid?*/
	<< "-"				<< ANY_LOCALE	<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid10")
	/* Number string		locales		validator state		isValid?*/
	<< "-3"				<< ANY_LOCALE	<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid11")
	/* Number string		locales		validator state		isValid?*/
	<< "-2/3"			<< ANY_LOCALE	<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid12")
	/* Number string		locales		validator state		isValid?*/
	<< "2/-"			<< ANY_LOCALE	<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid13")
	/* Number string		locales		validator state		isValid?*/
	<< "2/-3"			<< ANY_LOCALE	<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("invalid14")
	/* Number string		locales		validator state		isValid?*/
	<< ".678./"			<< ANY_LOCALE	<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("intermediate1")
	/* Number string		locales		validator state		isValid?*/
	<< "1/"				<< ANY_LOCALE	<< ITM			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("intermediate2")
	/* Number string		locales		validator state		isValid?*/
	<< "4 3/"			<< ANY_LOCALE	<< ITM			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;

	QTest::newRow("intermediate3")
	/* Number string		locales		validator state		isValid?*/
	<< "5 2"			<< ANY_LOCALE	<< ITM			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 0		<< 0		<< 0.0		<< false;


	/*****************************************************************************/
	/* Locale specific tests: check expected different behaviors.                */
	/*****************************************************************************/

	QTest::newRow("decimal_es") /* This is OK in Spanish locale */
	/* Number string		locales		validator state		isValid?*/
	<< "0,5"			<< "es"		<< ACC			<< true
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 1		<< 2		<< 0.5		<< false;

	QTest::newRow("decimal_es_inv") /* This should fail with "C" locale */
	/* Number string		locales		validator state		isValid?*/
	<< "0,5"			<< "C"		<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 0		<< 1		<< 2		<< 0.5		<< false;

	QTest::newRow("decimal2") /* This is OK in Spanish locale */
	/* Number string		locales		validator state		isValid?*/
	<< "2,"				<< "es"		<< ACC			<< true
	/* whole	numerator	denominator	value		is fraction? */
	<< 2		<< 0		<< 1		<< 2.0		<< false;

	QTest::newRow("decimal2_es_inv") /* This should fail with "C" locale */
	/* Number string		locales		validator state		isValid?*/
	<< "2,"				<< "C"		<< INV			<< false
	/* whole	numerator	denominator	value		is fraction? */
	<< 2		<< 0		<< 1		<< 2.0		<< false;

	QTest::newRow("thousand separator") /* This shouldn't be taken as one thousand */
	/* Number string		locales		validator state		isValid?*/
	<< "1.000"			<< ANY_LOCALE	<< ACC		<< true
	/* whole	numerator	denominator	value		is fraction? */
	<< 1		<< 0		<< 1		<< 1.0		<< false;

}

void TestMixedNumber::testFromString()
{
	//Fetch data
	QFETCH( QString, numberString );
	QFETCH( QString, locales );
	QFETCH( QValidator::State, validatorState );
	QFETCH( bool, isValid );
	QFETCH( int, whole );
	QFETCH( int, numerator );
	QFETCH( int, denominator );
	QFETCH( double, value );
	QFETCH( bool, isFraction );

	QStringList localeList = locales.split(",");
	foreach ( QString locale, localeList ) {

		//Set locale
		QLocale::setDefault(QLocale(locale));

		//Convert to MixedNumber
		MixedNumber number;
		QValidator::State state;
		state = MixedNumber::fromString( numberString, number );

		//Check the results
		qDebug() << "String:" << numberString << "Locale:" << locale 
			<< "Validator state: " << state;
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
}

void TestMixedNumber::testToString_data()
{
	QTest::addColumn<int>("whole");
	QTest::addColumn<int>("numerator");
	QTest::addColumn<int>("denominator");
	QTest::addColumn<QString>("fractionString");
	QTest::addColumn<QString>("decimalString");
	QTest::addColumn<bool>("locale_aware");
	QTest::addColumn<QString>("locales");

	QTest::newRow("invalid MixedNumber")
	/* whole	numerator	denominator	fraction	decimal */
	<< 0		<< 0		<< 0		<< ""		<< ""
	/* locale aware?	locale */
	<< false		<< ANY_LOCALE;

	QTest::newRow("valid1")
	/* whole	numerator	denominator	fraction	decimal */
	<< 0		<< 1		<< 2		<< "1/2"	<< "0.5"
	/* locale aware?	locale */
	<< false		<< ANY_LOCALE;

	QTest::newRow("valid1_es")
	/* whole	numerator	denominator	fraction	decimal */
	<< 0		<< 1		<< 2		<< "1/2"	<< "0,5"
	/* locale aware?	locale */
	<< true		<< "es";

	QTest::newRow("valid2")
	/* whole	numerator	denominator	fraction	decimal */
	<< 1		<< 1		<< 2		<< "1 1/2"	<< "1.5"
	/* locale aware?	locale */
	<< false		<< ANY_LOCALE;

	QTest::newRow("valid2_es")
	/* whole	numerator	denominator	fraction	decimal */
	<< 1		<< 1		<< 2		<< "1 1/2"	<< "1,5"
	/* locale aware?	locale */
	<< true		<< "es";

	QTest::newRow("valid3")
	/* whole	numerator	denominator	fraction	decimal */
	<< 1000		<< 0		<< 1		<< "1000"	<< "1000"
	/* locale aware?	locale */
	<< false		<< ANY_LOCALE;

	QTest::newRow("valid3_es")
	/* whole	numerator	denominator	fraction	decimal */
	<< 1000		<< 0		<< 1		<< "1000"	<< "1000"
	/* locale aware?	locale */
	<< true		<< "es";

}

void TestMixedNumber::testToString()
{
	QFETCH( int, whole );
	QFETCH( int, numerator );
	QFETCH( int, denominator );
	QFETCH( QString, fractionString );
	QFETCH( QString, decimalString );
	QFETCH( bool, locale_aware );
	QFETCH( QString, locales );

	MixedNumber number( whole, numerator, denominator );

	QStringList localeList = locales.split(",");
	foreach ( QString locale, localeList ) {

		//Set locale
		QLocale::setDefault(QLocale(locale));

		//Check the mixed number string
		QCOMPARE( number.toString( MixedNumber::MixedNumberFormat, locale_aware ),
			fractionString );

		//Check the decimal string
		QCOMPARE( number.toString( MixedNumber::DecimalFormat, locale_aware ),
			decimalString );

	}
}

void TestMixedNumber::testToFromString_data()
{
	testToString_data();
}

void TestMixedNumber::testToFromString()
{
	QFETCH( int, whole );
	QFETCH( int, numerator );
	QFETCH( int, denominator );
	QFETCH( QString, fractionString );
	QFETCH( QString, decimalString );
	QFETCH( bool, locale_aware );
	QFETCH( QString, locales );

	MixedNumber origNumber( whole, numerator, denominator );

	MixedNumber parsedMixedNumber1;
	MixedNumber parsedMixedNumber2;

	QStringList localeList = locales.split(",");
	foreach ( QString locale, localeList ) {

		//Set locale
		QLocale::setDefault(QLocale(locale));

		//Test that the numbers parsed are the same than the original
		MixedNumber::fromString(
			origNumber.toString( MixedNumber::MixedNumberFormat, locale_aware ),
			parsedMixedNumber1 );

		QCOMPARE( parsedMixedNumber1.toDouble(), origNumber.toDouble() );

		MixedNumber::fromString(
			origNumber.toString( MixedNumber::DecimalFormat, locale_aware ),
			parsedMixedNumber2 );

		QCOMPARE( parsedMixedNumber2.toDouble(), origNumber.toDouble() );

	}
}


QTEST_MAIN(TestMixedNumber)

#include "mixednumber.moc"
