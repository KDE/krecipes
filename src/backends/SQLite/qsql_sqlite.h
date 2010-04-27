/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef QSQL_SQLITE_H
#define QSQL_SQLITE_H

#include <qsqldriver.h>
#include <qsqlresult.h>
#include <qsqlrecord.h>
#include <qsqlindex.h>
//Added by qt3to4:
#include <QSqlQuery>

typedef QVariant QSqlVariant;

class QSQLiteDB;

class KreSQLiteDriver : public QSqlDriver
{
public:
	explicit KreSQLiteDriver(QObject * parent = 0, const char * name = 0);
	explicit KreSQLiteDriver(QSQLiteDB *connection, QObject *parent = 0, const char *name = 0);
	~KreSQLiteDriver();

	bool    hasFeature( DriverFeature ) const;
	bool    open( const QString&,
			const QString&,
			const QString&,
			const QString&,
			int,
			const QString&);
	void    close();
	QSqlQuery createQuery() const;
	QSqlResult* createResult () const;

	bool beginTransaction();
	bool commitTransaction();
	bool rollbackTransaction();
	QStringList tables(QSql::TableType) const;
	QSqlIndex primaryIndex(const QString &tblname) const;

	//QSqlRecordInfo recordInfo(const QString &tbl) const;
	//QSqlRecord record(const QString &tblname) const;
	//QString formatValue( const QSqlField* field, bool trimStrings ) const;

private:
	friend class KreSQLiteResult;

	QSQLiteDB *db;
};
#endif
