/***************************************************************************
 *   Copyright © 2012 José Manuel Santamaría Lema <panfaust@gmail.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include "authors.h"

#include "backends/recipedb.h"

#include <QtTest/QTest>

#include <QDebug>


void TestDatabaseAuthors::initTestCase()
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


void TestDatabaseAuthors::cleanupTestCase()
{
	if ( m_sqliteDatabase ) {
		delete m_sqliteDatabase;
	}
}


void TestDatabaseAuthors::createAuthors( RecipeDB * database, QString name, int expectedId )
{
	RecipeDB::IdType last_insert_id = database->createNewAuthor( name );
	qDebug() << last_insert_id << RecipeDB::InvalidId;
	QVERIFY( last_insert_id != RecipeDB::InvalidId );
	QCOMPARE( last_insert_id, expectedId );

	RecipeDB::IdType author_id = database->findExistingAuthorByName( name );
	qDebug() << author_id;
	QVERIFY( author_id != RecipeDB::InvalidId );
	QCOMPARE( last_insert_id, author_id);
}

void TestDatabaseAuthors::testCreateSQLite_data() 
{
	  QTest::addColumn<QString>("name");
	  QTest::addColumn<int>("expectedId");
	  QTest::newRow("Ethan") << QString("Ethan") << 1;
	  QTest::newRow("Et'han") << QString("Et'han") << 2;
	  QTest::newRow("John") << QString("John") << 3;
	  QTest::newRow("Martin") << QString("Martin") << 4;
}

void TestDatabaseAuthors::testCreateSQLite()
{
	QFETCH(QString, name);
	QFETCH(int, expectedId);
	createAuthors( m_sqliteDatabase, name, expectedId );
	m_sqliteDatabase->checkIntegrity();
}


QTEST_MAIN(TestDatabaseAuthors)

#include "authors.moc"
