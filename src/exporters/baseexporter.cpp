/***************************************************************************
*   Copyright © 2003-2005 Cyril Bosselut <bosselut@b1project.com>         *
*   Copyright © 2003-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "baseexporter.h"

#include <QFile>
#include <QFileInfo>

#include <kaboutdata.h>
#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kmessagebox.h>
#include <ktar.h>
#include <kstandarddirs.h>
#include <KComponentData>

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
		filename += '.' + extension;
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

void BaseExporter::exporter( const QList<int> &ids, RecipeDB *database, KProgressDialog *progress_dlg )
{
	m_progress_dlg = progress_dlg;

	if ( createFile() )
		saveToFile( ids, database );
	else
		kDebug() << "no output file has been selected for export." ;
}

void BaseExporter::exporter( int id, RecipeDB *database, KProgressDialog *progress_dlg )
{
	QList<int> single_recipe_list;
	single_recipe_list << id ;
	exporter( single_recipe_list, database, progress_dlg );
}

void BaseExporter::writeStream( QTextStream &stream, const RecipeList &recipe_list )
{
	stream << createHeader(recipe_list);
	stream << createContent(recipe_list);
	stream << createFooter();
}

bool BaseExporter::createFile()
{
	if ( file )
		return true;

	if ( !filename.isEmpty() ) {
		if ( compress ) {
			tar_file = new KTar( filename, "application/x-gzip" );
			QFileInfo fi( filename );
			file = new QFile( KStandardDirs::locateLocal( "tmp",fi.fileName()+"ml" ) );
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
	return filename;
}

void BaseExporter::saveToFile( const QList<int> &ids, RecipeDB *database )
{
	if ( file->open( QIODevice::WriteOnly ) ) {
		if ( m_progress_dlg )
			m_progress_dlg->progressBar()->setMaximum( ids.count()/progressInterval() );

		QList<int> ids_copy = ids;
		QTextStream stream( file );
		stream.setCodec( "UTF-8" );

		RecipeList recipe_list;
		if ( headerFlags() != RecipeDB::None ) {
			database->loadRecipes( &recipe_list, headerFlags(), ids );
		}
		stream << createHeader( recipe_list );

		recipe_list.clear();
		for ( int i = 0; i < ids.count(); i += progressInterval() ) {
			QList<int> sub_list;
			for ( int sub_i = 0; sub_i < progressInterval(); ++sub_i ) {
				if ( ids_copy.count() == 0 ) break;

				sub_list << *ids_copy.begin();
				ids_copy.erase( ids_copy.begin() );
			}

			RecipeList recipe_list;
			database->loadRecipes( &recipe_list, supportedItems(), sub_list );

			QString content = createContent( recipe_list );
			if ( !content.isEmpty() )
				stream << content;

			if ( m_progress_dlg && m_progress_dlg->wasCancelled() )
				break;

			if ( m_progress_dlg ) {
				m_progress_dlg->progressBar()->setValue(m_progress_dlg->progressBar()->value() + progressInterval() );
				kapp->processEvents();
			}
		}

		stream << createFooter();

		if ( tar_file && tar_file->open( QIODevice::WriteOnly ) ) {
			//close, which flushes the buffer, and then open for reading
			file->close();
			file->open( QIODevice::ReadOnly );

			QFileInfo fi( file->fileName() );
			QByteArray data = file->readAll();
			tar_file->writeFile( fi.fileName(), fi.owner(), fi.group(), data, data.size() );
			tar_file->close();
		}

		file->close();
	}
}

QString BaseExporter::krecipes_version() const
{
	const KComponentData * this_instance = &KGlobal::mainComponent();
	if ( this_instance && this_instance->aboutData() )
		return this_instance->aboutData() ->version();

	return QString(); //Oh, well.  We couldn't get the version.
}

