 /**************************************************************************
 *                                                                         *
 *                Copyright (C) 2003 by the opie project,                  *
 *                    qsqlite.sourceforge.net authors                      *
 *                 and krecipes.courceforge.net authors                    *
 *                                                                         *
 * This code was originally developed by the opie project, on which the    *
 *           qsqlite.sourceforge.netproject based their work.              *
 * This file is a small extension to it, necessary to perform some minimum *
 * SQL actions)                                                            *
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
	QSQLiteResult executeQuery(const QString &query, int *lastID=0);
	int size(); //Returns the number of rows returned

private:
	static int call_back(void* res, int argc, char** argv, char** columns);

	sqlite *m_db;
};

#endif
