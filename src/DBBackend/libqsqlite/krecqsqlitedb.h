 /***************************************************************************
 *   Copyright (C) 2003 by qsqlite.sourceforge.net authors                 *
 *                                                                         *
 *                                                                         *
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

#include <sqlite.h>

#include "krecqsqliteresult.h"


class QSQLiteDB
{
public:
	QSQLiteDB(QObject *parent = 0, const char *name = 0);
	bool open(const QString &dbname);
	void close();
	QSQLiteResult executeQuery(const QString &query);

private:
	static int call_back(void* res, int argc, char** argv, char** columns);

	sqlite *m_db;
};

#endif
