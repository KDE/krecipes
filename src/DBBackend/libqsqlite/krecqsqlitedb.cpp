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


#include "krecqsqlitedb.h"
#include "krecqsqliteresult.h"

#include <qvaluelist.h>
#include <sqlite.h>
#include <stdlib.h>

QSQLiteDB::QSQLiteDB(QObject *, const char *)
{
}

bool QSQLiteDB::open(const QString &dbname)
{
	char *errmsg = 0;

	m_db = sqlite_open(dbname.latin1(), 0, &errmsg);

	if (m_db == 0L)
	{
        free(errmsg);
        return false;
    }
    return true;
}

void QSQLiteDB::close()
{
	if (m_db)
	{
		sqlite_close(m_db);
		m_db = 0L;
	}
}

QSQLiteResult QSQLiteDB::executeQuery(const QString &query, int *lastID)
{
	QSQLiteResult res;
	if (!m_db)
	{
		return res;
	}

	char *errmsg = 0;
	if (sqlite_exec(m_db, query.latin1(), &call_back, &res, &errmsg) > 0)
	{
		res.setError(errmsg);
		res.setStatus(QSQLiteResult::Failure);
        free(errmsg);
	}

	if (lastID)
	{
	*lastID=sqlite_last_insert_rowid(m_db);
	}

	return res;
}

int QSQLiteDB::call_back(void* result, int argc, char** argv, char** columns)
{
	QSQLiteResult *res = (QSQLiteResult*)result;

    QMap<QString, QString> tableString;
    QMap<int, QString> tableInt;

	for (int i = 0; i < argc; i++ )
	{
		tableInt.insert(i, QString::fromUtf8(argv[i]));
		tableString.insert(QString::fromUtf8(columns[i]),
	                       QString::fromUtf8(argv[i]));
	}

    QSQLiteResultRow row(tableString, tableInt);
	res->addRow(row);

	return 0;
}
