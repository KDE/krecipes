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

class BaseExporter
{
public:
	BaseExporter( const QString &file, const QString &format );
	virtual ~BaseExporter();

	void exporter( const RecipeList& recipes, KProgressDialog * = 0 );
	void exporter( const Recipe &, KProgressDialog * = 0 );
	
	/** Returns the actual filename that will be written to during the export.
	  * Note that this can differ somewhat from the filename passed in the 
	  * constructor.
	  */
	QString fileName();

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

	/** If @ref exporter() was passed a KProgress object, then this function
	  * will advance that KProgress object. Subclasses could advance the
	  * progress after processing each recipe to let the user watch the
	  * progress of the export.
	  */
	void advanceProgressBar();

	/** If @ref exporter() was passed a KProgress object, then this function
	  * will set the total number of steps in this KProgress object.  By
	  * default, the total number of steps is the total number of recipes
	  * being imported.  That way, most subclasses can just call advanceProgressBar()
	  * after each recipe is processed, and won't need to mess with setting this value.
	  */
	void setProgressBarTotalSteps( int steps );

	/** If @ref exporter() was passed a KProgress object, then this function
	  * will return whether or not the progress bar has been canceled by the user.
	  * Subclasses should check the return value of the function and cancel exporting,
	  * as is appropriate.
	  */
	bool progressBarCancelled();

	QFile* file;
	QString format;
	QString filename;

private:
	bool createFile();

	KProgressDialog *m_progress_dlg;
};

#endif //BASEEXPORTER_H
