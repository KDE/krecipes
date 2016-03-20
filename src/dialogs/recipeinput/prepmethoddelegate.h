/****************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>       *
*                                                                           *
*   This program is free software; you can redistribute it and/or modify    *
*   it under the terms of the GNU General Public License as published by    *
*   the Free Software Foundation; either version 2 of the License, or       *
*   (at your option) any later version.                                     *
****************************************************************************/

#ifndef PREPMETHODDELEGATE_H
#define PREPMETHODDELEGATE_H

#include "backends/recipedb.h"
#include "datablocks/elementlist.h"

#include <QStyledItemDelegate>
#include <QHash>

class PrepMethodDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	PrepMethodDelegate(QObject *parent = 0);

	void loadAllPrepMethodsList( RecipeDB * database );

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
	ElementList m_prepMethodsList;

	QHash<QString,RecipeDB::IdType> m_nameToIdMap;
	QHash<RecipeDB::IdType,ElementList::iterator> m_idToIteratorMap;

};

#endif
