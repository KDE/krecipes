/***************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>      *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "ingredientseditor.h"
#include "ui_ingredientseditor.h"

#include "datablocks/ingredientlist.h"
#include "backends/recipedb.h"

#include <QStandardItemModel>

//#include "kdebug.h"

IngredientsEditor::IngredientsEditor( QWidget * parent)
		: QWidget( parent )
{
	ui = new Ui::IngredientsEditor;
	ui->setupUi( this );

	m_sourceModel = new QStandardItemModel;
	ui->m_treeView->setModel( m_sourceModel );


	//The horizontal column labels.
	QStringList horizontalLabels;
	horizontalLabels << i18nc( "@title:column", "Ingredient" )
		<< i18nc( "@title:column", "Amount" )
		<< i18nc( "@title:column", "Units" )
		<< i18nc( "@title:column", "Preparation Method" )
		<< i18nc( "@title:column", "Id" );
	m_sourceModel->setHorizontalHeaderLabels( horizontalLabels );


	//Connect signals/slots
	connect( ui->m_moveUpButton, SIGNAL(clicked()),
		this, SLOT(moveIngredientUpSlot()) );
	connect( ui->m_moveDownButton, SIGNAL(clicked()),
		this, SLOT(moveIngredientDownSlot()) );

	connect( ui->m_deleteButton, SIGNAL(clicked()),
		this, SLOT(removeIngredientSlot()) );
}

void IngredientsEditor::loadIngredients( IngredientList * ingredientList )
{
	m_ingredientList = ingredientList;

	m_sourceModel->setRowCount( 0 );
	m_sourceModel->setRowCount( ingredientList->count() );

	IngredientList::const_iterator it;
	int current_row = 0;
	QModelIndex index;
	RecipeDB::IdType lastGroupId = RecipeDB::InvalidId;
	for ( it = ingredientList->begin(); it != ingredientList->end(); ++it ) {
		//Check if we have to add a header
		if ( it->groupID != lastGroupId ) {
			m_sourceModel->insertRows( current_row, 1 );
			//The "Id" item.
			index = m_sourceModel->index( current_row, 4 );
			m_sourceModel->setData( index, QVariant(it->groupID), Qt::EditRole );
			m_sourceModel->itemFromIndex( index )->setEditable( false );
			//The "Header" item.
			index = m_sourceModel->index( current_row, 0 );
			m_sourceModel->setData( index, QVariant(it->group), Qt::EditRole );
			QStandardItem * headerItem = m_sourceModel->itemFromIndex( index );
			headerItem->setEditable( true );
			QFont font = headerItem->font();
			font.setBold( true );
			font.setUnderline( true );
			headerItem->setFont( font );
			//Set the rest of the empty columns (headers have no amount, units
			// or preparation method) as read only
			QStandardItem * item;
			for ( int i = 1; i <= 3; ++i ) {
				index = m_sourceModel->index( current_row, i );
				item = m_sourceModel->itemFromIndex( index );
				item->setEditable( false );
			}
			//Increase row count
			++current_row;
		}
		//The "Id" item.
		index = m_sourceModel->index( current_row, 4 );
		m_sourceModel->setData( index, QVariant(it->ingredientID), Qt::EditRole );
		m_sourceModel->itemFromIndex( index )->setEditable( false );
		//The "Ingredient" item.
		index = m_sourceModel->index( current_row, 0 );
		m_sourceModel->setData( index, QVariant(it->name), Qt::EditRole );
		QStandardItem * ingItem = m_sourceModel->itemFromIndex( index );
		ingItem->setEditable( true );
		//The "Amount" item.
		index = m_sourceModel->index( current_row, 1 );
		m_sourceModel->setData( index, QVariant(it->amountString()), Qt::EditRole );
		m_sourceModel->itemFromIndex( index )->setEditable( true );
		//The "Units" item.
		index = m_sourceModel->index( current_row, 2 );
		m_sourceModel->setData( index, QVariant(it->amountUnitString()), Qt::EditRole );
		m_sourceModel->itemFromIndex( index )->setEditable( true );
		//The "PreparationMethod" item.
		index = m_sourceModel->index( current_row, 3 );
		QString prepMethodListString = it->prepMethodList.join(", ");
		if ( !it->prepMethodList.isEmpty() )
			m_sourceModel->setData( index, QVariant(prepMethodListString), Qt::EditRole );
		m_sourceModel->itemFromIndex( index )->setEditable( true );

		//Add the susbtitutes, if any
		Ingredient::SubstitutesList::const_iterator sub_it;
		Ingredient::SubstitutesList::const_iterator sub_it_begin = it->substitutes.constBegin();
		Ingredient::SubstitutesList::const_iterator sub_it_end = it->substitutes.constEnd();
		ingItem->setRowCount( it->substitutes.count() );
		int sub_row_count = 0;
		for ( sub_it = sub_it_begin; sub_it != sub_it_end; ++sub_it ) {
			//Ingredient name
			QStandardItem * subItem = new QStandardItem;
			subItem->setData( QVariant(it->name), Qt::EditRole );
			subItem->setData( QVariant( i18n("OR") + " " + sub_it->name), Qt::DisplayRole );
			ingItem->setChild( sub_row_count, 0, subItem );
			//Ingredient amount
			//(substitutes can't have an amount so adding a dummy item)
			QStandardItem * subItemAmount = new QStandardItem;
			subItemAmount->setEditable( false );
			ingItem->setChild( sub_row_count, 1, subItemAmount );
			//Ingredient units
			//(substitutes can't have an amount so adding a dummy item)
			QStandardItem * subItemUnits = new QStandardItem;
			subItemUnits->setEditable( false );
			ingItem->setChild( sub_row_count, 2, subItemUnits );
			//Preparation method
			//(substitutes can't have a preparation method so adding a dummy item)
			QStandardItem * subItemPrep = new QStandardItem;
			subItemPrep->setEditable( false );
			ingItem->setChild( sub_row_count, 3, subItemPrep );
			//Substitute ingredient ID
			QStandardItem * subItemId = new QStandardItem;
			subItemId->setEditable( false );
			subItemId->setData( QVariant(sub_it->ingredientID), Qt::DisplayRole );
			ingItem->setChild( sub_row_count, 4, subItemId );
			//Increase substitutes row count
			++sub_row_count;
		}
		lastGroupId = it->groupID;
		++current_row;
	}
	ui->m_treeView->expandAll();
	resizeColumnsToContents();
}

void IngredientsEditor::resizeColumnsToContents()
{
	int columnCount = m_sourceModel->columnCount();
	for ( int i = 0; i < columnCount; ++i ) {
		ui->m_treeView->resizeColumnToContents( i );
	}
}


void IngredientsEditor::moveColumn( int offset )
{
	QModelIndex index = ui->m_treeView->currentIndex();
	int row = index.row();
	int col = index.column();
	if ( index.parent() != QModelIndex() ) {
		QStandardItem * parentItem = m_sourceModel->itemFromIndex( index.parent() );
		int rowCount = parentItem->rowCount();
		//Abort if we are moving the elements beyond the limit of the list
		//e.g. moving up the first element or moving down the last one
		if ( (row+offset < 0) || (row+offset > rowCount-1) ) {
			return;
		}
		//Move the item
		QList<QStandardItem*> itemList;
		itemList = parentItem->takeRow(row);
		parentItem->insertRow( row += offset, itemList );
		//Re-select it in the QTreeView
		index = m_sourceModel->indexFromItem( parentItem->child( row, col ) );
		ui->m_treeView->setCurrentIndex( index );
	} else {
		int rowCount = m_sourceModel->rowCount();
		//Abort if we are moving the elements beyond the limit of the list
		//e.g. moving up the first element or moving down the last one
		if ( (row+offset < 0) || (row+offset > rowCount-1) ) {
			return;
		}
		//Check if we have children and save it to re-add them after the moving
		bool gotChildren = false;
		QList< QList<QStandardItem*> > childRowItems;
		if ( m_sourceModel->hasChildren( index ) ) {
			QStandardItem * item = m_sourceModel->itemFromIndex( index );
			for ( int i = 0; i < item->rowCount(); ++i ) {
				childRowItems << m_sourceModel->itemFromIndex(index)->takeRow( 0 );
			}
			gotChildren = true;
		}
		//Move the item
		QList<QStandardItem*> itemList;
		itemList = m_sourceModel->takeRow(row);
		m_sourceModel->insertRow( row += offset, itemList );
		//Restore the children
		QStandardItem * item = m_sourceModel->item( row, 0 );
		if ( gotChildren ) {
			int childRow = 0;
			QList< QList<QStandardItem*> >::const_iterator it;
			for ( it = childRowItems.constBegin(); it != childRowItems.constEnd(); ++it ) {
				item->insertRow( childRow, *it );
				++childRow;
			}
		}
		//Re-select the moved ingredient in the QTreeView
		ui->m_treeView->expandAll();
		ui->m_treeView->setCurrentIndex( m_sourceModel->index( row, col ) );
	}
}

void IngredientsEditor::moveIngredientUpSlot()
{
	moveColumn( -1 );
}

void IngredientsEditor::moveIngredientDownSlot()
{
	moveColumn( +1 );
}

void IngredientsEditor::removeIngredientSlot()
{
	QModelIndex index = ui->m_treeView->currentIndex();
	m_sourceModel->removeRow( index.row(), index.parent() );
}
