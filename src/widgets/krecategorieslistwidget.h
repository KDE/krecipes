/***************************************************************************
*   Copyright © 2012-2016 José Manuel Santamaría Lema <panfaust@gmail.com> *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#ifndef KRECATEGORIESLISTWIDGET_H
#define KRECATEGORIESLISTWIDGET_H


#include <QWidget>

#include "kregenericlistwidget.h"

#include "backends/recipedb.h"

class Element;
class QStandardItem;
class QModelIndex;
class QPersistentModelIndex;


class KreCategoriesListWidget : public KreGenericListWidget
{
	Q_OBJECT

public:
	enum UserRoles{
		ItemTypeRole=Qt::UserRole,
		IdRole=Qt::UserRole+1,
		CategorizedRole=Qt::UserRole+2 };

	KreCategoriesListWidget( QWidget *parent, RecipeDB *db,
		bool itemsCheckable = false );
	~KreCategoriesListWidget();

	void checkCategories( const ElementList & items_on );

	ElementList checkedCategories();

	void edit( int row, const QModelIndex & parent );

	void expandAll();
	void collapseAll();

protected slots:
	void setFilter( const QString & filter );
	virtual void createCategorySlot( const Element & category, int parent_id );
	virtual void modifyCategory( const Element & category );
	virtual void modifyCategory( int category_id, int new_parent_id );
	virtual void removeCategory( int id );

	void itemsChangedSlot( const QModelIndex & topLeft,
		const QModelIndex & bottomRight );

protected:
	virtual int elementCount();
	virtual void load(int limit, int offset);
	virtual void cancelLoad(){}

	virtual int idColumn();

	void populate ( QStandardItem * item, int id );

	bool m_itemsCheckable;
	ElementList * m_checkedCategories;

	QHash<RecipeDB::IdType,QPersistentModelIndex> m_categoryIdToIndexMap;
};


#endif //KRECATEGORIESLISTWIDGET_H
