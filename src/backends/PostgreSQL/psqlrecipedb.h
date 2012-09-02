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
	explicit PSqlRecipeDB( const QString& host, const QString& user = QString(), const QString& pass = QString(), const QString& DBName = DEFAULT_DB_NAME, int port = 0 );
	~PSqlRecipeDB( void );

	void initializeData();

	void createTable( const QString &tableName );

	virtual int maxAuthorNameLength() const;
	virtual int maxCategoryNameLength() const;
	virtual int maxIngredientNameLength() const;
	virtual int maxIngGroupNameLength() const;
	virtual int maxRecipeTitleLength() const;
	virtual int maxUnitNameLength() const;
	virtual int maxPrepMethodNameLength() const;
	virtual int maxPropertyNameLength() const;
	virtual int maxYieldTypeLength() const;

	void givePermissions( const QString &dbName, const QString &username, const QString &password, const QString &clientHost );
	
	float databaseVersion( void );	

protected:
	virtual QString qsqlDriverPlugin() const
	{
		return PSQL_DRIVER;
	}
	virtual int getNextInsertID( const QString &table, const QString &column );
	virtual RecipeDB::IdType lastInsertId( const QSqlQuery &query );

	virtual void empty( void );

private:
	void portOldDatabases( float version );
	QStringList backupCommand() const;
	QStringList restoreCommand() const;

	void addColumn( const QString &new_table_sql, const QString &new_col_info, const QString &default_value, const QString &table_name, int col_index );

	int last_insert_id;
};




#endif
