/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2009 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef AUTHORACTIONSHANDLER_H
#define AUTHORACTIONSHANDLER_H

#include "actionshandlers/actionshandlerbase.h"

class DBListViewBase;
class Q3ListViewItem;

class AuthorActionsHandler : public ActionsHandlerBase
{
	Q_OBJECT

public:
	AuthorActionsHandler( DBListViewBase *_parentListView, RecipeDB *db );
	~AuthorActionsHandler() {}

public slots:
	void createNew();
	void remove();

private:
	bool checkBounds( const QString &name );

private slots:
	void saveElement( Q3ListViewItem* i );
};

#endif //AUTHORACTIONSHANDLER_H

