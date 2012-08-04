/***************************************************************************
*   Copyright © 2012 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef KREGENERICLISTWIDGET_H
#define KREGENERICLISTWIDGET_H

#include <QWidget>

//FIXME: Remove this header and uncomment ReloadFlags below
// whenever we can get rid of the obsolete DBListviewBase.
#include "dblistviewbase.h"

class QStandardItemModel;
class QSortFilterProxyModel;

class Element;
class RecipeDB;

namespace Ui {
	class KreGenericListWidget;
}

/* enum ReloadFlags {
	Load,			// Only performs the reload if the list hasn't already been loaded.
	ReloadIfPopulated,	// Only performs the reload if the list has been loaded.
	ForceReload		// Load/reload the list regardless of whether or not it's been loaded.
};*/

class KreGenericListWidget : public QWidget
{
	Q_OBJECT

public:
	KreGenericListWidget( QWidget *parent, RecipeDB *db );
	virtual ~KreGenericListWidget();

	void setCurrentLimit( int value );
	void reload( ReloadFlags flags );

protected slots:
	void activatePreviousPage();
	void activateNextPage();

protected:
	virtual void load(int limit, int offset) = 0;

	int m_currentLimit;
	int m_currentOffset;

	Ui::KreGenericListWidget *ui;
	QStandardItemModel *m_sourceModel;
	QSortFilterProxyModel *m_proxyModel;
	RecipeDB *m_database;
};


#endif //KREGENERICLISTWIDGET_H
