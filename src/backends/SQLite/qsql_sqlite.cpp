/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "qsql_sqlite.h"

#include <qdatetime.h>
#include <qregexp.h>
#include <qfile.h>

#include <qptrvector.h>
#include <unistd.h>

#include <kdebug.h>

#include "libqsqlite/krecqsqlitedb.h"
#include "libqsqlite/krecqsqliteresult.h"

#define QSQLITE_DRIVER_NAME "KRE_QSQLITE"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_SQLITE3
#include <sqlite3.h>
#define sqlite_free sqlite3_free
#elif HAVE_SQLITE
#include <sqlite.h>
#endif

class KreSQLiteResult : public QSqlResult
{
public:
	KreSQLiteResult( const KreSQLiteDriver *d ): QSqlResult( d )
	{
		db = d->db;
	}
	~KreSQLiteResult() {}
protected:
	QVariant    data( int );
	bool        reset ( const QString& );
	bool        fetch( int );
	bool        fetchFirst();
	bool        fetchNext();
	bool        fetchLast();
	bool        isNull( int );
	QSqlRecord  record();
	int         size();
	int         numRowsAffected();

private:
	QSQLiteResult result;
	QSQLiteResultRow row;

	QSQLiteDB *db;
};


QVariant KreSQLiteResult::data( int field )
{
	if ( !isSelect() ) {
		//qWarning( "KreSQLiteResult::data: column %d out of range", field );
		qWarning( "KreSQLiteResult::data: not a select statement" );
		return QVariant();
	}
	
	return QVariant(row.data(field));
}

bool KreSQLiteResult::fetch( int i )
{
kdDebug()<<"fetch_i"<<endl;
	if ( isForwardOnly() ) { // fake a forward seek
		if ( at() < i ) {
			int x = i - at();
			while ( --x && fetchNext() );
			return fetchNext();
		} else {
			return FALSE;
		}
	}
	if ( at() == i )
		return TRUE;

	row = result.first();
	for ( int j = 0; j < i; ++j ) {
		if ( result.atEnd() )
			return FALSE;

		row = result.next();
	}

	setAt( i );
	return TRUE;
}

bool KreSQLiteResult::fetchNext()
{
	row = result.next();

	if ( result.atEnd() )
		return false;

	setAt( at() + 1 );
	return TRUE;
}

bool KreSQLiteResult::fetchFirst()
{
	row = result.first();

	if ( result.atEnd() )
		return false;

	setAt(0);
	return true;
}

bool KreSQLiteResult::fetchLast() 
{kdDebug()<<"fetchlast"<<endl;
	if ( isForwardOnly() ) { // fake this since MySQL can't seek on forward only queries
		bool success = fetchNext(); // did we move at all?
		while ( fetchNext() );
		return success;
	}
	int numRows = size();
	if ( !numRows )
		return FALSE;
	return fetch( numRows - 1 );
}

bool KreSQLiteResult::isNull( int i )
{
	return FALSE;
}

QSqlRecord KreSQLiteResult::record()
{kdDebug()<<"record"<<endl;
	return QSqlRecord();
}

int KreSQLiteResult::size()
{
//kdDebug()<<"size: "<<result.size()<<endl;
	return result.size();
}

int KreSQLiteResult::numRowsAffected()
{kdDebug()<<"numrowsaffected"<<endl;
	return 1;/*sqlite3_changes(db)*/
}

/*
   Execute \a query.
*/
bool KreSQLiteResult::reset(const QString& query)
{
	// this is where we build a query.
	if (!driver())
		return FALSE;
	if (!driver()-> isOpen() || driver()->isOpenError())
		return FALSE;

	//cleanup
	setAt( -1 );

	setSelect(true);


	result = db->executeQuery( query );
	int res = result.getStatus();

	if (res != SQLITE_OK ) {
		setLastError(QSqlError("Unable to execute statement", result.getError(), QSqlError::Statement, res));
	}

	setActive(TRUE);
	return TRUE;
}

/////////////////////////////////////////////////////////

