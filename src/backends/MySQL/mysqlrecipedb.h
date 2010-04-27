/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
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
	explicit MySQLRecipeDB( const QString &host, const QString &user = QString(), const QString &pass = QString(), const QString &DBName = DEFAULT_DB_NAME, int port = 0 );
	~MySQLRecipeDB( void );

	int lastInsertID();

	void createTable( const QString &tableName );
	void givePermissions( const QString &dbName, const QString &username, const QString &password = QString(), const QString &clientHost = "localhost" );

protected:
	QString qsqlDriverPlugin() const
	{
		return MYSQL_DRIVER;
	}

private:
	void portOldDatabases( float version );
	QStringList backupCommand() const;
	QStringList restoreCommand() const;
};




#endif
