/***************************************************************************
*   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>            *
*   Copyright © 2003-2004 Cyril Bosselut <bosselut@b1project.com>         *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef LITERECIPEDB_H
#define LITERECIPEDB_H

#include "backends/qsqlrecipedb.h"

#include <QString>
#include <KDebug>

#define SQLITE_DRIVER "QSQLITE"

class LiteRecipeDB : public QSqlRecipeDB
{

	Q_OBJECT

private:
	void createDB( void );

public:
	LiteRecipeDB( const QString &DBName = DEFAULT_DB_NAME );
	~LiteRecipeDB( void );

	virtual void createTable( const QString &tableName );

	virtual int maxAuthorNameLength() const;
	virtual int maxCategoryNameLength() const;
	virtual int maxIngredientNameLength() const;
	virtual int maxIngGroupNameLength() const;
	virtual int maxRecipeTitleLength() const;
	virtual int maxUnitNameLength() const;
	virtual int maxPrepMethodNameLength() const;
	virtual int maxPropertyNameLength() const;
	virtual int maxYieldTypeLength() const;


	virtual void givePermissions(const QString&, const QString&, const QString&, const QString&){} //no permissions in this backend

protected:
        QString qsqlDriverPlugin() const
        {
                return SQLITE_DRIVER;
        }

	virtual QString escapeAndEncode( const QString &s ) const;
	virtual QString unescapeAndDecode( const QByteArray &s ) const;

	virtual void storePhoto( int recipeID, const QByteArray &data );

private:
	virtual void portOldDatabases( float version );
	virtual QStringList backupCommand() const;
	virtual QStringList restoreCommand() const;

	void addColumn( const QString &new_table_sql, const QString &new_col_info, const QString &default_value, const QString &table_name, int col_index );

	QString escape( const QString &s ) const;
};




#endif
