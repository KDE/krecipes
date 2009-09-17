/***************************************************************************
*   Copyright © 2003-2005 Cyril Bosselut <bosselut@b1project.com>         *
*   Copyright © 2003-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef BASEEXPORTER_H
#define BASEEXPORTER_H

#include <QTextStream>
#include <QList>

#include <kapplication.h>
#include <kprogressdialog.h>

#include "datablocks/recipelist.h"

class QFile;

class KTar;

class RecipeDB;

class BaseExporter
{
public:
	BaseExporter( const QString &file, const QString &ext );
	virtual ~BaseExporter();

	/** Subclasses must report which items it is able to work with.
	 *  These should be or'ed together items from RecipeDB::RecipeItems
	 */
	virtual int supportedItems() const = 0;

	/** Export the recipes with the given ids to the file specified in the constructor.
	 *  Optionally, a progress dialog may be given to specify the progress made.
	 */
	void exporter( const QList<int> &ids, RecipeDB *database, KProgressDialog * = 0 );

	/** Convenience function for the above, which exports a single recipe. */
	void exporter( int id, RecipeDB *database, KProgressDialog * = 0 );

	/** Returns the actual filename that will be written to during the export.
	  * Note that this can differ somewhat from the filename passed in the 
	  * constructor.
	  */
	QString fileName() const;

	/** Write the given recipe list to a text stream.
	 *  This can be used to export recipes without use of the database.
	 */
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

	/** Extra RecipeDB::RecipeItems that a subclass requires when creating a file's header.
	 *  For example, the Krecipes file format requires writing the category hierarchy in the header,
	 *  so it's exporter adds RecipeDB::Categories.
	 */
	virtual int headerFlags() const;

	/** Make generated file a gzipped tarball */
	void setCompressed( bool );

	/** Attempt to return the version of the application via
	  * KGlobal::instance()->aboutData()->version()
	  * This can be used by exporters to put the version of the app exporting the file.
	  */
	QString krecipes_version() const;

private:
	bool createFile();
	void saveToFile( const QList<int> &ids, RecipeDB *database );

	QFile* file;
	KTar *tar_file;
	QString filename;
	KProgressDialog *m_progress_dlg;
	bool compress;
};

#endif //BASEEXPORTER_H
