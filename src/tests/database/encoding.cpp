/***************************************************************************
 *   Copyright © 2015 Adriaan de Groot <groot@kde.org>                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include "encoding.h"

#include "backends/recipedb.h"

#include <QtTest/QTest>

#include <QDebug>


void TestDatabaseEncoding::initTestCase()
{
	QLatin1String dummy("dummy");
	m_sqliteDatabase = RecipeDB::createDatabase(QLatin1String("SQLite"), dummy, dummy, dummy, dummy, 0, QLatin1String(":memory:"));
	QVERIFY(m_sqliteDatabase);
	m_sqliteDatabase->connect();

        QVERIFY( m_sqliteDatabase->ok() );
	m_sqliteDatabase->transaction();
	m_sqliteDatabase->wipeDatabase();
	m_sqliteDatabase->commit();
	m_sqliteDatabase->checkIntegrity();
}


void TestDatabaseEncoding::cleanupTestCase()
{
	if ( m_sqliteDatabase ) {
		delete m_sqliteDatabase;
	}
}

void TestDatabaseEncoding::testSourceEncoding()
{
	QString moose0 = QString::fromUtf8("\xc2\xb5\xc3\xb8\xc3\xb8\x73\x65");  // UTF-8 bytes
	QString moose1 = QString::fromUtf8("µøøse");  // Natively encoded as UTF-8 in source
	QLatin1String moose2("\xb5\xf8\xf8\x73\x65");  // Latin1
	
	QCOMPARE( moose0.length(), 5 );
	QCOMPARE( moose0.at(0), QChar(0xb5) );  // "micro" symbol
	QCOMPARE( moose0, moose1 );
	QCOMPARE( moose0, moose2 );  // This is the important bit: the string is representable in latin1
	
	qDebug() << "ORG" << moose0;
}

void TestDatabaseEncoding::testInsertPropertyLatin1()
{
	QString moose1 = QString::fromUtf8("µøøse");
	QVERIFY( m_sqliteDatabase->ok() );
	
	// Add property "grams of moose" (per kilogram, I suppose)
	RecipeDB::IdType i = m_sqliteDatabase->addProperty( moose1, QString("g"));
	QVERIFY( i > 0 );
	if ( i > 0 ) {
		m_sqliteDatabase->removeProperty( i );
	}
}

void TestDatabaseEncoding::testRetrievePropertyLatin1()
{
	QString moose1 = QString::fromUtf8("µøøse");
	QVERIFY( m_sqliteDatabase->ok() );
	
	// Add property "grams of moose" (per kilogram, I suppose)
	RecipeDB::IdType i = m_sqliteDatabase->addProperty( moose1, QString("g"));

	QVERIFY( i > 0 );
	
	IngredientProperty p = m_sqliteDatabase->propertyName( i );
	QCOMPARE( p.units, QString("g") );
	QCOMPARE( p.name, moose1 );
	
	qDebug() << "ORG" << moose1 << "GOT" << p.units;
}


QTEST_MAIN(TestDatabaseEncoding)

#include "encoding.moc"
