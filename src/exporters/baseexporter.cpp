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

#include <kaboutdata.h>
#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kmessagebox.h>

#include "DBBackend/recipedb.h"

BaseExporter::BaseExporter( const QString& _filename, const QString &_format ) :
  file(0),
  format(_format),
  filename(_filename),
  m_progress_dlg(0)
{
}


BaseExporter::~BaseExporter()
{
}

void BaseExporter::exporter( const RecipeList& recipes, KProgressDialog *progress_dlg )
{
	m_progress_dlg = progress_dlg;
	if (m_progress_dlg){ m_progress_dlg->progressBar()->setTotalSteps(recipes.count());}

	if ( createFile() )
	{
		bool fileExists = file->exists();
		int overwrite;
		if(fileExists)
			overwrite = KMessageBox::questionYesNo( 0,QString(i18n("File \"%1\" exists. Would you like to overwrite it?")).arg(file->name()),i18n("Saving recipe") );

		if(!fileExists || overwrite == KMessageBox::Yes)
			saveToFile( recipes );
	}
	else
		kdDebug()<<"no output file has been selected for export."<<endl;
}

void BaseExporter::exporter( const Recipe &recipe, KProgressDialog *progress_dlg )
{
	RecipeList single_recipe_list;
	single_recipe_list.append( recipe );
	exporter( single_recipe_list, progress_dlg );
}

bool BaseExporter::createFile()
{
	if( !filename.isEmpty() )
	{
		QStringList possible_formats = QStringList::split(',',extensions());
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
		filename = (fi.fileName()).remove("."+format);

		return true;
	}
	else
		return false;
}

void BaseExporter::saveToFile( const RecipeList& recipes )
{
	if ( file->open( IO_WriteOnly ) )
	{
		QTextStream stream( file );
		stream << createContent(recipes);
		file->close();
	}
}

QString BaseExporter::krecipes_version() const
{
	KInstance *this_instance = KGlobal::instance();
	if ( this_instance && this_instance->aboutData() )
		return this_instance->aboutData()->version();

	return QString::null; //Oh, well.  We couldn't get the version.
}

bool BaseExporter::progressBarCanceled()
{
	if(m_progress_dlg)
		return m_progress_dlg->wasCancelled();

	return false;
}

void BaseExporter::setProgressBarTotalSteps( int steps )
{
	if (m_progress_dlg)
		m_progress_dlg->progressBar()->setTotalSteps(steps);
}

void BaseExporter::advanceProgressBar()
{
	if (m_progress_dlg)
	{
		m_progress_dlg->progressBar()->advance(1);
		kapp->processEvents();
	}
}


