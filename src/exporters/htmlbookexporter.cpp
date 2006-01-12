/***************************************************************************
*   Copyright (C) 2006 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "htmlbookexporter.h"

#include <qfile.h>

#include <kdebug.h>

#include "backends/recipedb.h"

HTMLBookExporter::HTMLBookExporter( RecipeDB *db, const QString& basedir, const QString &format ) :
		HTMLExporter( db, basedir+"/index", format ), database( db ), m_basedir(basedir)
{
}

HTMLBookExporter::~HTMLBookExporter()
{
	QMap<QString,QTextStream*>::const_iterator it;
	for ( it = fileMap.begin(); it != fileMap.end(); ++it ) {
		(*it)->device()->close();

		//does it matter the order of deletion here?
		QIODevice *file = (*it)->device();
		delete *it;
		delete file;
	}
}

int HTMLBookExporter::headerFlags() const
{
	return RecipeDB::Categories;
}

QString HTMLBookExporter::createContent( const RecipeList& recipes )
{
	RecipeList::const_iterator recipe_it;
	for ( recipe_it = recipes.begin(); recipe_it != recipes.end(); ++recipe_it ) {
		for ( ElementList::const_iterator cat_it = ( *recipe_it ).categoryList.begin(); cat_it != ( *recipe_it ).categoryList.end(); ++cat_it ) {
			QMap<QString,QTextStream*>::iterator stream_it = fileMap.find( (*cat_it).name );
			(**stream_it) << HTMLExporter::createContent(*recipe_it);
		}
	}

	return QString::null;
}

QString HTMLBookExporter::createHeader( const RecipeList &list )
{
	QString output = HTMLExporter::createHeader(list);
	QStringList catLinks;

	RecipeList::const_iterator recipe_it;
	for ( recipe_it = list.begin(); recipe_it != list.end(); ++recipe_it ) {
		for ( ElementList::const_iterator cat_it = ( *recipe_it ).categoryList.begin(); cat_it != ( *recipe_it ).categoryList.end(); ++cat_it ) {
			QMap<QString,QTextStream*>::iterator stream_it = fileMap.find( (*cat_it).name );
			if ( stream_it == fileMap.end() ) {
				QString catPageName = m_basedir+"/"+escape((*cat_it).name)+".html";
				QFile *catPage = new QFile( catPageName );
				catPage->open( IO_WriteOnly );
				QTextStream *stream = new QTextStream( catPage );
				stream->setEncoding( QTextStream::UnicodeUTF8 );
				(*stream) << HTMLExporter::createHeader(list);
				(*stream) << "<h1>"<<(*cat_it).name<<"</h1>";

				catLinks << QString("<a href=\"" + catPageName + "\">" + (*cat_it).name + "</a>");

				fileMap.insert((*cat_it).name,stream);
			}
		}
	}

	return output+"<h1>Krecipes Recipes</h1><div><ul><li>"+catLinks.join("</li><li>")+"</li></ul></div>";
}

QString HTMLBookExporter::createFooter()
{
	QString output = HTMLExporter::createFooter();

	return output;
}
