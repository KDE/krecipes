/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef KRECIPES_H
#define KRECIPES_H

#include <kio/slavebase.h>
#include <qstring.h>

class KURL;
class QCString;

class RecipeDB;

class kio_krecipesProtocol : public KIO::SlaveBase
{
public:
	kio_krecipesProtocol(const QCString &pool_socket,
				const QCString &app_socket);
	virtual ~kio_krecipesProtocol();
	
	virtual void listDir( const KURL & url );
	virtual void stat( const KURL & url );
	virtual void get( const KURL & url );

private:
	RecipeDB *m_db;
};


#endif /* KRECIPES_H */
