/*****************************************************************************
*   Copyright © 2012 José Manuel Santamaría Lema <panfaust@gmail.com>        *
*                                                                            *
*   This program is free software; you can redistribute it and/or modify     *
*   it under the terms of the GNU General Public License as published by     *
*   the Free Software Foundation; either version 2 of the License, or        *
*   (at your option) any later version.                                      *
******************************************************************************/

#ifndef LOADINGAUTHORSTHREAD_H
#define LOADINGAUTHORSTHREAD_H

#include <QThread>

#include "datablocks/elementlist.h"


class RecipeDB;


class LoadingAuthorsThread: public QThread
{
	Q_OBJECT

public:
	LoadingAuthorsThread( QObject * parent = 0 );
	void setLimit(int limit);
	void setOffset(int offset);

signals:
	void loadFinished( const ElementList & authorList, int authorsLoaded );

protected:
	void run();
	//Uncomment this line to make it a fake thread.
	//void start(){ run(); };

private:
	RecipeDB * m_database;
	int m_limit, m_offset;
};


#endif //LOADINGAUTHORSTHREAD_H
