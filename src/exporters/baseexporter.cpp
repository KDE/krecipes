/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn (mizunoami22@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "baseexporter.h"

#include <qfile.h>
#include <qfileinfo.h>

#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>

#include "recipedb.h"

BaseExporter::BaseExporter( RecipeDB *db, const QString& _filename, const QString _format ) :
  file(0),
  format(_format),
  filename(_filename),
  database(db)
{
}


BaseExporter::~BaseExporter()
{
}

void BaseExporter::exporter( const QValueList<int>& l )
{
	if ( createFile() )
	{
		bool fileExists = file->exists();
		int overwrite;
		if(fileExists)
			overwrite = KMessageBox::questionYesNo( 0,QString(i18n("File \"%1\" exists. Would you like to overwrite it?")).arg(file->name()),i18n("Saving recipe") );

		if(!fileExists || overwrite == KMessageBox::Yes)
		{
			QPtrList<Recipe> recipes;
			recipes.setAutoDelete(true);

			QValueList<int>::const_iterator it;
			for ( it = l.begin(); it != l.end(); ++it )
			{
				Recipe *recipe = new Recipe;
				database->loadRecipe( recipe, *it );

				recipes.append( recipe );
			}

			saveToFile( recipes );
		}
	}
	else
		kdDebug()<<"no output file have been selected for export."<<endl;
}

bool BaseExporter::createFile()
{
	if(filename != QString::null)
	{
		for ( QStringList::const_iterator it = possible_formats.begin(); it != possible_formats.end(); ++it )
		{
			if ( filename.right((*it).length()) == *it )
			{
				file = new QFile(filename);
				break;
			}
		}

		if ( !file )
		{
			format = format.remove('*');

			if ( possible_formats.contains( format ) < 1 )
				format = possible_formats[0];

			file = new QFile(filename+format);
		}

		QFileInfo fi(*file);
		format = fi.extension();
		filename = (fi.fileName()).remove(format);

		return true;
	}
	else
		return false;
}
