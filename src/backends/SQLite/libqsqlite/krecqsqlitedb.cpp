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


#include "krecqsqlitedb.h"
#include "krecqsqliteresult.h"

#include <q3valuelist.h>
//Added by qt3to4:
#include <QByteArray>

#include <kdebug.h>

#include <config-krecipes.h>
#ifdef HAVE_SQLITE
#include <sqlite.h>
#include <cstdlib>
#elif HAVE_SQLITE3
#include <sqlite3.h>
#endif

QSQLiteDB::QSQLiteDB( QObject *, const char * )
{}

bool QSQLiteDB::open( const QString &dbname )
{
#ifdef HAVE_SQLITE
	char * errmsg = 0;
	if ( (m_db = sqlite_open( dbname.toLatin1(), 0, &errmsg )) == 0L ) {
		free( errmsg );
		return false;
	}
#elif HAVE_SQLITE3
	if ( sqlite3_open( dbname.toLatin1(), &m_db ) != SQLITE_OK )
		return false;
#endif

	int func_res;
	#ifdef HAVE_SQLITE
	func_res = sqlite_create_function(m_db,"lastInsertID",0,&lastInsertID,m_db );
	#elif HAVE_SQLITE3
	func_res = sqlite3_create_function(m_db,"lastInsertID",0,SQLITE_ANY,m_db,
	   &lastInsertID, 0, 0 );
	#endif

	return func_res == 0;
}

void QSQLiteDB::close()
{
	if ( m_db ) {
#ifdef HAVE_SQLITE
		sqlite_close( m_db );
#elif HAVE_SQLITE3

		sqlite3_close( m_db );
#endif

		m_db = 0L;
	}
}

QSQLiteResult QSQLiteDB::executeQuery( const QString &query, int *lastID )
{
	QSQLiteResult res;
	if ( !m_db ) {
		return res;
	}

	char *errmsg = 0;
#ifdef HAVE_SQLITE

	if ( sqlite_exec( m_db, query.toLatin1(), &call_back, &res, &errmsg ) > 0 )
#elif HAVE_SQLITE3

	if ( sqlite3_exec( m_db, query.toLatin1(), &call_back, &res, &errmsg ) > 0 )
#endif

	{
		kDebug() << "SQLite error: " << errmsg << endl <<
		"\t (Query: " << query << ")" ;
		res.setError( QString(errmsg) );
		res.setStatus( QSQLiteResult::Failure );

#ifdef HAVE_SQLITE
		sqlite_freemem( errmsg );
#elif HAVE_SQLITE3
		sqlite3_free( errmsg );
#endif
	}
	if ( lastID ) {
#ifdef HAVE_SQLITE
		* lastID = sqlite_last_insert_rowid( m_db );
#elif HAVE_SQLITE3

		*lastID = sqlite3_last_insert_rowid( m_db );
#endif

	}
	res.setStatus( QSQLiteResult::Success );
	return res;
}

int QSQLiteDB::call_back( void* result, int argc, char** argv, char** columns )
{
	QSQLiteResult * res = ( QSQLiteResult* ) result;

	QMap<QString, QByteArray> tableString;
	QMap<int, QByteArray> tableInt;

	for ( int i = 0; i < argc; i++ ) {
		tableInt.insert( i, argv[ i ] );
		tableString.insert( columns[ i ],
		                    argv[ i ] );
	}

	QSQLiteResultRow row( tableString, tableInt );
	res->addRow( row );

	return 0;
}

#ifdef HAVE_SQLITE
void QSQLiteDB::lastInsertID(sqlite_func *context,int argc,const char**)
{
	Q_ASSERT( argc==0 );

	void *db = sqlite_user_data(context);
	sqlite_set_result_int(context, sqlite_last_insert_rowid( (sqlite*)db ) );
}
#elif HAVE_SQLITE3
void QSQLiteDB::lastInsertID( sqlite3_context *context, int argc, sqlite3_value ** )
{
	Q_ASSERT( argc==0 );

	void *db = sqlite3_user_data(context);
	sqlite3_result_int(context, sqlite3_last_insert_rowid( (sqlite3*)db ) );
}
#endif

