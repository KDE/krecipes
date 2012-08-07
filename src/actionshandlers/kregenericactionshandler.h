/*****************************************************************************
*   Copyright © 2009-2012 José Manuel Santamaría Lema <panfaust@gmail.com>   *
*                                                                            *
*   This program is free software; you can redistribute it and/or modify     *
*   it under the terms of the GNU General Public License as published by     *
*   the Free Software Foundation; either version 2 of the License, or        *
*   (at your option) any later version.                                      *
******************************************************************************/

#ifndef KREGENERICACTIONSHANDLER_H
#define KREGENERICACTIONSHANDLER_H

#include <QObject>

class KreGenericListWidget;
class RecipeDB;
class KMenu;
class KAction;
class QModelIndex;
class QPoint;


class KreGenericActionsHandler : public QObject
{
	Q_OBJECT

public:
	KreGenericActionsHandler( KreGenericListWidget * listWidget, RecipeDB * db );

public slots:
	virtual void createNew() = 0;
	virtual void rename();
	virtual void remove() = 0;
	void addAction( KAction * action );

protected:
	KMenu * m_contextMenu;

	KreGenericListWidget * m_listWidget;
	RecipeDB * m_database;

protected slots:
	virtual void saveElement( const QModelIndex & topLeft, 
		const QModelIndex & bottomRight) = 0;
	virtual void showPopup( const QModelIndex & index,
		const QPoint & point);
};

#endif //KREGENERICACTIONSHANDLER_H

