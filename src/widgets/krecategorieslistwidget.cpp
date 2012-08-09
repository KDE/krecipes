/***************************************************************************
*   Copyright © 2012 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "krecategorieslistwidget.h"
#include "ui_kregenericlistwidget.h"

#include <KLocalizedString>
#include <KApplication>
#include <KConfigGroup>
#include <KGlobal>

#include <QStandardItemModel>

#include "datablocks/categorytree.h"
#include "backends/recipedb.h"


KreCategoriesListWidget::KreCategoriesListWidget( QWidget *parent, RecipeDB *db ):
	KreGenericListWidget( parent, db )
{

	//The QTreeView.
	ui->m_treeView->setRootIsDecorated( true );
	ui->m_treeView->showColumn( 0 );
	ui->m_treeView->hideColumn( 1 );

	//The horizontal column labels.
	QStringList horizontalLabels;
	horizontalLabels << i18nc( "@title:column", "Categories" ) << "Id";
	m_sourceModel->setHorizontalHeaderLabels( horizontalLabels );

	//The maximum number of elements to show in the author list.
	KConfigGroup config = KGlobal::config()->group( "Performance" );
	setCurrentLimit( config.readEntry( "CategoryLimit", -1 ) );

	/*connect( m_database, SIGNAL( categoryCreated( const Element &, int) ), 
		SLOT( createCategory( const Element &, int ) ) );
	connect( m_database, SIGNAL( categoryRemoved( int ) ), 
		SLOT( removeCategory( int ) ) );*/

}

void KreCategoriesListWidget::createCategory( const Element & category, int parent_id )
{
	//reload( ForceReload );
}

void KreCategoriesListWidget::populate( QStandardItem * item, int id )
{
	int current_row = 0;

	CategoryTree categoryTree;
	m_database->loadCategories( &categoryTree, -1, 0, id, false );

	for ( CategoryTree * child_it = categoryTree.firstChild(); 
	child_it; child_it = child_it->nextSibling() ) {
		//The "Id" item.
		QStandardItem *itemId = new QStandardItem;
		itemId->setData( QVariant(child_it->category.id), Qt::EditRole );
		itemId->setEditable( false );

		//The "Category" item.
		QStandardItem *itemCategory = new QStandardItem( child_it->category.name );
		itemCategory->setEditable( true );
	
		//Insert the items as children
		item->setChild( current_row, 0, itemCategory );
		item->setChild( current_row, 1, itemId );
		populate( itemCategory, child_it->category.id );
		++current_row;
	}

}

void KreCategoriesListWidget::removeCategory( int id )
{
	//reload( ForceReload );
}

void KreCategoriesListWidget::load( int limit, int offset )
{
	CategoryTree categoryTree;
	CategoryTree * pCategoryTree = &categoryTree;
	m_database->loadCachedCategories( &pCategoryTree, -1, -1, -1, true );

        for ( CategoryTree * child_it = pCategoryTree->firstChild(); child_it; child_it = child_it->nextSibling() ) {
		//The "Id" item.
		QStandardItem *itemId = new QStandardItem;
		itemId->setData( QVariant(child_it->category.id), Qt::EditRole );
		itemId->setEditable( false );

		//The "Category" item.
		QStandardItem *itemCategory = new QStandardItem( child_it->category.name );
		itemCategory->setEditable( true );

		//Insert the items in the model.
		QList<QStandardItem*> items;
		items << itemCategory << itemId;
		m_sourceModel->appendRow( items );

		//Populate the current element.
		populate( itemCategory, child_it->category.id );
        }
}


#include "krecategorieslistwidget.moc"
