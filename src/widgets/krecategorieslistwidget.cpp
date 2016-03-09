/***************************************************************************
*   Copyright © 2012-2016 José Manuel Santamaría Lema <panfaust@gmail.com> *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

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


KreCategoriesListWidget::KreCategoriesListWidget( QWidget *parent, RecipeDB *db, bool itemsCheckable):
	KreGenericListWidget( parent, db ),
	m_itemsCheckable( itemsCheckable )
{

	if ( itemsCheckable ) {
		m_checkedCategories = new ElementList;
	} else {
		m_checkedCategories = 0;
	}

	//This is an unusual column to filter by
	m_proxyModel->setFilterKeyColumn( 0 );

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
		this, SLOT( createCategorySlot( const Element &, int ) ) );
	connect( m_database, SIGNAL( categoryModified( const Element & ) ),
		SLOT( modifyCategory( const Element & ) ) );
	connect( m_database, SIGNAL( categoryModified( int, int ) ),
		SLOT( modifyCategory( int, int ) ) );
	connect( m_database, SIGNAL( categoryRemoved( int ) ), 
		SLOT( removeCategory( int ) ) );

	connect( this, SIGNAL( itemsChanged( const QModelIndex &, const QModelIndex & ) ),
		SLOT( itemsChangedSlot( const QModelIndex &, const QModelIndex & ) ) );
}

KreCategoriesListWidget::~KreCategoriesListWidget()
{
	delete m_checkedCategories;
}

void KreCategoriesListWidget::checkCategories( const ElementList & items_on )
{
	ElementList::const_iterator it;
	for ( it = items_on.begin(); it != items_on.end(); it++ ) {
		QPersistentModelIndex index = m_categoryIdToIndexMap[it->id];
		QStandardItem * item = m_sourceModel->itemFromIndex( index );
		item->setCheckState( Qt::Checked );
	}
}

ElementList KreCategoriesListWidget::checkedCategories()
{
	return *m_checkedCategories;
}

void KreCategoriesListWidget::edit( int row, const QModelIndex & parent )
{
	QModelIndex index = m_proxyModel->index( row, 0, parent );
	ui->m_treeView->edit( index );
}

void KreCategoriesListWidget::expandAll()
{
	ui->m_treeView->expandAll();
}

void KreCategoriesListWidget::collapseAll()
{
	ui->m_treeView->collapseAll();
}

void KreCategoriesListWidget::createCategorySlot( const Element & category, int parent_id )
{
	//The "Id" item.
	QStandardItem *itemId = new QStandardItem;
	itemId->setData( QVariant(category.id), Qt::EditRole );
	itemId->setData( category.id, IdRole );
	itemId->setEditable( false );

	//The "Category" item.
	QStandardItem *itemCategory = new QStandardItem( category.name );
	itemCategory->setData( KIcon("folder-yellow"), Qt::DecorationRole );
	itemCategory->setData( category.id, IdRole );
	itemCategory->setEditable( true );
	if ( m_itemsCheckable ) {
		itemCategory->setCheckable( true );
	}

	//Insert the items in the model.
	QModelIndex parentIndex;
	if ( parent_id != RecipeDB::InvalidId ) {
		parentIndex = m_categoryIdToIndexMap[parent_id];
	}
	QStandardItem * parentItem = m_sourceModel->itemFromIndex( parentIndex );
	QList<QStandardItem*> items;
	items << itemCategory << itemId;
	if ( parentItem ) {
		parentItem->appendRow( items );
	} else {
		m_sourceModel->appendRow( items );
	}

	//Update the persistent index map.
	m_categoryIdToIndexMap[category.id] = itemCategory->index();

	//Sort
	m_proxyModel->sort( 0 );
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
		itemId->setData( child_it->category.id, IdRole );
		itemId->setEditable( false );

		//The "Category" item.
		QStandardItem *itemCategory = new QStandardItem( child_it->category.name );
		itemCategory->setData( KIcon("folder-yellow"), Qt::DecorationRole );
		itemCategory->setData( child_it->category.id, IdRole );
		itemCategory->setEditable( true );
		if ( m_itemsCheckable ) {
			itemCategory->setCheckable( true );
		}
	
		//Insert the items as children
		item->setChild( current_row, 0, itemCategory );
		item->setChild( current_row, 1, itemId );

		//Update the persistent index map.
		m_categoryIdToIndexMap[child_it->category.id] = itemCategory->index();

		//Populate this node with its subcategories.
		populate( itemCategory, child_it->category.id );
		++current_row;

	}

}

void KreCategoriesListWidget::modifyCategory( const Element & category )
{
	Q_UNUSED( category );
	m_proxyModel->sort( 0 );
}

void KreCategoriesListWidget::modifyCategory( int category_id, int new_parent_id )
{
	//FIXME: too slow
	Q_UNUSED( category_id );
	Q_UNUSED( new_parent_id );
	reload( ForceReload );
}

void KreCategoriesListWidget::removeCategory( int id )
{
	QModelIndex index = m_categoryIdToIndexMap[id];
	QStandardItem * parentItem = m_sourceModel->itemFromIndex( index.parent() );
	if ( parentItem ) {
		parentItem->removeRow( index.row() );
	} else {
		m_sourceModel->removeRow( index.row() );
	}
	m_categoryIdToIndexMap.remove(id);
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
		itemId->setData( child_it->category.id, IdRole );
		itemId->setEditable( false );

		//The "Category" item.
		QStandardItem *itemCategory = new QStandardItem( child_it->category.name );
		itemCategory->setData( KIcon("folder-yellow"), Qt::DecorationRole );
		itemCategory->setData( child_it->category.id, IdRole );
		itemCategory->setEditable( true );
		if ( m_itemsCheckable ) {
			itemCategory->setCheckable( true );
		}

		//Insert the items in the model.
		QList<QStandardItem*> items;
		items << itemCategory << itemId;
		m_sourceModel->appendRow( items );

		//Update the persistent index map.
		m_categoryIdToIndexMap[child_it->category.id] = itemCategory->index();

		//Populate the current element.
		populate( itemCategory, child_it->category.id );
        }

	emit loadFinishedPrivate();
}

int KreCategoriesListWidget::idColumn()
{
	return 1;
}

void KreCategoriesListWidget::setFilter( const QString & filter )
{
	KreGenericListWidget::setFilter( filter );
	this->expandAll();
}

void KreCategoriesListWidget::itemsChangedSlot( const QModelIndex & topLeft,
	const QModelIndex & bottomRight )
{
	Q_UNUSED(bottomRight)
	if ( m_checkedCategories ) {
		QModelIndex sourceIndex = m_proxyModel->mapToSource( topLeft );
		if ( m_sourceModel->itemFromIndex( sourceIndex )->checkState() == Qt::Checked ) {
			Element el;
			el.id = m_proxyModel->data( topLeft, IdRole).toInt();
			el.name = m_proxyModel->data( topLeft, Qt::EditRole).toString();
			(*m_checkedCategories) << el;
		} else {
			Element el;
			el.id = m_proxyModel->data( topLeft, IdRole).toInt();
			el.name = m_proxyModel->data( topLeft, Qt::EditRole).toString();
			m_checkedCategories->removeOne( el );
		}
	}
}

#include "krecategorieslistwidget.moc"
