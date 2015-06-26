/***************************************************************************
*   Copyright © 2015 José Manuel Santamaría Lema <panfaust@gmail.com>      *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#include "kresearchresultlistwidget.h"

#include "ui_kregenericlistwidget.h"

//#include <kdebug.h>

#include <QStandardItemModel>
#include <QSortFilterProxyModel>



KreSearchResultListWidget::KreSearchResultListWidget( QWidget *parent, RecipeDB *db ):
	KreRecipesListWidget( parent, db )
{
	ui->m_treeView->setRootIsDecorated( false );
	ui->m_searchBox->hide();
}

void KreSearchResultListWidget::showNotFoundMessage(const QString & message )
{
	clear();

	//The "Id" item.
	QStandardItem *itemId = new QStandardItem;
	itemId->setData( QVariant(), Qt::EditRole );
	itemId->setData( OtherItem, ItemTypeRole );
	itemId->setEditable( false );

	//The "Recipe" item. This is the one showing the "not found' message.
	QStandardItem *itemRecipe = new QStandardItem( message );
	itemRecipe->setData( OtherItem, ItemTypeRole );
	itemRecipe->setEditable( false );

	//Add the items to the model.
	QList<QStandardItem *> items;
	items << itemRecipe << itemId;
	m_sourceModel->appendRow( items );

}

void KreSearchResultListWidget::displayRecipes( const RecipeList & recipes )
{
	clear();

	//Allocate memory for the items.
	m_sourceModel->setRowCount( recipes.count() );

	//Display the recipes in the items.
	RecipeList::const_iterator it; 
	QModelIndex index;
	int row_count = 0;
	for ( it = recipes.constBegin(); it != recipes.constEnd(); ++it ) {
		//The "Id" item.
		index = m_sourceModel->index( row_count, 1 );
		m_sourceModel->setData( index, it->recipeID, Qt::EditRole );
		m_sourceModel->setData( index, RecipeItem, ItemTypeRole );
		m_sourceModel->setData( index, it->recipeID, IdRole );
		//m_sourceModel->setData( status, CategorizedRole );
		m_sourceModel->itemFromIndex( index )->setEditable( false );

		//The "Recipe" item.
		index = m_sourceModel->index( row_count, 0 );
		m_sourceModel->setData( index, it->title, Qt::EditRole );
		m_sourceModel->setData( index, KIcon("documentation"), Qt::DecorationRole );
		m_sourceModel->setData( index, RecipeItem, ItemTypeRole );
		m_sourceModel->setData( index, it->recipeID, IdRole );
		//m_sourceModel->setData( status, CategorizedRole );
		m_sourceModel->itemFromIndex( index )->setEditable( false );

		//Update the persistent index map.
		m_recipeIdToIndexMap.insert( it->recipeID, index );

		row_count++;
	}

	//Sort the results.
	m_proxyModel->sort( 0 );
}

		
void KreSearchResultListWidget::modifyRecipeSlot( const Element & recipe, const ElementList & categories)
{
	Q_UNUSED(categories)

	//Get the index.
	QModelIndex index = m_recipeIdToIndexMap.values( recipe.id ).first();

	//Change the "Recipe" item.
	m_sourceModel->setData( index, recipe.name, Qt::EditRole );

}

