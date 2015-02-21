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
#include <QSortFilterProxyModel>

#include "datablocks/categorytree.h"
#include "backends/recipedb.h"


KreCategoriesListWidget::KreCategoriesListWidget( QWidget *parent, RecipeDB *db ):
	KreGenericListWidget( parent, db )
{

	//The QTreeView.
	ui->m_treeView->setRootIsDecorated( true );
	ui->m_treeView->showColumn( 0 );
	KConfigGroup configAdvanced( KGlobal::config(), "Advanced" );
	if ( !configAdvanced.readEntry( "ShowID", false ) ) {
		ui->m_treeView->hideColumn( 1 );
	}
	ui->m_treeView->header()->setStretchLastSection( false );
	ui->m_treeView->header()->setResizeMode( 0, QHeaderView::Stretch );


	//The horizontal column labels.
	QStringList horizontalLabels;
	horizontalLabels << i18nc( "@title:column", "Categories" ) << i18nc( "@title:column", "Id" );
	m_sourceModel->setHorizontalHeaderLabels( horizontalLabels );

	//The maximum number of elements to show in the author list.
	KConfigGroup configPerformance = KGlobal::config()->group( "Performance" );
	setCurrentLimit( configPerformance.readEntry( "CategoryLimit", -1 ) );

	connect( m_database, SIGNAL( categoryCreated( const Element &, int) ), 
		SLOT( createCategory( const Element &, int ) ) );
	connect( m_database, SIGNAL( categoryRemoved( int ) ), 
		SLOT( removeCategory( int ) ) );

}

void KreCategoriesListWidget::createCategory( const Element & category, int parent_id )
{
          Q_UNUSED(category);
          Q_UNUSED(parent_id);
	reload( ForceReload );
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
          Q_UNUSED(id);
	reload( ForceReload );
}

int KreCategoriesListWidget::elementCount()
{
	return m_database->categoryTopLevelCount();
}

void KreCategoriesListWidget::load( int limit, int offset )
{
	CategoryTree categoryTree;
	CategoryTree * pCategoryTree = &categoryTree;
	m_sourceModel->setRowCount( 0 );
	m_database->loadCachedCategories( &pCategoryTree, limit, offset, -1, true );

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

	emit loadFinishedPrivate();
}

int KreCategoriesListWidget::idColumn()
{
	return 1;
}


#include "krecategorieslistwidget.moc"
