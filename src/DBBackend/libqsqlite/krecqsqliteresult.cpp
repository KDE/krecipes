 /**************************************************************************
 *               Copyright (C) 2003 by the opie project and                *
 *                    qsqlite.sourceforge.net authors                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "krecqsqliteresult.h"

QSQLiteResultRow::QSQLiteResultRow( const TableString& string, const TableInt& Int)
 : m_string( string ), m_int( Int )
{
}

QSQLiteResultRow::~QSQLiteResultRow()
{
}

QSQLiteResultRow::QSQLiteResultRow( const QSQLiteResultRow& item)
{
    *this = item;
}

QSQLiteResultRow &QSQLiteResultRow::operator=( const QSQLiteResultRow& other)
{
    m_string = other.m_string;
    m_int = other.m_int;
    return *this;
}

QSQLiteResultRow::TableString QSQLiteResultRow::tableString() const
{
    return m_string;
}

QSQLiteResultRow::TableInt QSQLiteResultRow::tableInt() const
{
    return m_int;
}

QString QSQLiteResultRow::data( const QString& columnName, bool *ok )
{
    TableString::Iterator it = m_string.find( columnName );

    /* if found */
    if ( it != m_string.end() )
	{
        if ( ok ) *ok = true;
        return it.data();
    }
	else
	{
        if ( ok )  *ok = false;
        return QString::null;
    }

}

QString QSQLiteResultRow::data( int column, bool *ok )
{
    TableInt::Iterator it = m_int.find( column );

    // if found
    if ( it != m_int.end() ) {
        if ( ok ) *ok = true;
        return it.data();
    }
	else
	{
        if ( ok )  *ok = false;
        return QString::null;
    }
}

/*
 * DateFormat is 'YYYY-MM-DD'
 */
QDate QSQLiteResultRow::dataToDate( const QString& column, bool *ok )
{
    QDate date = QDate::currentDate();
    QString str = data( column,  ok );
    if (!str.isEmpty() ) {
      ;// convert
    }
    return date;
}

QDate QSQLiteResultRow::dataToDate( int column,  bool *ok )
{
    QDate date = QDate::currentDate();
    QString str = data( column,  ok );
    if (!str.isEmpty() ) {
      ;// convert
    }
    return date;
}

QDateTime QSQLiteResultRow::dataToDateTime( const QString& column )
{
    QDateTime time = QDateTime::currentDateTime();
    return time;
}

QDateTime QSQLiteResultRow::dataToDateTime( int column )
{
    QDateTime time = QDateTime::currentDateTime();
    return time;
}

QSQLiteResult::QSQLiteResult( enum Status status,
                              const QSQLiteResult::Columns& list,
	                          const QString &error )
 : m_status( status ), m_list( list ), m_error( error )
{
}

QSQLiteResult::~QSQLiteResult()
{
}

QSQLiteResult::Status QSQLiteResult::getStatus() const
{
    return m_status;
}

void QSQLiteResult::setStatus( QSQLiteResult::Status status )
{
    m_status = status;
}

QSQLiteResult::Columns QSQLiteResult::getResults() const
{
    return m_list;
}

void QSQLiteResult::setResults( const QSQLiteResult::Columns& result )
{
    m_list = result;
}

void QSQLiteResult::addRow(QSQLiteResultRow row)
{
	m_list.append(row);
}

QString QSQLiteResult::getError() const
{
	return m_error;
}

void QSQLiteResult::setError(const QString &error)
{
	m_error = error;
}

QSQLiteResultRow QSQLiteResult::first()
{
    it = m_list.begin();
    return (*it);
}

QSQLiteResultRow QSQLiteResult::next()
{
    ++it;
    return (*it);
}

bool QSQLiteResult::atEnd()
{
    if ( it == m_list.end() )
	{
        return true;
	}

    return false;
}

QSQLiteResult::Columns::ConstIterator QSQLiteResult::iterator() const
{
    QSQLiteResult::Columns::ConstIterator it;
    it = m_list.begin();
    return it;
}
