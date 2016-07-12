/***************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gail.com>       *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#include "kreallingheadersmodels.h"

#include "kresinglecolumnproxymodel.h"
#include "backends/recipedb.h"

#include <KCompletion>
#include <QStandardItemModel>


KreAllIngHeadersModels::KreAllIngHeadersModels( RecipeDB * database ):
	m_database( database )
{

	//Create models
	m_sourceModel = new QStandardItemModel( this );
	m_ingHeaderNameModel = new KreSingleColumnProxyModel( 1, this );
	m_ingHeaderNameModel->setSortCaseSensitivity( Qt::CaseInsensitive );
	m_ingHeaderNameModel->setSourceModel( m_sourceModel );
	m_ingHeaderNameCompletion = new KCompletion;

//	//Connect signals and slots for model updating
//	connect( database, SIGNAL(ingredientCreated(const KreIngredient &)),
//		this, SLOT(ingredientCreatedDBSlot(const KreIngredient &)) );
//	connect( database, SIGNAL(ingredientModified(const KreIngredient &)),
//		this, SLOT(ingredientModifiedDBSlot(const KreIngredient &)) );
//	connect( database, SIGNAL(ingredientRemoved(const QVariant &)),
//		this, SLOT(ingredientRemovedDBSlot(const QVariant &)) );
}

KreAllIngHeadersModels::~KreAllIngHeadersModels()
{
	delete m_ingHeaderNameCompletion;
}

QStandardItemModel * KreAllIngHeadersModels::sourceModel()
{
	return m_sourceModel;
}

KreSingleColumnProxyModel * KreAllIngHeadersModels::ingHeaderNameModel()
{
	return m_ingHeaderNameModel;
}

KCompletion * KreAllIngHeadersModels::ingHeaderNameCompletion()
{
	return m_ingHeaderNameCompletion;
}

//void KreAllIngredientsModels::ingredientCreatedDBSlot( const KreIngredient & ingredient )
//{
//	//Prepare the id item
//	QStandardItem * itemId = new QStandardItem;
//	itemId->setData( ingredient.id(), Qt::EditRole );
//        itemId->setEditable( false );
//
//        //Prepare the name item
//        QStandardItem *itemName = new QStandardItem( ingredient.name() );
//	itemName->setData( ingredient.name(), Qt::EditRole );
//        itemName->setEditable( true );
//
//	//Add the new row to the source model
//	QList<QStandardItem*> itemList;
//	itemList << itemId << itemName;
//	m_sourceModel->appendRow( itemList );
//
//	//Add the item to the completion object
//	m_ingredientNameCompletion->addItem( ingredient.name() );
//}
//
//void KreAllIngredientsModels::ingredientModifiedDBSlot( const KreIngredient & newIngredient )
//{
//	int i;
//	int rowCount;
//	QModelIndex index;
//
//	//Rename the ingredient in the source model
//	QVariant id;
//	QString oldName;
//	rowCount = m_sourceModel->rowCount();
//	for ( i = 0; i < rowCount; ++i ) {
//		index = m_sourceModel->index( i, 0 );
//		id = m_sourceModel->data( index );
//		if ( id == newIngredient.id() ) {
//			index = m_sourceModel->index( i, 1 );
//			oldName = m_sourceModel->data( index, Qt::EditRole ).toString();
//			m_sourceModel->setData( index, newIngredient.name(), Qt::EditRole );
//			break;
//		}
//	}
//
//	//Delete the ingredient name from the KCompletion if it was unique
//	QString name;
//	int oldNameMatches = 0;
//	for ( i = 0; i < rowCount; ++i ) {
//		index = m_sourceModel->index( i, 1 );
//		name = m_sourceModel->data( index ).toString();
//		if ( name == oldName ) {
//			++oldNameMatches;
//		}
//	}
//
//	//If the ingredient name was unique, delete it from completion
//	if ( oldNameMatches == 0 ) {
//		m_ingredientNameCompletion->removeItem( oldName );
//	}
//
//	//Add the new ingredient name to completion
//	m_ingredientNameCompletion->addItem( newIngredient.name() );
//
//}
//
//void KreAllIngredientsModels::ingredientRemovedDBSlot( const QVariant & id )
//{
//	int i;
//	int rowCount;
//	QModelIndex index;
//
//	//Delete the ingredient in the source model
//	QVariant currentId;
//	QString oldName;
//	rowCount = m_sourceModel->rowCount();
//	for ( i = 0; i < rowCount; ++i ) {
//		index = m_sourceModel->index( i, 0 );
//		currentId = m_sourceModel->data( index );
//		if ( currentId == id ) {
//			index = m_sourceModel->index( i, 1 );
//			oldName = m_sourceModel->data( index, Qt::EditRole ).toString();
//			m_sourceModel->takeRow( i );
//			break;
//		}
//	}
//
//	//Delete the ingredient name from the KCompletion if it was unique
//	QString name;
//	int oldNameMatches = 0;
//	for ( i = 0; i < rowCount; ++i ) {
//		index = m_sourceModel->index( i, 1 );
//		name = m_sourceModel->data( index ).toString();
//		if ( name == oldName ) {
//			++oldNameMatches;
//		}
//	}
//
//	//If the ingredient name was unique, delete it from completion
//	if ( oldNameMatches == 0 ) {
//		m_ingredientNameCompletion->removeItem( oldName );
//	}
//
//}
