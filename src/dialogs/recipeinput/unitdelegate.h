/****************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>       *
*                                                                           *
*   This program is free software; you can redistribute it and/or modify    *
*   it under the terms of the GNU General Public License as published by    *
*   the Free Software Foundation; either version 2 of the License, or       *
*   (at your option) any later version.                                     *
****************************************************************************/

#ifndef UNITDELEGATE_H
#define UNITDELEGATE_H

#include "backends/recipedb.h"
#include "datablocks/unit.h"

#include <QStyledItemDelegate>
#include <QHash>

class RecipeDB;


class UnitDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	UnitDelegate(QObject *parent = 0);
	void loadAllUnitsList( RecipeDB * database );
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private slots:
	/*void ingredientCreatedSlot( const Element & element );
	void ingredientRemovedSlot( int id );*/

	/*void headerCreatedSlot( const Element & element );
	void headerRemovedSlot( int id );*/

private:
	UnitList m_unitList;
	QHash<QString,RecipeDB::IdType> m_singularNameToIdMap;
	QHash<QString,RecipeDB::IdType> m_pluralNameToIdMap;
};

#endif
