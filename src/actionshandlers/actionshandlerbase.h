/***************************************************************************
*   Copyright © 2009 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef ACTIONSHANDLERBASE_H
#define ACTIONSHANDLERBASE_H

#include <QObject>

class DBListViewBase;
class K3ListView;
class KMenu;
class KAction;
class RecipeDB;
class Q3ListViewItem;
class QPoint;

class ActionsHandlerBase : public QObject
{
	Q_OBJECT

public:
	ActionsHandlerBase( DBListViewBase *_parentListView, RecipeDB *db );

public slots:
	virtual void createNew() = 0;
	virtual void rename();
	virtual void remove() = 0;
	void addAction( KAction * action );

protected:
	KMenu *kpop;

	DBListViewBase *parentListView;
	RecipeDB *database;

protected slots:
	virtual void renameElement( Q3ListViewItem* i, const QPoint &p, int c );
	virtual void saveElement( Q3ListViewItem* i ) = 0;
	virtual void showPopup( K3ListView *, Q3ListViewItem *, const QPoint &);
};

#endif //ACTIONSHANDLERBASE_H

