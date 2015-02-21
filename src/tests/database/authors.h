/***************************************************************************
 *   Copyright © 2012 José Manuel Santamaría Lema <panfaust@gmail.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#ifndef TEST_DATABASE_AUTHORS_H
#define TEST_DATABASE_AUTHORS_H

#include <QtCore/QObject>

class RecipeDB;

class TestDatabaseAuthors : public QObject
{
	Q_OBJECT;

private slots:
	void initTestCase();
	void cleanupTestCase();
	void testCreateSQLite();
	void testCreateSQLite_data();
private:
	RecipeDB * createDatabase( const QString & configFilename );
	inline void createAuthors( RecipeDB * database, QString name, int expectedId );

	RecipeDB * m_sqliteDatabase;
};

#endif // TEST_DATABASE_AUTHORS_H
