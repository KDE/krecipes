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

class QStandardItemModel;
class QSortFilterProxyModel;

class Element;
class RecipeDB;

namespace Ui {
	class KreGenericListWidget;
}

class KreGenericListWidget : public QWidget
{
	Q_OBJECT

public:
	KreGenericListWidget( QWidget *parent, RecipeDB *db );
	virtual ~KreGenericListWidget();

protected:
	Ui::KreGenericListWidget *ui;
	QStandardItemModel *m_sourceModel;
	QSortFilterProxyModel *m_proxyModel;
};


#endif //KREGENERICLISTWIDGET_H
