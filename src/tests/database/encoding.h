/***************************************************************************
 *   Copyright © 2015 Adriaan de Groot <groot@kde.org>                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#ifndef TEST_DATABASE_ENCODING_H
#define TEST_DATABASE_ENCODING_H

#include <QtCore/QObject>

class RecipeDB;

class TestDatabaseEncoding : public QObject
{
	Q_OBJECT;

private slots:
	void initTestCase();
	void cleanupTestCase();
	void testSourceEncoding();

	void testInsertPropertyLatin1_SQLite();
	void testRetrievePropertyLatin1_SQLite();
	void testInsertPropertyUTF8_SQLite();
	void testRetrievePropertyUTF8_SQLite();

#ifdef KRE_TESTS_MYSQL
	void testInsertPropertyLatin1_MySQL();
	void testRetrievePropertyLatin1_MySQL();
	void testInsertPropertyUTF8_MySQL();
	void testRetrievePropertyUTF8_MySQL();
#endif

#ifdef KRE_TESTS_POSTGRESQL
	void testInsertPropertyLatin1_PostgreSQL();
	void testRetrievePropertyLatin1_PostgreSQL();
	void testInsertPropertyUTF8_PostgreSQL();
	void testRetrievePropertyUTF8_PostgreSQL();
#endif

private:
	RecipeDB * createDatabase( const QString & configFilename );
	inline void testInsertPropertyLatin1();
	inline void testRetrievePropertyLatin1();
	inline void testInsertPropertyUTF8();
	inline void testRetrievePropertyUTF8();

	RecipeDB * m_sqliteDatabase;
#ifdef KRE_TESTS_MYSQL
	RecipeDB * m_mysqlDatabase;
#endif
#ifdef KRE_TESTS_POSTGRESQL
	RecipeDB * m_postgresqlDatabase;
#endif

};

#endif // TEST_DATABASE_ENCODING_H
