/***************************************************************************
*   Copyright © 2012-2015 José Manuel Santamaría Lema <panfaust@gmail.com> *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#ifndef KRERECIPESLISTWIDGET_H
#define KRERECIPESLISTWIDGET_H


#include "krecategorieslistwidget.h"
#include <QPersistentModelIndex>

class Element;
class ElementList;
class QStandardItem;
class QItemSelection;


class KreRecipesListWidget : public KreCategoriesListWidget
{
	Q_OBJECT

public:
	enum ItemType{ CategoryItem=0, RecipeItem=1, OtherItem=2 };
	enum RecipeStatus{ Categorized=1, Uncategorized=0 };

	KreRecipesListWidget( QWidget *parent, RecipeDB *db );

	QList<int> selectedCategories();
	QList<int> selectedRecipes();
	QList<int> getAllVisibleRecipes();

	/* Returns a list of QPairs<int,int>, the first one is the id of the
	   recipe and the second one is the id of the category of all selected
	   recipes */ 
	QList< QPair<int,int> > selectedRecipesWithCategories();

	QString getRecipeName( int id );

signals:
	void elementSelected(const QList<int> & recipes, const QList<int> & categories);
	void selectionChanged();
	void doubleClicked( const QModelIndex & index );
	

protected slots:
	//Operations with categories.
	virtual void createCategorySlot( const Element & category, int parent_id );
	virtual void removeCategorySlot( int id );

	//Operations with recipes.
	virtual void createRecipeSlot( const Element & recipe, const ElementList & categories );
	virtual void modifyRecipeSlot( const Element & recipe, const ElementList & categories );
	virtual void removeRecipeFromCategorySlot( int recipe_id, int category_id );
	virtual void removeRecipeSlot( int id );

	//Selection changed.
	void selectionChangedSlot(const QItemSelection & current, const QItemSelection & previous);

protected:
	//virtual int elementCount();
	virtual void load(int limit, int offset);
	//virtual void cancelLoad(){}

	//virtual int idColumn();

	void populate ( QStandardItem * item, int id );

	void addRecipeItem( const Element & recipe, 
		QModelIndex categoryIndex, RecipeStatus status = Categorized );

	QList<int> m_selectedRecipes;
	QList<int> m_selectedCategories;

	QList< QPair<int,int> > m_recipesWithCategories;

	QMultiHash<int,QPersistentModelIndex> m_recipeIdToIndexMap;

	QPersistentModelIndex m_uncategorizedItemIndex;
};


#endif //KRERECIPESLISTWIDGET_H
