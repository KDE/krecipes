/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef BASEEXPORTER_H
#define BASEEXPORTER_H

#include <qvaluelist.h>
#include <qstringlist.h>
#include <qptrlist.h>

#include "DBBackend/recipedb.h"
#include "datablocks/recipelist.h"

class QFile;
class RecipeDB;

class BaseExporter
{
public:
	BaseExporter( RecipeDB *, const QString &file, const QString format );
	virtual ~BaseExporter();

	void exporter( const QValueList<int>& ids);
	void exporter( int id );

	virtual QString createContent( const QPtrList<Recipe> & ) = 0;

protected:
	/** Default implementation writes the return value of createContent() to a file. */
	virtual void saveToFile( const QPtrList<Recipe> & );

	/** Returns the extension (comma-separated if a single exporter can save to more than one format)
	  * of the file format.
	  */
	virtual QString extensions() const = 0;

	/** Attempt to return the version of the application via
	  * KMainWindow::memberList[0]->instance()->aboutData()->version()
	  * This can be used by exporters to put the version of the app exporting the file.
	  */
	QString krecipes_version() const;

	QFile* file;
	QString format;
	QString filename;

private:
	bool createFile();

	RecipeDB *database;
};

#endif //BASEEXPORTER_H
