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

#include <qstringlist.h>

#include "datablocks/recipelist.h"

class QFile;

class BaseExporter
{
public:
	BaseExporter( const QString &file, const QString &format );
	virtual ~BaseExporter();

	void exporter( const RecipeList& recipes );
	void exporter( const Recipe & );

	virtual QString createContent( const RecipeList & ) = 0;

protected:
	/** Default implementation writes the return value of createContent() to a file. */
	virtual void saveToFile( const RecipeList & );

	/** Returns the extension (comma-separated if a single exporter can save to more than one format)
	  * of the file format.
	  */
	virtual QString extensions() const = 0;

	/** Attempt to return the version of the application via
	  * KGlobal::instance()->aboutData()->version()
	  * This can be used by exporters to put the version of the app exporting the file.
	  */
	QString krecipes_version() const;

	QFile* file;
	QString format;
	QString filename;

private:
	bool createFile();
};

#endif //BASEEXPORTER_H
