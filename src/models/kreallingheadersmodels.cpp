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

	//Connect signals and slots for model updating
	connect( database, SIGNAL(ingHeaderCreated(const KreIngHeader &)),
		this, SLOT(ingHeaderCreatedDBSlot(const KreIngHeader &)) );
	connect( database, SIGNAL(ingHeaderModified(const KreIngHeader &)),
		this, SLOT(ingHeaderModifiedDBSlot(const KreIngHeader &)) );
	connect( database, SIGNAL(ingHeaderRemoved(const QVariant &)),
		this, SLOT(ingHeaderRemovedDBSlot(const QVariant &)) );
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

void KreAllIngHeadersModels::ingHeaderCreatedDBSlot( const KreIngHeader & ingHeader )
{
	//Prepare the id item
	QStandardItem * itemId = new QStandardItem;
	itemId->setData( ingHeader.id(), Qt::EditRole );
	itemId->setEditable( false );

	//Prepare the name item
	QStandardItem *itemName = new QStandardItem( ingHeader.name() );
	itemName->setData( ingHeader.name(), Qt::EditRole );
	itemName->setEditable( true );

	//Add the new row to the source model
	QList<QStandardItem*> itemList;
	itemList << itemId << itemName;
	m_sourceModel->appendRow( itemList );

	//Add the item to the completion object
	m_ingHeaderNameCompletion->addItem( ingHeader.name() );
}

void KreAllIngHeadersModels::ingHeaderModifiedDBSlot( const KreIngHeader & newIngHeader )
{
	int i;
	int rowCount;
	QModelIndex index;

	//Rename the ingredient header in the source model
	QVariant id;
	QString oldName;
	rowCount = m_sourceModel->rowCount();
	for ( i = 0; i < rowCount; ++i ) {
		index = m_sourceModel->index( i, 0 );
		id = m_sourceModel->data( index );
		if ( id == newIngHeader.id() ) {
			index = m_sourceModel->index( i, 1 );
			oldName = m_sourceModel->data( index, Qt::EditRole ).toString();
			m_sourceModel->setData( index, newIngHeader.name(), Qt::EditRole );
			break;
		}
	}

	//Delete the ingredient header name from the KCompletion if it was unique
	QString name;
	int oldNameMatches = 0;
	for ( i = 0; i < rowCount; ++i ) {
		index = m_sourceModel->index( i, 1 );
		name = m_sourceModel->data( index ).toString();
		if ( name == oldName ) {
			++oldNameMatches;
		}
	}

	//If the ingredient header name was unique, delete it from completion
	if ( oldNameMatches == 0 ) {
		m_ingHeaderNameCompletion->removeItem( oldName );
	}

	//Add the new ingredient header name to completion
	m_ingHeaderNameCompletion->addItem( newIngHeader.name() );
}

void KreAllIngHeadersModels::ingHeaderRemovedDBSlot( const QVariant & id )
{
	int i;
	int rowCount;
	QModelIndex index;

	//Delete the ingredient header in the source model
	QVariant currentId;
	QString oldName;
	rowCount = m_sourceModel->rowCount();
	for ( i = 0; i < rowCount; ++i ) {
		index = m_sourceModel->index( i, 0 );
		currentId = m_sourceModel->data( index );
		if ( currentId == id ) {
			index = m_sourceModel->index( i, 1 );
			oldName = m_sourceModel->data( index, Qt::EditRole ).toString();
			m_sourceModel->takeRow( i );
			break;
		}
	}

	//Delete the ingredient header name from the KCompletion if it was unique
	QString name;
	int oldNameMatches = 0;
	for ( i = 0; i < rowCount; ++i ) {
		index = m_sourceModel->index( i, 1 );
		name = m_sourceModel->data( index ).toString();
		if ( name == oldName ) {
			++oldNameMatches;
		}
	}

	//If the ingredient header name was unique, delete it from completion
	if ( oldNameMatches == 0 ) {
		m_ingHeaderNameCompletion->removeItem( oldName );
	}
}
