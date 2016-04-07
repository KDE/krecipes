/****************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>       *
*                                                                           *
*   This program is free software; you can redistribute it and/or modify    *
*   it under the terms of the GNU General Public License as published by    *
*   the Free Software Foundation; either version 2 of the License, or       *
*   (at your option) any later version.                                     *
****************************************************************************/

#ifndef INGREDIENTNAMEDELEGATE_H
#define INGREDIENTNAMEDELEGATE_H

#include "backends/recipedb.h"
#include "datablocks/elementlist.h"

#include <QStyledItemDelegate>
#include <QHash>

class RecipeDB;


class IngredientNameDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	IngredientNameDelegate( RecipeDB * database,
		QObject *parent = 0 );
	void loadAllIngredientsList( RecipeDB * database );
	void loadAllHeadersList( RecipeDB * database );
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private slots:
	void ingredientCreatedSlot( const Element & element );
	void ingredientModifiedSlot( const Ingredient & ingredient );
	void ingredientRemovedSlot( int id );

	void headerCreatedSlot( const Element & element );
	void headerModifiedSlot( const Element & element );
	void headerRemovedSlot( int id );

private:
	RecipeDB * m_database;

	QHash<RecipeDB::IdType,Element> m_idToIngredientMap;
	QMultiHash<QString,RecipeDB::IdType> m_ingredientNameToIdMap;

	QHash<RecipeDB::IdType,Element> m_idToHeaderMap;
	QMultiHash<QString,RecipeDB::IdType> m_headerNameToIdMap;
};

#endif
