#include "krecqsqlitedb.h"
#include "krecqsqliteresult.h"

#include <qvaluelist.h>
#include <sqlite.h>
#include <stdlib.h>

QSQLiteDB::QSQLiteDB(QObject *parent, const char *name)
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

QSQLiteResult QSQLiteDB::executeQuery(const QString &query)
{
	QSQLiteResult *res = new QSQLiteResult();

	if (!m_db)
	{
		return *res;		
	}

	char *errmsg = 0;
	if (sqlite_exec(m_db, query.latin1(), &call_back, res, &errmsg) > 0)
	{
		res->setError(errmsg);
		res->setStatus(QSQLiteResult::Failure);
        free(errmsg);
	}

	return *res;		
}

int QSQLiteDB::call_back(void* result, int argc, char** argv, char** columns)
{
	QSQLiteResult *res = (QSQLiteResult*)result;

    QMap<QString, QString> tableString;
    QMap<int, QString> tableInt;

	for (int i = 0; i < argc; i++ )
	{
		tableInt.insert(i, QString::fromLocal8Bit(argv[i]));
		tableString.insert(QString::fromLocal8Bit(columns[i]),
	                       QString::fromLocal8Bit(argv[i]));
	}

    QSQLiteResultRow row(tableString, tableInt);
	res->addRow(row);

	return 0;
}
