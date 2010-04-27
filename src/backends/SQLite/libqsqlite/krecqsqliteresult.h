/***************************************************************************
*                                                                         *
* Copyright © 2003 Unai Garro <ugarro@gmail.com>                          *
* Copyright © 2003 Martin Imobersteg <imm@gmx.ch>                         *
* Copyright © 2003 Opie Project <opie-devel@handhelds.org>                *
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

#ifndef QSQLITERESULT_H
#define QSQLITERESULT_H

#include <qmap.h>
#include <q3valuelist.h>

/**
 * ResultRow  represents one row of the resulting answer
 */
class QSQLiteResultRow
{
public:
	/**
	 * TableString is used to establish the relations
	 * between the column name and the real item
	 */
	typedef QMap<QString, QByteArray> TableString;

	/**
	 * TableInt is used to establish a relation between a
	 * position of a column and the row value
	 */
	typedef QMap<int, QByteArray> TableInt;

	/**
	 * Default c'tor. It has a TableString and a TableInt
	 */
	explicit QSQLiteResultRow( const TableString& = TableString(),
	                  const TableInt& = TableInt() );
	QSQLiteResultRow( const QSQLiteResultRow& );
	~QSQLiteResultRow();
	QSQLiteResultRow &operator=( const QSQLiteResultRow& );

	/**
	 * returns the TableString
	 */
	TableString tableString() const;

	/**
	 * returns the TableInt
	 */
	TableInt tableInt() const;

	/**
	 * retrieves the Data from columnName
	 *
	 */
	QByteArray data( const QString& columnName, bool *ok = 0 );

	/**
	 * QString for column number
	 */
	QByteArray data( int columnNumber, bool *ok = 0 );

private:
	TableString m_string;
	TableInt m_int;
};

/**
 * the QSQLiteResult
 * either a SQL statusment failed or succeeded
 */
class QSQLiteResult
{

public:
	typedef Q3ValueList<QSQLiteResultRow> Columns;

	/** The Status of a Result */
	enum Status{ Success = 0, Failure, Undefined };

	/**
	 * default c'tor
	 * @param status The Status of the Result
	 * @param r ResultItems
	 * @param error Error Message
	 */
	explicit QSQLiteResult( enum Status status = Undefined,
	               const Columns &r = Columns(),
	               const QString &error = 0L );
	~QSQLiteResult();

	Status getStatus() const;
	Columns getResults() const;
	QString getError() const;

	void setStatus( enum Status status );
	void setResults( const Columns &result );
	void setError( const QString &error );

	void addRow( QSQLiteResultRow row );

	QSQLiteResultRow first();
	QSQLiteResultRow next();
	bool atEnd();

	Columns::ConstIterator iterator() const;
	int size() const;

private:
	enum Status m_status;
	Columns m_list;
	QString m_error;
	Columns::Iterator it;
};

#endif