KreSQLiteDriver::KreSQLiteDriver(QObject * parent, const char * name)
    : QSqlDriver(parent, name ? name : QSQLITE_DRIVER_NAME)
{
}

KreSQLiteDriver::KreSQLiteDriver(QSQLiteDB *connection, QObject *parent, const char *name)
    : QSqlDriver(parent, name ? name : QSQLITE_DRIVER_NAME)
{
	db = connection;
	setOpen(TRUE);
	setOpenError(FALSE);
}


KreSQLiteDriver::~KreSQLiteDriver()
{
}

bool KreSQLiteDriver::hasFeature(DriverFeature f) const
{
	switch (f) {
	case QuerySize:
	case Transactions:
		return TRUE;
	//   case BLOB:
	default:
		return FALSE;
	}
}

/*
   SQLite dbs have no user name, passwords, hosts or ports.
   just file names.
*/
bool KreSQLiteDriver::open(const QString & file, const QString &, const QString &, const QString &, int)
{
	if (isOpen())
		close();
	
	if (file.isEmpty())
		return FALSE;
	
	db = new QSQLiteDB;
	if ( !db->open(QFile::encodeName(file)) ) {
		setLastError(QSqlError("Error to open database", 0, QSqlError::Connection));
		setOpenError(true);
		return false;
	}
	
	if (db) {
		setOpen(TRUE);
		setOpenError(FALSE);
		return TRUE;
	}
	setOpenError(TRUE);
	return FALSE;
}

void KreSQLiteDriver::close()
{
    if (isOpen()) {
        db->close();
        delete db; db = 0;
        setOpen(FALSE);
        setOpenError(FALSE);
    }
}

QSqlQuery KreSQLiteDriver::createQuery() const
{
    return QSqlQuery(new KreSQLiteResult(this));
}

bool KreSQLiteDriver::beginTransaction()
{
	if (!isOpen() || isOpenError())
		return FALSE;
	
	QSQLiteResult result = db->executeQuery( "BEGIN" );
	int status = result.getStatus();
	if (status == QSQLiteResult::Success)
		return TRUE;
	
	setLastError(QSqlError("Unable to begin transaction", result.getError(), QSqlError::Transaction, status));
	return FALSE;
}

bool KreSQLiteDriver::commitTransaction()
{
	if (!isOpen() || isOpenError())
		return FALSE;
	
	QSQLiteResult result = db->executeQuery( "COMMIT" );
	int status = result.getStatus();
	if (status == QSQLiteResult::Success)
		return TRUE;
	
	setLastError(QSqlError("Unable to commit transaction", result.getError(), QSqlError::Transaction, status));
	return FALSE;
}

bool KreSQLiteDriver::rollbackTransaction()
{
	if (!isOpen() || isOpenError())
		return FALSE;
	
	QSQLiteResult result = db->executeQuery( "ROLLBACK" );
	int status = result.getStatus();
	if (status == SQLITE_OK)
		return TRUE;
	
	setLastError(QSqlError("Unable to rollback transaction", result.getError(), QSqlError::Transaction, status));
	return FALSE;
}

QStringList KreSQLiteDriver::tables(const QString &typeName) const
{
	QStringList res;
	if (!isOpen())
		return res;

	int type = typeName.toInt();
	
	QSqlQuery q = createQuery();
	q.setForwardOnly(TRUE);
	#if (QT_VERSION-0 >= 0x030000)
	if ((type & (int)QSql::Tables) && (type & (int)QSql::Views))
		q.exec("SELECT name FROM sqlite_master WHERE type='table' OR type='view'");
	else if (typeName.isEmpty() || (type & (int)QSql::Tables))
		q.exec("SELECT name FROM sqlite_master WHERE type='table'");
	else if (type & (int)QSql::Views)
		q.exec("SELECT name FROM sqlite_master WHERE type='view'");
	#else
		q.exec("SELECT name FROM sqlite_master WHERE type='table' OR type='view'");
	#endif
	
	if (q.isActive()) {
		while(q.next()) {
			res.append(q.value(0).toString());
		}
	}
	
	#if (QT_VERSION-0 >= 0x030000)
	if (type & (int)QSql::SystemTables) {
		// there are no internal tables beside this one:
		res.append("sqlite_master");
	}
	#endif
	
	return res;
}

