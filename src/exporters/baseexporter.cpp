/***************************************************************************
*   Copyright (C) 2003-2005 by                                            *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
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
#include <ktar.h>
#include <kstandarddirs.h>

#include "backends/recipedb.h"

BaseExporter::BaseExporter( const QString& _filename, const QString &format ) :
		file( 0 ),
		tar_file( 0 ),
		filename( _filename ),
		m_progress_dlg( 0 ),
		compress(false)
{
	//automatically append extension
	QString extension = format.right( format.length()-2 );
	if ( filename.right( extension.length() ) != extension )
		filename += "." + extension;
}

BaseExporter::~BaseExporter()
{
	delete file;
	delete tar_file;
}

int BaseExporter::headerFlags() const
{
	return RecipeDB::None;
}

void BaseExporter::setCompressed( bool b )
{
	compress = b;
}

void BaseExporter::exporter( const QValueList<int> &ids, RecipeDB *database, KProgressDialog *progress_dlg )
{
	m_progress_dlg = progress_dlg;

	if ( createFile() )
		saveToFile( ids, database );
	else
		kdDebug() << "no output file has been selected for export." << endl;
}

void BaseExporter::exporter( int id, RecipeDB *database, KProgressDialog *progress_dlg )
{
	QValueList<int> single_recipe_list;
	single_recipe_list << id ;
	exporter( single_recipe_list, database, progress_dlg );
}

bool BaseExporter::createFile()
{
	if ( file )
		return true;

	if ( !filename.isEmpty() ) {
		if ( compress ) {
			tar_file = new KTar( filename, "application/x-gzip" );
			QFileInfo fi( filename );
			file = new QFile( locateLocal( "tmp",fi.fileName()+"ml" ) );
		}
		else
			file = new QFile(filename);

		return (file != 0);
	}
	else
		return false;
}

QString BaseExporter::fileName() const
{
	QFileInfo fi( filename );
	return fi.fileName();
}

void BaseExporter::saveToFile( const QValueList<int> &ids, RecipeDB *database )
{
	if ( file->open( IO_WriteOnly ) ) {
		if ( m_progress_dlg )
			m_progress_dlg->progressBar()->setTotalSteps( ids.count()/progressInterval() );

		QValueList<int> ids_copy = ids;
		QTextStream stream( file );

		RecipeList recipe_list;
		if ( headerFlags() != RecipeDB::None ) {
			database->loadRecipes( &recipe_list, headerFlags(), ids );
		}
		stream << createHeader( recipe_list );

		recipe_list.clear();
		for ( int i = 0; i < ids.count(); i += progressInterval() ) {
			QValueList<int> sub_list;
			for ( int sub_i = 0; sub_i < progressInterval(); ++sub_i ) {
				if ( ids_copy.count() == 0 ) break;

				sub_list << *ids_copy.begin();
				ids_copy.remove( ids_copy.begin() );
			}

			RecipeList recipe_list;
			database->loadRecipes( &recipe_list, RecipeDB::All, sub_list );

			QString content = createContent( recipe_list );
			if ( !content.isEmpty() )
				stream << content;

			if ( m_progress_dlg && m_progress_dlg->wasCancelled() )
				break;

			if ( m_progress_dlg ) {
				m_progress_dlg->progressBar()->advance( progressInterval() );
				kapp->processEvents();
			}
		}

		stream << createFooter();

		file->close();

		if ( tar_file && tar_file->open( IO_WriteOnly ) ) {
			QFileInfo fi( file->name() );
			tar_file->addLocalFile( file->name(), fi.fileName() );
			tar_file->close();
		}
	}
}

QString BaseExporter::krecipes_version() const
{
	KInstance * this_instance = KGlobal::instance();
	if ( this_instance && this_instance->aboutData() )
		return this_instance->aboutData() ->version();

	return QString::null; //Oh, well.  We couldn't get the version.
}

