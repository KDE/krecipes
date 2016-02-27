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

}

void IngredientsEditor::loadIngredients( IngredientList * ingredientList )
{
	m_ingredientList = ingredientList;
	//kDebug() << *ingredientList;

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