QSqlIndex KreSQLiteDriver::primaryIndex(const QString &tblname) const
{
    QSqlRecordInfo rec(recordInfo(tblname)); // expensive :(

    if (!isOpen())
        return QSqlIndex();

    QSqlQuery q = createQuery();
    q.setForwardOnly(TRUE);
    // finrst find a UNIQUE INDEX
    q.exec("PRAGMA index_list('" + tblname + "');");
    QString indexname;
    while(q.next()) {
	if (q.value(2).toInt()==1) {
	    indexname = q.value(1).toString();
	    break;
	}
    }
    if (indexname.isEmpty())
	return QSqlIndex();

    q.exec("PRAGMA index_info('" + indexname + "');");

    QSqlIndex index(tblname, indexname);
    while(q.next()) {
	QString name = q.value(2).toString();
	QSqlVariant::Type type = QSqlVariant::Invalid;
	if (rec.contains(name))
	    type = rec.find(name).type();
	index.append(QSqlField(name, type));
    }
    return index;
}

#if 0
QSqlRecordInfo KreSQLiteDriver::recordInfo(const QString &tbl) const
{
	if (!isOpen())
		return QSqlRecordInfo();
	
	QSqlQuery q = createQuery();
	q.setForwardOnly(TRUE);
	q.exec("SELECT * FROM " + tbl + " LIMIT 1");
return QSqlRecordInfo();
//	return recordInfo(q);
}

QSqlRecord KreSQLiteDriver::record(const QString &tblname) const
{
    if (!isOpen())
        return QSqlRecord();

    return recordInfo(tblname).toRecord();
}

QSqlRecord KreSQLiteDriver::record(const QSqlQuery& query) const
{
    if (query.isActive() && query.driver() == this) {
        KreSQLiteResult* result = (KreSQLiteResult*)query.result();
        return result->rInf.toRecord();
    }
    return QSqlRecord();
}

QSqlRecordInfo KreSQLiteDriver::recordInfo(const QSqlQuery& query) const
{
    if (query.isActive() && query.driver() == this) {
        KreSQLiteResult* result = (KreSQLiteResult*)query.result();
        return result->rInf;
    }
    return QSqlRecordInfo();
}

//this would be used below in formatValue()
static QString escape( const QString &s )
{
	QString s_escaped = s;

	if ( !s_escaped.isEmpty() ) { //###: sqlite_mprintf() seems to fill an empty string with garbage
		// Escape using SQLite's function
#if HAVE_SQLITE
		char * escaped = sqlite_mprintf( "%q", s.latin1() ); // Escape the string(allocates memory)
#elif HAVE_SQLITE3
		char * escaped = sqlite3_mprintf( "%q", s.latin1() ); // Escape the string(allocates memory)
#endif
		s_escaped = escaped;
#if HAVE_SQLITE
		sqlite_freemem( escaped ); // free allocated memory
#elif HAVE_SQLITE3
		sqlite3_free( escaped ); // free allocated memory
#endif
	}

	return ( s_escaped );
}


// Everything is considered a string given the implementation of this driver (we don't have field info).  This would ruin a QByteArray (for the photo).
QString KreSQLiteDriver::formatValue( const QSqlField* field, bool trimStrings ) const
{
	QString r;
	if ( field->isNull() ) {
		r = nullText();
	} else {
		switch( field->type() ) {
			case QVariant::String:
			case QVariant::CString: {
				// Escape '\' characters
				r = QSqlDriver::formatValue( field );
				//r = escape(r);
				//kdDebug()<<"escaping sqlite string: "<<r<<endl;
				break;
			}
			default:
				r = QSqlDriver::formatValue( field, trimStrings );
		}
	}
	return r;
}
#endif

