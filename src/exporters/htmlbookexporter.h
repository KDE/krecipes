/***************************************************************************
*   Copyright (C) 2006 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef HTMLBOOKEXPORTER_H
#define HTMLBOOKEXPORTER_H

#include <qmap.h>

#include "baseexporter.h"
#include "htmlexporter.h"

class RecipeDB;

/**
  * Exports a given recipe list as HTML
  * @author Jason Kivlighn
  */
class HTMLBookExporter : public HTMLExporter
{
public:
	HTMLBookExporter( RecipeDB *, const QString&, const QString& );
	virtual ~HTMLBookExporter();

protected:
	virtual QString createContent( const RecipeList & );
	virtual QString createHeader( const RecipeList & );
	virtual QString createFooter();

	virtual int headerFlags() const;

private:
	QMap<QString,QTextStream*> fileMap;

	RecipeDB *database;
	QString m_basedir;
};

#endif //HTMLBOOKEXPORTER_H
