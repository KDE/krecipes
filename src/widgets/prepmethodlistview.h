/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef PREPMETHODLISTVIEW_H
#define PREPMETHODLISTVIEW_H

#include "dblistviewbase.h"

#include "datablocks/element.h"

class RecipeDB;
class KMenu;

class PrepMethodListView : public DBListViewBase
{
	Q_OBJECT

public:
	PrepMethodListView( QWidget *parent, RecipeDB *db );

public slots:
	virtual void load( int curr_limit, int curr_offset );

protected slots:
	virtual void createPrepMethod( const Element & ) = 0;
	virtual void removePrepMethod( int ) = 0;

	void checkCreatePrepMethod( const Element &el );
};


class StdPrepMethodListView : public PrepMethodListView
{
	Q_OBJECT

public:
	StdPrepMethodListView( QWidget *parent, RecipeDB *db, bool editable = false );

protected:
	virtual void createPrepMethod( const Element & );
	virtual void removePrepMethod( int );
};

#endif //PREPMETHODLISTVIEW_H
