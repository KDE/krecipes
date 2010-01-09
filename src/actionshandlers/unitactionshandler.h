/***************************************************************************
*   Copyright © 2004-2006 Jason Kivlighn <jkivlighn@gmail.com>            *
*   Copyright © 2004 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2004 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2009 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef UNITACTIONSHANDLER_H
#define UNITACTIONSHANDLER_H

#include "actionshandlers/actionshandlerbase.h"

class StdUnitListView;
class Q3ListViewItem;
class Unit;
class QPoint;
class QString;

class UnitActionsHandler : public ActionsHandlerBase
{
	Q_OBJECT

public:
	UnitActionsHandler( StdUnitListView *_parentListView, RecipeDB *db );
	~UnitActionsHandler() {}

public slots:
	void createNew();
	void rename();
	void remove();

private:
	bool checkBounds( const Unit &unit );

private slots:
	void renameElement( Q3ListViewItem* i, const QPoint &p, int c );
	void saveElement( Q3ListViewItem* ){}
	void saveUnit( Q3ListViewItem* i, const QString &text, int c );
};

#endif //UNITACTIONSHANDLER_H

