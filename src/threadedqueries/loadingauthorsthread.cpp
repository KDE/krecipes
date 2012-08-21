/*****************************************************************************
*   Copyright © 2012 José Manuel Santamaría Lema <panfaust@gmail.com>        *
*                                                                            *
*   This program is free software; you can redistribute it and/or modify     *
*   it under the terms of the GNU General Public License as published by     *
*   the Free Software Foundation; either version 2 of the License, or        *
*   (at your option) any later version.                                      *
******************************************************************************/

#include "loadingauthorsthread.h"

#include <kdebug.h>

#include "backends/recipedb.h"


LoadingAuthorsThread::LoadingAuthorsThread( QObject * parent ):
	QThread( parent )
{
	m_limit = m_offset = -1;
}


void LoadingAuthorsThread::setLimit(int limit)
{
	m_limit = limit;
}


void LoadingAuthorsThread::setOffset(int offset)
{
	m_offset = offset;
}


void LoadingAuthorsThread::run()
{
	kDebug() << "Running query in a separate thread.";

	// Connect to the database reading the settings from krecipesrc.
	m_database = RecipeDB::createDatabase();
	m_database->connect( false, false );

	//Retrieve the list of authors;
	ElementList authorList;
	int numberOfAuthors = m_database->loadAuthors( &authorList , m_limit, m_offset );

	delete m_database;
	emit loadFinished( authorList, numberOfAuthors );
}


