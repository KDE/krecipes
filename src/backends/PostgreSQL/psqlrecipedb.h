/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/


#ifndef PSQLRECIPEDB_H
#define PSQLRECIPEDB_H

#define PSQL_DRIVER "QPSQL7"

#include "qsqlrecipedb.h"

/**
@author Jason Kivlighn
*/
class PSqlRecipeDB : public QSqlRecipeDB
{

	Q_OBJECT

private:
	void createDB( void );

public:
	PSqlRecipeDB( const QString host, const QString user = QString::null, const QString pass = QString::null, const QString DBName = DEFAULT_DB_NAME );
	~PSqlRecipeDB( void );

	int lastInsertID();
	void initializeData();

	void createTable( const QString &tableName );
	void givePermissions( const QString &dbName, const QString &username, const QString &password, const QString &clientHost );

protected:
	virtual QString qsqlDriverPlugin() const
	{
		return PSQL_DRIVER;
	}
	virtual int getNextInsertID( const QString &table, const QString &column );

	virtual void empty( void );

private:
	void portOldDatabases( float version );
	QStringList backupCommand() const;
	QStringList restoreCommand() const;

	void addColumn( const QString &new_table_sql, const QString &new_col_info, const QString &default_value, const QString &table_name, int col_index );

	int last_insert_id;
};




#endif
