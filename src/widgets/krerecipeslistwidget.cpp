/***************************************************************************
*   Copyright © 2015 José Manuel Santamaría Lema <panfaust@gmail.com>      *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#include "krerecipeslistwidget.h"
#include "ui_kregenericlistwidget.h"

#include "backends/recipedb.h"
#include "datablocks/categorytree.h"
#include "datablocks/elementlist.h"

#include <kdebug.h>

#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QItemSelection>
#include <QPersistentModelIndex>



KreRecipesListWidget::KreRecipesListWidget( QWidget *parent, RecipeDB *db ):
	KreCategoriesListWidget( parent, db )
{
	ui->m_treeView->setSelectionMode( QAbstractItemView::ExtendedSelection );
        
	//The horizontal column labels.
        QStringList horizontalLabels;
        horizontalLabels << i18nc( "@title:column", "Recipes" ) << i18nc( "@title:column", "Id" );
        m_sourceModel->setHorizontalHeaderLabels( horizontalLabels );

	connect( ui->m_treeView, SIGNAL(doubleClicked(QModelIndex)),
		this, SIGNAL(doubleClicked(QModelIndex)) );

	//Connect signals and slots from the database
	connect( m_database, SIGNAL(categoryCreated(Element,int)),
		this, SLOT(createCategorySlot(Element,int)) );
	//TODO: connect the following signals from database
	// right now they are not needed but will be in the future.
	//void categoryRemoved( int id );
	//void categoryModified( const Element & );
	//void categoryModified( int id, int parent_id );
	//void categoriesMerged( int id1, int id2 );
	//void recipeCreated( const Element &, const ElementList &categories );
	connect( m_database, SIGNAL(recipeRemoved(int)),
		this, SLOT(removeRecipeSlot(int)) );
	connect( m_database, SIGNAL(recipeRemoved(int,int)),
		this, SLOT(removeRecipeFromCategorySlot(int,int)) );
	connect( m_database, SIGNAL(recipeModified(Element,ElementList)),
		this, SLOT(modifyRecipeSlot(Element,ElementList)) );

	//Needed to update the lists of ids of the selected elements and to trigger
	//our elementSelected signal.
	connect( ui->m_treeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), 
		this, SLOT(selectionChangedSlot(QItemSelection,QItemSelection)) );
}

void KreRecipesListWidget::createCategorySlot( const Element & category, int parent_id )
{
	//FIXME: This could be more efficient.
	Q_UNUSED(category)
	Q_UNUSED(parent_id)
	reload( ForceReload );
}

void KreRecipesListWidget::removeCategorySlot( int id )
{
	//FIXME: This could be more efficient.
	Q_UNUSED(id)
	reload( ForceReload );
}

void KreRecipesListWidget::addRecipeItem(const Element & recipe, 
	QModelIndex categoryIndex, RecipeStatus status )
{
	//The "Id" item.
	QStandardItem *itemRecipeId = new QStandardItem;
	itemRecipeId->setData( QVariant( recipe.id ), Qt::EditRole );
	itemRecipeId->setData( RecipeItem, ItemTypeRole );
	itemRecipeId->setData( recipe.id, IdRole );
	itemRecipeId->setData( status, CategorizedRole );
	itemRecipeId->setEditable( false );

	//The "Recipe" item.
	QStandardItem *itemRecipe = new QStandardItem( recipe.name );
	itemRecipe->setData( KIcon("documentation"), Qt::DecorationRole );
	itemRecipe->setData( RecipeItem, ItemTypeRole );
	itemRecipe->setData( recipe.id, IdRole );
	itemRecipe->setData( status, CategorizedRole );
	itemRecipe->setEditable( false );

	//Add the items to the model.
	QStandardItem *categoryItem = m_sourceModel->itemFromIndex( categoryIndex );
	QList<QStandardItem *> items;
	items << itemRecipe << itemRecipeId;
	categoryItem->appendRow( items );
		
	//Update the persistent index map.
	m_recipeIdToIndexMap.insert( recipe.id, itemRecipe->index() );
}

void KreRecipesListWidget::createRecipeSlot( const Element & recipe, const ElementList & categories )
{
	if ( categories.isEmpty() ) {
		//Add the recipe to the "Uncategorized" item if needed.
		if ( m_uncategorizedItemIndex.isValid() ) {
			addRecipeItem( recipe, m_uncategorizedItemIndex, Uncategorized );
		} else {
			kDebug() << "WARNING: Model index invalid";
		}
	} else {
		//Add the recipe to the corresponding category folders.
		ElementList::const_iterator el_it;
		for ( el_it = categories.constBegin(); el_it != categories.constEnd(); el_it++ ) {
			QModelIndex categoryIndex = m_categoryIdToIndexMap[el_it->id];
			if ( categoryIndex.isValid() ) {
				addRecipeItem( recipe, categoryIndex, Categorized );
			} else {
				kDebug() << "WARNING: Model index invalid";
			}
		}
	}

}
		
void KreRecipesListWidget::modifyRecipeSlot( const Element & recipe, const ElementList & categories)
{
	removeRecipeSlot( recipe.id );
	createRecipeSlot( recipe, categories );
}

void KreRecipesListWidget::removeRecipeFromCategorySlot( int recipe_id, int category_id )
{
	//Save the name of the recipe in case we need it later.
	QModelIndex firstRecipeIndex = m_recipeIdToIndexMap.values(recipe_id).first();
	QString recipeName = m_sourceModel->data( firstRecipeIndex, Qt::EditRole ).toString(); 

	//Remove the recipe from the model.
	QList<QPersistentModelIndex> indexList = m_recipeIdToIndexMap.values( recipe_id );
	for ( QList<QPersistentModelIndex>::const_iterator it = indexList.constBegin();
	it != indexList.constEnd(); it++) {
		QModelIndex index = *it;
		if ( it->isValid() ) {
			int id_found = m_sourceModel->data( index.parent(), IdRole ).toInt();
			if ( id_found == category_id ) {
				//Update the index map.
				m_recipeIdToIndexMap.remove( recipe_id, index );
				//Remove row.
				m_sourceModel->removeRow( it->row(), it->parent() );
			}
		} else {
			kDebug() << "WARNING: Model index invalid";
		}
	}


	//If the recipe becomes uncategorized, it must be added as a child 
	//of the "uncategorized" item.
	if ( m_recipeIdToIndexMap.values( recipe_id ).isEmpty() ) {
		Element recipe ( recipeName, recipe_id );
		addRecipeItem( recipe, m_uncategorizedItemIndex, Uncategorized );
	}
}

void KreRecipesListWidget::removeRecipeSlot( int id )
{
	QList<QPersistentModelIndex> indexList = m_recipeIdToIndexMap.values( id );
	for ( QList<QPersistentModelIndex>::const_iterator it = indexList.constBegin();
	it != indexList.constEnd(); it++) {
		QModelIndex index = *it;
		if ( it->isValid() ) {
			m_sourceModel->removeRow( it->row(), it->parent() );
		} else {
			kDebug() << "WARNING: Model index invalid";
		}
	}
	m_recipeIdToIndexMap.remove( id );
}

void KreRecipesListWidget::selectionChangedSlot(const QItemSelection & selected,
	const QItemSelection & deselected)
{
	Q_UNUSED(selected)
	Q_UNUSED(deselected)

	QSet<int> recipes, categories;

	m_recipesWithCategories.clear();

	QItemSelection selection = ui->m_treeView->selectionModel()->selection();

	QModelIndexList indexes = selection.indexes();
	for ( QModelIndexList::const_iterator it = indexes.constBegin();
	it != indexes.constEnd(); ++it ) {
		int type = m_proxyModel->data( *it, Qt::UserRole ).toInt();
		if ( type == RecipeItem ) {
			// Get the recipe id.
			int recipe_id = m_proxyModel->data( 
				*it, KreRecipesListWidget::IdRole ).toInt();
			//Get the category id.
			QModelIndex parentIndex = it->parent();
			int category_id = m_proxyModel->data( 
				parentIndex, KreRecipesListWidget::IdRole ).toInt();
			//Insert the pair of id's in the list.
			QPair<int,int> id_pair( recipe_id, category_id );
			m_recipesWithCategories << id_pair;
			//Insert the recipe id in the list.
			recipes << recipe_id;
		} else {
			categories << m_proxyModel->data( *it, Qt::UserRole+1 ).toInt();
		}
	}
	
	m_selectedRecipes = recipes.toList();
	m_selectedCategories = categories.toList();
	emit elementSelected( m_selectedRecipes, m_selectedCategories );
	emit selectionChanged();
}

QList<int> KreRecipesListWidget::selectedCategories()
{
	return m_selectedCategories;
}

QList<int> KreRecipesListWidget::selectedRecipes()
{
	return m_selectedRecipes;
}

QList<int> KreRecipesListWidget::getAllVisibleRecipes()
{
	return m_recipeIdToIndexMap.keys();
}

QList< QPair<int,int> > KreRecipesListWidget::selectedRecipesWithCategories()
{
	return m_recipesWithCategories;
}

QString KreRecipesListWidget::getRecipeName( int id ) 
{
	QPersistentModelIndex index = m_recipeIdToIndexMap.value( id );
	QString name = m_sourceModel->data( index, Qt::EditRole ).toString();
	return name;
}

void KreRecipesListWidget::populate( QStandardItem * item, int id )
{
	int current_row = 0;

	CategoryTree categoryTree;
	m_database->loadCategories( &categoryTree, -1, 0, id, false );

	for ( CategoryTree * child_it = categoryTree.firstChild(); 
	child_it; child_it = child_it->nextSibling() ) {
		//The "Id" item.
		QStandardItem *itemId = new QStandardItem;
		itemId->setData( QVariant(child_it->category.id), Qt::EditRole );
		itemId->setData( CategoryItem, Qt::UserRole );
		itemId->setData( child_it->category.id, Qt::UserRole+1 );
		itemId->setEditable( false );

		//The "Category" item.
		QStandardItem *itemCategory = new QStandardItem( child_it->category.name );
		itemCategory->setData( KIcon("folder-yellow"), Qt::DecorationRole );
		itemCategory->setData( CategoryItem, Qt::UserRole );
		itemCategory->setData( child_it->category.id, Qt::UserRole+1 );
		itemCategory->setEditable( false );
	
		//Insert the category items as children
		item->setChild( current_row, 0, itemCategory );
		item->setChild( current_row, 1, itemId );
		populate( itemCategory, child_it->category.id );

		//Update the persistent index map.
		m_categoryIdToIndexMap[child_it->category.id] = itemCategory->index();

		++current_row;
	}

	//Insert the recipes as children
	ElementList recipeList;
	m_database->loadRecipeList( &recipeList, id, false );

	for ( ElementList::const_iterator it = recipeList.constBegin();
	it != recipeList.constEnd(); ++it ) {

		//The "Id" item.
		QStandardItem *itemRecipeId = new QStandardItem;
		itemRecipeId->setData( QVariant( it->id ), Qt::EditRole );
		itemRecipeId->setData( RecipeItem, Qt::UserRole );
		itemRecipeId->setData( it->id, Qt::UserRole+1 );
		itemRecipeId->setData( Categorized, CategorizedRole ); //recipe categorized
		itemRecipeId->setEditable( false );

		//The "Recipe" item.
		QStandardItem *itemRecipe = new QStandardItem( it->name );
		itemRecipe->setData( KIcon("documentation"), Qt::DecorationRole );
		itemRecipe->setData( RecipeItem, Qt::UserRole );
		itemRecipe->setData( it->id, Qt::UserRole+1 );
		itemRecipe->setData( Categorized, CategorizedRole ); //recipe categorized
		itemRecipe->setEditable( false );

		//Insert the items in the model.
		item->setChild( current_row, 0, itemRecipe );
		item->setChild( current_row, 1, itemRecipeId );
			
		//Update the persistent index map.
		m_recipeIdToIndexMap.insert( it->id, itemRecipe->index() );

		++current_row;
	}

}

void KreRecipesListWidget::load( int limit, int offset )
{
	//Load the category tree.
	CategoryTree categoryTree;
	CategoryTree * pCategoryTree = &categoryTree;
	m_database->loadCachedCategories( &pCategoryTree, limit, offset, -1, true );

	//Load the uncategorized recipes.
	ElementList uncategorizedRecipes;
	m_database->loadUncategorizedRecipes( &uncategorizedRecipes );

	//FIXME: set the number of root categories or number+1 if they are uncategorized recipes.
	// Doing this will improve performance.
	int model_row_count = 0;
	//if ( !uncategorizedRecipes.isEmpty() ) {
	//	model_row_count++;
	//}
	m_sourceModel->setRowCount( model_row_count );

	m_recipeIdToIndexMap.clear();
	m_categoryIdToIndexMap.clear();

	if ( !uncategorizedRecipes.isEmpty() ) {

		//Add the "Uncategorized" item to the model.
		QStandardItem * uncategorizedItem = new QStandardItem;
		uncategorizedItem->setData( KIcon("folder-grey"), Qt::DecorationRole );
		uncategorizedItem->setData( 
			i18nc( "@item Uncategorized recipes", "Uncategorized" ), Qt::EditRole );
		uncategorizedItem->setData( OtherItem, ItemTypeRole );
		uncategorizedItem->setData( -1, IdRole );
		uncategorizedItem->setEditable( false );
		QList<QStandardItem*> items;
		items << uncategorizedItem << (new QStandardItem);
		m_sourceModel->appendRow( items );

		m_uncategorizedItemIndex = uncategorizedItem->index();

		//Add the uncategorized recipes to the model
		int current_row = 0;
		for ( ElementList::const_iterator it = uncategorizedRecipes.constBegin();
		it != uncategorizedRecipes.constEnd(); it++) {
			//The "Id" item.
			QStandardItem *itemRecipeId = new QStandardItem;
			itemRecipeId->setData( QVariant( it->id ), Qt::EditRole );
			itemRecipeId->setData( RecipeItem, ItemTypeRole );
			itemRecipeId->setData( it->id, IdRole );
			itemRecipeId->setData( Uncategorized, CategorizedRole ); //recipe uncategorized
			itemRecipeId->setEditable( false );

			//The "Recipe" item.
			QStandardItem *itemRecipe = new QStandardItem( it->name );
			itemRecipe->setData( KIcon("documentation"), Qt::DecorationRole );
			itemRecipe->setData( RecipeItem, Qt::UserRole );
			itemRecipe->setData( it->id, Qt::UserRole+1 );
			itemRecipe->setData( false, Qt::UserRole+2 );
			itemRecipeId->setData( Uncategorized, CategorizedRole ); //recipe uncategorized
			itemRecipe->setEditable( false );

			//Insert the items in the model.
			uncategorizedItem->setChild( current_row, 0, itemRecipe );
			uncategorizedItem->setChild( current_row, 1, itemRecipeId );

			//Update the persistent index map.
			m_recipeIdToIndexMap.insert( it->id, itemRecipe->index() );

			++current_row;
		}

	}

	//Add the rest of recipes and categories to the model.
        for ( CategoryTree * child_it = pCategoryTree->firstChild(); child_it; child_it = child_it->nextSibling() ) {
		int category_id = child_it->category.id;

		//The "Id" item.
		QStandardItem *itemId = new QStandardItem;
		itemId->setData( QVariant(category_id), Qt::EditRole );
		itemId->setData( CategoryItem, Qt::UserRole );
		itemId->setData( category_id, Qt::UserRole+1 );
		itemId->setEditable( false );

		//The "Category" item.
		QStandardItem *itemCategory = new QStandardItem( child_it->category.name );
		itemCategory->setData( KIcon("folder-yellow"), Qt::DecorationRole );
		itemCategory->setData( CategoryItem, Qt::UserRole );
		itemCategory->setData( category_id, Qt::UserRole+1 );
		itemCategory->setEditable( false );

		//Insert the items in the model.
		QList<QStandardItem*> items;
		items << itemCategory << itemId;
		m_sourceModel->appendRow( items );

		//Update the persistent index map.
		m_categoryIdToIndexMap[category_id] = itemCategory->index();

		//Populate the current element recursively.
		populate( itemCategory, category_id );
        }

	emit loadFinishedPrivate();
}

