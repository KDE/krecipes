/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef KREDBIMPORTER_H
#define KREDBIMPORTER_H

#include <QString>

#include "baseimporter.h"

/** Class to import recipes from any other Krecipes database backend.
  * Note: Though independent of database type, the two databases must have the same structure (i.e. be the same version)
  * @author Jason Kivlighn
  */
class KreDBImporter : public BaseImporter
{
public:
	explicit KreDBImporter( const QString &dbType, const QString &host = QString(), const QString &user = QString(), const QString &pass = QString(), int port = 0 );
	virtual ~KreDBImporter();

private:
	virtual void parseFile( const QString &file_or_table );

	QString dbType;
	QString host;
	QString user;
	QString pass;
	int port;
};

#endif //KREDBIMPORTER_H
