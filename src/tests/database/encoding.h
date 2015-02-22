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
	void testInsertProperty();
	void testRetrieveProperty();
private:
	RecipeDB * createDatabase( const QString & configFilename );
	RecipeDB * m_sqliteDatabase;
};

#endif // TEST_DATABASE_ENCODING_H
