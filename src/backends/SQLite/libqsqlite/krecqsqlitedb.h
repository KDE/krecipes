/***************************************************************************
*                                                                         *
*                           Copyright (C) 2003                            *
*                by Unai Garro (ugarro@users.sourceforge.net)             *
*                       Martin Imobersteg <imm@gmx.ch>                    *
*                          and opie project                               *
*                                                                         *
*                                                                         *
*    This code was originally developed by the opie project, on which     *
*                    Martin Imobersteg based his work.                    *
* This file is adds a small extension, necessary to perform some minimum  * 
* SQL actions                                                             *
*                                                                         *
*         (this project is different from that in qsqlite.sf.net)         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef QSQLITEDB_H
#define QSQLITEDB_H

#include <qvaluelist.h>
#include <qstringlist.h>
#include <qobject.h>

#include "config.h"
#if HAVE_SQLITE
#include <sqlite.h>
#elif HAVE_SQLITE3
#include <sqlite3.h>
#endif

#include "krecqsqliteresult.h"


class QSQLiteDB
{
public:
	QSQLiteDB( QObject *parent = 0, const char *name = 0 );
	bool open( const QString &dbname );
	void close();
	QSQLiteResult executeQuery( const QString &query, int *lastID = 0 );
	int size(); //Returns the number of rows returned

private:
	static int call_back( void* res, int argc, char** argv, char** columns );

	#if HAVE_SQLITE
	static void lastInsertID(sqlite_func*,int,const char**);
	sqlite *m_db;
	#elif HAVE_SQLITE3
	static void lastInsertID(sqlite3_context *context, int argc, sqlite3_value **argv);
	sqlite3 *m_db;
	#endif
};

#endif
