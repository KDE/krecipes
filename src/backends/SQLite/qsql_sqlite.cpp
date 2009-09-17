/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "qsql_sqlite.h"

#include <QFile>

#include <q3ptrvector.h>
//Added by qt3to4:
#include <QSqlField>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <unistd.h>

#include <kdebug.h>

#include "libqsqlite/krecqsqlitedb.h"
#include "libqsqlite/krecqsqliteresult.h"

#define QSQLITE_DRIVER_NAME "KRE_QSQLITE"

#include <config-krecipes.h>

#ifdef HAVE_SQLITE3
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
	QSqlRecord  record() const;
	int         size();
	int         numRowsAffected();

private:
	QSQLiteResult result;
	QSQLiteResultRow row;

	QSQLiteDB *db;
};


QVariant KreSQLiteResult::data( int field )
{
    kDebug()<<" field :"<<field;
	if ( !isSelect() ) {
		//qWarning( "KreSQLiteResult::data: column %d out of range", field );
		qWarning( "KreSQLiteResult::data: not a select statement" );
		return QVariant();
	}
        kDebug()<<" QVariant(row.data(field) :"<<QVariant(row.data(field) );
	return QVariant(row.data(field));
}

bool KreSQLiteResult::fetch( int i )
{
kDebug()<<"fetch_i";
	if ( isForwardOnly() ) { // fake a forward seek
		if ( at() < i ) {
			int x = i - at();
			while ( --x && fetchNext() );
			return fetchNext();
		} else {
			return false;
		}
	}
	if ( at() == i )
		return true;

	row = result.first();
	for ( int j = 0; j < i; ++j ) {
		if ( result.atEnd() )
			return false;

		row = result.next();
	}

	setAt( i );
	return true;
}

