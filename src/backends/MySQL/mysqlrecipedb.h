/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/


#ifndef MYSQLRECIPEDB_H
#define MYSQLRECIPEDB_H

#define MYSQL_DRIVER "QMYSQL3"

#include "qsqlrecipedb.h"

/**
@author Unai Garro
*/
class MySQLRecipeDB : public QSqlRecipeDB
{

	Q_OBJECT

private:
	void createDB( void );

public:
	MySQLRecipeDB( const QString &host, const QString &user = QString::null, const QString &pass = QString::null, const QString &DBName = DEFAULT_DB_NAME );
	~MySQLRecipeDB( void );

	int lastInsertID();

	void createTable( const QString &tableName );
	void givePermissions( const QString &dbName, const QString &username, const QString &password = QString::null, const QString &clientHost = "localhost" );

protected:
	QString qsqlDriver() const
	{
		return MYSQL_DRIVER;
	}

private:
	void portOldDatabases( float version );
	QStringList backupCommand() const;
};




#endif
