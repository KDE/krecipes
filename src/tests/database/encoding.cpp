/***************************************************************************
 *   Copyright © 2015 Adriaan de Groot <groot@kde.org>                     *
 *   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include "encoding.h"

#include "backends/recipedb.h"

#include <KApplication>
#include <KGlobal>
#include <KStandardDirs>
#include <KConfigGroup>
#include <KSharedConfigPtr>

#include <QtTest/QTest>

#include <QDebug>


RecipeDB * TestDatabaseEncoding::createDatabase(const QString & configFilename )
{
	RecipeDB * database;

	QString configFilePath = ":/" + configFilename;
	qDebug() << "opening database configured in" << configFilePath;

	QStringList strList;
	strList << configFilePath;
	KGlobal::config()->addConfigSources( strList );

	KConfigGroup dbtypeGroup = KGlobal::config()->group( "DBType" );
	QString dbType = dbtypeGroup.readEntry( "Type", "" );

	KConfigGroup serverGroup = KGlobal::config()->group( "Server" );
	QString host = serverGroup.readEntry( "Host", "localhost" );
	QString user = serverGroup.readEntry( "Username", QString() );
	QString pass = serverGroup.readEntry( "Password", QString() );
	QString dbname = serverGroup.readEntry( "DBName", "Krecipes" );
	int port = serverGroup.readEntry( "Port", 0 );
	QString dbfile = serverGroup.readEntry( "DBFile",
		KStandardDirs::locateLocal ( "appdata", "krecipes.krecdb" ) );

	database = RecipeDB::createDatabase( dbType, host, user, pass, dbname, port, dbfile );

	database->connect();

	return database;
}

void TestDatabaseEncoding::initTestCase()
{
	m_sqliteDatabase = createDatabase( "sqliterc" );
	qDebug() << m_sqliteDatabase->dbErr;
        QVERIFY( m_sqliteDatabase->ok() );
	m_sqliteDatabase->transaction();
	m_sqliteDatabase->wipeDatabase();
	m_sqliteDatabase->commit();
	//m_sqliteDatabase->checkIntegrity();

#ifdef KRE_TESTS_MYSQL
	m_mysqlDatabase = createDatabase( "mysqlrc" );
	qDebug() << m_mysqlDatabase->dbErr;
        QVERIFY( m_mysqlDatabase->ok() );
	m_mysqlDatabase->transaction();
	m_mysqlDatabase->wipeDatabase();
	m_mysqlDatabase->commit();
	//m_mysqlDatabase->checkIntegrity();
#endif

#ifdef KRE_TESTS_POSTGRESQL
	m_postgresqlDatabase = createDatabase( "postgresqlrc" );
	qDebug() << m_postgresqlDatabase->dbErr;
        QVERIFY( m_postgresqlDatabase->ok() );
	m_postgresqlDatabase->transaction();
	m_postgresqlDatabase->wipeDatabase();
	m_postgresqlDatabase->commit();
	//m_postgresqlDatabase->checkIntegrity();
#endif
}


void TestDatabaseEncoding::cleanupTestCase()
{
	if ( m_sqliteDatabase ) {
		delete m_sqliteDatabase;
	}
#ifdef KRE_TESTS_MYSQL
	if ( m_mysqlDatabase ) {
		delete m_mysqlDatabase;
	}
#endif
#ifdef KRE_TESTS_POSTGRESQL
	if ( m_postgresqlDatabase ) {
		delete m_postgresqlDatabase;
	}
#endif
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

	if ( i > 0 ) {
		m_sqliteDatabase->removeProperty( i );
	}

}

void TestDatabaseEncoding::testInsertPropertyUTF8()
{
	QString moose1 = QString::fromUtf8("µøøse");  // That's a string that fix in latin1
	QString moose2 = QString::fromUtf8("ሐረር ቢራ");  // Harar Beer, from Ethiopia, does not fit in latin1
	QString moose3 = QString::fromUtf8("μøøse");  // That's a mu-oose, not a micro-oose
	QVERIFY( m_sqliteDatabase->ok() );

	QTextCodec *codec = QTextCodec::codecForName("latin-1");
	QVERIFY2( codec, "No codec" );
	QVERIFY2( codec->canEncode(moose1), "Can't encode micro-oose" );
	QVERIFY2( !codec->canEncode(moose2), "Can encode harar beer (unexpected)" );
	QVERIFY2( !codec->canEncode(moose3), "Can encode mu-oose (unexpected)" );
	
	QVERIFY( moose1 == QString::fromLatin1(moose1.toLatin1()) );  // Converts OK
	QVERIFY( moose2 != QString::fromLatin1(moose2.toLatin1()) );  // Lossy conversion
	QVERIFY( moose3 != QString::fromLatin1(moose3.toLatin1()) );  // Lossy conversion
	
	// Both of the non-latin1 properties can be added
	RecipeDB::IdType i = m_sqliteDatabase->addProperty( moose2, QString("mL"));
	QVERIFY( i > 0 );
	if ( i > 0 ) {
		m_sqliteDatabase->removeProperty( i );
	}

	i = m_sqliteDatabase->addProperty( moose3, QString("g"));
	QVERIFY( i > 0 );
	if ( i > 0 ) {
		m_sqliteDatabase->removeProperty( i );
	}
}

void TestDatabaseEncoding::testRetrievePropertyUTF8()
{
	QString moose2 = QString::fromUtf8("ሐረር ቢራ");  // Harar Beer, from Ethiopia, does not fit in latin1
	QVERIFY( m_sqliteDatabase->ok() );
	RecipeDB::IdType i = m_sqliteDatabase->addProperty( moose2, QString("mL"));

	QVERIFY( i > 0 );
	
	IngredientProperty p = m_sqliteDatabase->propertyName( i );
	QVERIFY2( p.units == QString("mL"), "Units have changed." );
	//FIXME: this would not work due to the bad encoding handling in Krecipes,
	//it's going to take time to fix it, so comment it out temporalily.
	//QVERIFY2( p.name == moose2, "Property name has been mangled." );

	if ( i > 0 ) {
		m_sqliteDatabase->removeProperty( i );
	}

}

void TestDatabaseEncoding::testInsertPropertyLatin1_SQLite()
{
	testInsertPropertyLatin1();
}

void TestDatabaseEncoding::testRetrievePropertyLatin1_SQLite()
{
	testRetrievePropertyLatin1();
}

void TestDatabaseEncoding::testInsertPropertyUTF8_SQLite()
{
	testInsertPropertyUTF8();
}

void TestDatabaseEncoding::testRetrievePropertyUTF8_SQLite()
{
	testRetrievePropertyUTF8();
}

#ifdef KRE_TESTS_MYSQL
void TestDatabaseEncoding::testInsertPropertyLatin1_MySQL()
{
	testInsertPropertyLatin1();
}

void TestDatabaseEncoding::testRetrievePropertyLatin1_MySQL()
{
	testRetrievePropertyLatin1();
}

void TestDatabaseEncoding::testInsertPropertyUTF8_MySQL()
{
	testInsertPropertyUTF8();
}

void TestDatabaseEncoding::testRetrievePropertyUTF8_MySQL()
{
	testRetrievePropertyUTF8();
}
#endif

#ifdef KRE_TESTS_POSTGRESQL
void TestDatabaseEncoding::testInsertPropertyLatin1_PostgreSQL()
{
	testInsertPropertyLatin1();
}

void TestDatabaseEncoding::testRetrievePropertyLatin1_PostgreSQL()
{
	testRetrievePropertyLatin1();
}

void TestDatabaseEncoding::testInsertPropertyUTF8_PostgreSQL()
{
	testInsertPropertyUTF8();
}

void TestDatabaseEncoding::testRetrievePropertyUTF8_PostgreSQL()
{
	testRetrievePropertyUTF8();
}
#endif


QTEST_MAIN(TestDatabaseEncoding)

#include "encoding.moc"
