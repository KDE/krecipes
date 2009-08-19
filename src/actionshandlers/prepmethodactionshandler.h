/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2009 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef PREPMETHODACTIONSHANDLER_H
#define PREPMETHODACTIONSHANDLER_H

#include "actionshandlers/actionshandlerbase.h"

class DBListViewBase;
class Q3ListViewItem;

class PrepMethodActionsHandler : public ActionsHandlerBase
{
	Q_OBJECT

public:
	PrepMethodActionsHandler( DBListViewBase *_parentListView, RecipeDB *db );
	~PrepMethodActionsHandler() {}

public slots:
	void createNew();
	void remove();

private:
	bool checkBounds( const QString &name );

private slots:
	void saveElement( Q3ListViewItem* i );
};

#endif //PREPMETHODACTIONSHANDLER_H

