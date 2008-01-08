/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com                                   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef CONVERT_SQLITE3_H
#define CONVERT_SQLITE3_H

#include <qobject.h>

class KProcIO;

class ConvertSQLite3 : public QObject
{
Q_OBJECT

public:
	ConvertSQLite3( const QString &db_file = QString::null );
	~ConvertSQLite3();

public slots:
	void processOutput( KProcIO* p );

private:
	bool copyFile( const QString &oldFilePath, const QString &newFilePath );

	bool error;
};

#endif //CONVERT_SQLITE3_H
