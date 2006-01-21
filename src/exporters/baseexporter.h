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

#include <kapplication.h>
#include <kprogress.h>

#include "datablocks/recipelist.h"

class QFile;

class KTar;

class RecipeDB;

class BaseExporter
{
public:
	BaseExporter( const QString &file, const QString &ext );
	virtual ~BaseExporter();

	/** Subclasses must report which items it is able to work with. */
	virtual int supportedItems() const = 0;

	void exporter( const QValueList<int> &ids, RecipeDB *database, KProgressDialog * = 0 );
	void exporter( int id, RecipeDB *database, KProgressDialog * = 0 );

	/** Returns the actual filename that will be written to during the export.
	  * Note that this can differ somewhat from the filename passed in the 
	  * constructor.
	  */
	QString fileName() const;

	void writeStream( QTextStream &, const RecipeList & );

protected:
	virtual QString createContent( const RecipeList & ) = 0;
	virtual QString createFooter(){ return QString(); }
	virtual QString createHeader( const RecipeList & ){ return QString(); }

	/** The number of recipes to load into memory at once.  This many recipes will be
	  * loaded from the database, processed, and then another batch of this many will be
	  * processed until all recipes are exported.
	  */
	virtual int progressInterval() const { return 50; }

	virtual int headerFlags() const;

	void setCompressed( bool );

	/** Attempt to return the version of the application via
	  * KGlobal::instance()->aboutData()->version()
	  * This can be used by exporters to put the version of the app exporting the file.
	  */
	QString krecipes_version() const;

private:
	bool createFile();
	void saveToFile( const QValueList<int> &ids, RecipeDB *database );

	QFile* file;
	KTar *tar_file;
	QString filename;
	KProgressDialog *m_progress_dlg;
	bool compress;
};

#endif //BASEEXPORTER_H
