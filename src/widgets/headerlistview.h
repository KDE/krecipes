/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2004 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2004 Cyril Bosselut <bosselut@b1project.com>              *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef HEADERLISTVIEW_H
#define HEADERLISTVIEW_H

#include "dblistviewbase.h"

#include "datablocks/element.h"

class RecipeDB;
class KMenu;

class HeaderListView : public DBListViewBase
{
	Q_OBJECT

public:
	HeaderListView( QWidget *parent, RecipeDB *db );

public slots:
	virtual void load( int curr_limit, int curr_offset );

protected slots:
	virtual void createHeader( const Element & ) = 0;
	virtual void removeHeader( int ) = 0;

	void checkCreateHeader( const Element &el );

};

class StdHeaderListView : public HeaderListView
{
	Q_OBJECT

public:
	StdHeaderListView( QWidget *parent, RecipeDB *db, bool editable = false );

protected:
	virtual void createHeader( const Element & );
	virtual void removeHeader( int );
};

#endif //HEADERLISTVIEW_H