bool KreSQLiteResult::fetchNext()
{
        kDebug();
	row = result.next();
        kDebug()<<" before";
        kDebug()<< "result.atEnd() :"<<result.atEnd();
	if ( result.atEnd() )
		return false;

	setAt( at() + 1 );
	return true;
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
{
    kDebug()<<"fetchlast";
	if ( isForwardOnly() ) { // fake this since MySQL can't seek on forward only queries
		bool success = fetchNext(); // did we move at all?
		while ( fetchNext() );
		return success;
	}
	int numRows = size();
	if ( !numRows )
		return false;
	return fetch( numRows - 1 );
}

bool KreSQLiteResult::isNull( int /*i*/ )
{
	return false;
}

QSqlRecord KreSQLiteResult::record() const
{
    kDebug()<<"record";
	return QSqlRecord();
}

int KreSQLiteResult::size()
{
//kDebug()<<"size: "<<result.size();
	return result.size();
}

int KreSQLiteResult::numRowsAffected()
{
    kDebug()<<"numrowsaffected";
	return 1;/*sqlite3_changes(db)*/
}

/*
   Execute \a query.
*/
bool KreSQLiteResult::reset(const QString& query)
{
    kDebug()<<" query :"<<query;
	// this is where we build a query.
	if (!driver())
		return false;
        kDebug()<<" driver";
	if (!driver()-> isOpen() || driver()->isOpenError())
		return false;
        kDebug()<<" after";
	//cleanup
	setAt( -1 );

	setSelect(true);


	result = db->executeQuery( query );
        kDebug()<<" after executeQuery";
	int res = result.getStatus();

	if (res != SQLITE_OK ) {
		setLastError(QSqlError("Unable to execute statement", result.getError(), QSqlError::Statement, res));
	}
        kDebug()<<" active";
	setActive(true);
        kDebug()<<" after active db";
	return true;
}

/////////////////////////////////////////////////////////

KreSQLiteDriver::KreSQLiteDriver(QObject * parent, const char * name)
    : QSqlDriver(parent)
{
    setObjectName( name ? name : QSQLITE_DRIVER_NAME);
    kDebug()<<" objectName :"<<objectName();
}

KreSQLiteDriver::KreSQLiteDriver(QSQLiteDB *connection, QObject *parent, const char *name)
    : QSqlDriver(parent)
{
    setObjectName(  name ? name : QSQLITE_DRIVER_NAME);
    kDebug()<<" objectName "<<objectName();
	db = connection;
	setOpen(true);
	setOpenError(false);
}


KreSQLiteDriver::~KreSQLiteDriver()
{
}

bool KreSQLiteDriver::hasFeature(DriverFeature f) const
{
	switch (f) {
	case QuerySize:
	case Transactions:
		return true;
	//   case BLOB:
	default:
		return false;
	}
}

/*
   SQLite dbs have no user name, passwords, hosts or ports.
   just file names.
*/
bool KreSQLiteDriver::open(const QString & file, const QString &, const QString &, const QString &, int, const QString&)
{
    kDebug()<<"  KreSQLiteDriver::open file : "<<file;
	if (isOpen())
		close();

	if (file.isEmpty())
		return false;

        kDebug()<<" create new database";
	db = new QSQLiteDB;
        kDebug()<<" db :"<<db;
	if ( !db->open(QFile::encodeName(file)) ) {
		setLastError(QSqlError("Error to open database", 0, QSqlError::Connection));
		setOpenError(true);
		setOpen(false);
		return false;
	}
	setOpen(true);
	setOpenError(false);
        kDebug()<<"KreSQLiteDriver::open 'true' ";
	return true;
}

void KreSQLiteDriver::close()
{
    kDebug()<<" CLOSE !!!!!!!!!!!!!!!!!!!!!!!!!!!";
    if (isOpen()) {
        db->close();
        delete db; db = 0;
        setOpen(false);
        setOpenError(false);
    }
}

QSqlQuery KreSQLiteDriver::createQuery() const
{
    kDebug();
    return QSqlQuery(new KreSQLiteResult(this));
}

QSqlResult* KreSQLiteDriver::createResult() const
{
    kDebug();
    return new KreSQLiteResult(this);
}


bool KreSQLiteDriver::beginTransaction()
{
    kDebug();
	if (!isOpen() || isOpenError())
		return false;

	QSQLiteResult result = db->executeQuery( "BEGIN" );
	int status = result.getStatus();
	if (status == QSQLiteResult::Success)
		return true;

	setLastError(QSqlError("Unable to begin transaction", result.getError(), QSqlError::Transaction, status));
	return false;
}

bool KreSQLiteDriver::commitTransaction()
{
    kDebug();
	if (!isOpen() || isOpenError())
		return false;

	QSQLiteResult result = db->executeQuery( "COMMIT" );
	int status = result.getStatus();
	if (status == QSQLiteResult::Success)
		return true;

	setLastError(QSqlError("Unable to commit transaction", result.getError(), QSqlError::Transaction, status));
	return false;
}

bool KreSQLiteDriver::rollbackTransaction()
{
    kDebug();
	if (!isOpen() || isOpenError())
		return false;

	QSQLiteResult result = db->executeQuery( "ROLLBACK" );
	int status = result.getStatus();
	if (status == SQLITE_OK)
		return true;

	setLastError(QSqlError("Unable to rollback transaction", result.getError(), QSqlError::Transaction, status));
	return false;
}

QStringList KreSQLiteDriver::tables(QSql::TableType typeTable) const
{
    kDebug();
	QStringList res;
	if (!isOpen())
		return res;


	QSqlQuery q = createQuery();
	q.setForwardOnly(true);
	if ((typeTable & QSql::Tables) && (typeTable & QSql::Views))
		q.exec("SELECT name FROM sqlite_master WHERE type='table' OR type='view'");
	else if ((typeTable & QSql::Tables))
		q.exec("SELECT name FROM sqlite_master WHERE type='table'");
	else if (typeTable & QSql::Views)
		q.exec("SELECT name FROM sqlite_master WHERE type='view'");

	if (q.isActive()) {
		while(q.next()) {
			res.append(q.value(0).toString());
		}
	}

	if (typeTable & QSql::SystemTables) {
		// there are no internal tables beside this one:
		res.append("sqlite_master");
	}

	return res;
}

QSqlIndex KreSQLiteDriver::primaryIndex(const QString &tblname) const
{
    kDebug();
    QSqlRecord rec(record(tblname)); // expensive :(

    if (!isOpen())
        return QSqlIndex();

    QSqlQuery q = createQuery();
    q.setForwardOnly(true);
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
	    type = rec.value(name).type();
	index.append(QSqlField(name, type));
    }
    return index;
}

#if 0
QSqlRecord KreSQLiteDriver::recordInfo(const QString &tbl) const
{
	if (!isOpen())
		return QSqlRecord();

	QSqlQuery q = createQuery();
	q.setForwardOnly(true);
	q.exec("SELECT * FROM " + tbl + " LIMIT 1");
return QSqlRecord();
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

QSqlRecord KreSQLiteDriver::recordInfo(const QSqlQuery& query) const
{
    if (query.isActive() && query.driver() == this) {
        KreSQLiteResult* result = (KreSQLiteResult*)query.result();
        return result->rInf;
    }
    return QSqlRecord();
}

//this would be used below in formatValue()
static QString escape( const QString &s )
{
	QString s_escaped = s;

	if ( !s_escaped.isEmpty() ) { //###: sqlite_mprintf() seems to fill an empty string with garbage
		// Escape using SQLite's function
#ifdef HAVE_SQLITE
		char * escaped = sqlite_mprintf( "%q", s.toLatin1() ); // Escape the string(allocates memory)
#elif HAVE_SQLITE3
		char * escaped = sqlite3_mprintf( "%q", s.toLatin1() ); // Escape the string(allocates memory)
#endif
		s_escaped = escaped;
#ifdef HAVE_SQLITE
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
				//kDebug()<<"escaping sqlite string: "<<r;
				break;
			}
			default:
				r = QSqlDriver::formatValue( field, trimStrings );
		}
	}
	return r;
}
#endif

