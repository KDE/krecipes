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
class QString;
class QModelIndex;
class QPoint;
class QModelIndex;

class KPixmapSequenceWidget;

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

	void setSearchAsYouType( bool value );
	void setCurrentLimit( int value );
	void reload( ReloadFlags flags );
	int currentRow();
	QModelIndex currentParent() const;
	void edit( int row );
	int selectedRowId();

signals:
	void contextMenuRequested( const QModelIndex & index, const QPoint & point);
	void itemsChanged( const QModelIndex & topLeft, const QModelIndex & bottomRight);
	void loadFinishedPrivate();

protected slots:
	void setFilter();
	void setFilter( const QString & filter );
	void activatePreviousPage();
	void activateNextPage();
	void contextMenuSlot( const QPoint & point );
	void startAnimation();
	void loadFinishedPrivateSlot();

protected:
	static const int busyAnimationThreshold = 500;

	virtual int elementCount() = 0;
	virtual void load(int limit, int offset) = 0;
	virtual void cancelLoad() = 0;

 	//This function must return the column number where the id is stored.
	virtual int idColumn() = 0;

	bool m_searchAsYouType;

	int m_currentLimit;
	int m_currentOffset;

	Ui::KreGenericListWidget *ui;
	KPixmapSequenceWidget * m_anim;
	bool m_loadFinished;
	QStandardItemModel *m_sourceModel;
	QSortFilterProxyModel *m_proxyModel;
	RecipeDB *m_database;
};


#endif //KREGENERICLISTWIDGET_H
