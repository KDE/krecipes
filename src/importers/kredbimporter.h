/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef KREDBIMPORTER_H
#define KREDBIMPORTER_H

#include <qstring.h>

#include "baseimporter.h"

/** Class to import recipes from any other Krecipes database backend.
  * Note: Though independant of database type, the two databases must have the same structure (i.e. be the same version)
  * @author Jason Kivlighn
  */
class KreDBImporter : public BaseImporter
{
public:
	KreDBImporter( const QString &dbType, const QString &host = QString::null, const QString &user = QString::null, const QString &pass = QString::null );
	virtual ~KreDBImporter();

private:
	virtual void parseFile( const QString &file_or_table );

	QString dbType;
	QString host;
	QString user;
	QString pass;
};

#endif //KREDBIMPORTER_H
