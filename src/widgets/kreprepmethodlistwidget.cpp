/***************************************************************************
*   Copyright © 2015 José Manuel Santamaría Lema <panfaust@gmail.com>      *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#include "kreprepmethodlistwidget.h"

#include "ui_kregenericlistwidget.h"

#include <QStandardItemModel>

#include "backends/recipedb.h"


KrePrepMethodListWidget::KrePrepMethodListWidget( QWidget *parent, RecipeDB *db )
	: KreGenericListWidget( parent, db )
{
	//The horizontal column labels.
	QStringList horizontalLabels;
	horizontalLabels << i18nc( "@title:column", "Id" )
		<< i18nc( "@title:column", "Preparation Method" );
	m_sourceModel->setHorizontalHeaderLabels( horizontalLabels );

	//The maximum number of elements to show in the author list.
	KConfigGroup config = KGlobal::config()->group( "Performance" );
	setCurrentLimit( config.readEntry( "Limit", -1 ) );

	connect( m_database, SIGNAL(prepMethodCreated(Element)), SLOT(createPrepMethod(Element)) );
	connect( m_database, SIGNAL(prepMethodRemoved(int)), SLOT(removePrepMethod(int)) );

}


int KrePrepMethodListWidget::elementCount()
{
	return m_database->prepMethodCount();
}


void KrePrepMethodListWidget::load(int limit, int offset)
{
	ElementList prepMethodList;
	int numberOfPrepMethods = m_database->loadPrepMethods( &prepMethodList, limit, offset );
	m_sourceModel->setRowCount( numberOfPrepMethods );

	int current_row = 0;
	QModelIndex index;
	for ( ElementList::const_iterator it = prepMethodList.constBegin(); 
	it != prepMethodList.constEnd(); ++it ) {
		// Write the database id in the model.
		index = m_sourceModel->index( current_row, 0 );
		m_sourceModel->setData( index, QVariant(it->id), Qt::EditRole );
		m_sourceModel->itemFromIndex( index )->setEditable( false );
		// Write the name of the unit (singular) in the model.
		index = m_sourceModel->index( current_row, 1 );
		m_sourceModel->setData( index, QVariant(it->name), Qt::EditRole );
		m_sourceModel->itemFromIndex( index )->setEditable( true );
		current_row++;
	}
	emit loadFinishedPrivate();
}


void KrePrepMethodListWidget::cancelLoad()
{
	//We do nothing here because we are not using threads.
}


int KrePrepMethodListWidget::idColumn()
{
 	//This function must return the column number where the id is stored.
	return 0;
}


void KrePrepMethodListWidget::createPrepMethod( const Element &el )
{
	//FIXME: This could be more efficient
	Q_UNUSED(el)
	reload( ForceReload );
}


void KrePrepMethodListWidget::removePrepMethod( int id )
{
	//FIXME: This could be more efficient
	Q_UNUSED(id)
	reload( ForceReload );
}

