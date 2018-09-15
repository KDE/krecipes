/***************************************************************************
*   Copyright © 2015 José Manuel Santamaría Lema <panfaust@gmail.org>      *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#include "kreheaderlistwidget.h"

#include <KConfigGroup>
#include <KLocale>
#include <KGlobal>
#include <QModelIndex>
#include <QStandardItemModel>

#include "backends/recipedb.h"


KreHeaderListWidget::KreHeaderListWidget( QWidget * parent, RecipeDB *db ): 
	KreGenericListWidget( parent, db )
{
	//The horizontal column labels.
	QStringList horizontalLabels;
	horizontalLabels << i18nc( "@title:column", "Id" )
		<< i18nc( "@title:column", "Header" );
	m_sourceModel->setHorizontalHeaderLabels( horizontalLabels );

	//The maximum number of elements to show in the author list.
	//FIXME: The database doesn't support limit and offset for headers
	//KConfigGroup config = KGlobal::config()->group( "Performance" );
	//setCurrentLimit( config.readEntry( "Limit", -1 ) );

	//Connect signals and slots.
	connect( m_database, SIGNAL(ingGroupCreated(Element)), 
		SLOT(createHeader(Element)) );
	connect( m_database, SIGNAL(ingGroupRemoved(int)), 
		SLOT(removeHeader(int)) );
}


int KreHeaderListWidget::elementCount()
{
	return m_database->ingredientGroupCount();
}


void KreHeaderListWidget::load( int limit, int offset )
{
	//FIXME: The database doesn't support limit and offset for headers
	Q_UNUSED(limit)
	Q_UNUSED(offset)
	
	ElementList headerList;
	int numberOfHeaders = m_database->loadIngredientGroups( &headerList );
	m_sourceModel->setRowCount( numberOfHeaders );

	int current_row = 0;
	QModelIndex index;
	for ( ElementList::const_iterator it = headerList.constBegin(); 
	it != headerList.constEnd(); ++it ) {
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


void KreHeaderListWidget::cancelLoad()
{
	//We do nothing here because we are not using threads.
}


int KreHeaderListWidget::idColumn()
{
 	//This function must return the column number where the id is stored.
	return 0;
}


void KreHeaderListWidget::createHeader( const Element &el )
{
	//FIXME: This could be more efficient
	Q_UNUSED(el)
	reload( ForceReload );
}


void KreHeaderListWidget::removeHeader( int id )
{
	//FIXME: This could be more efficient
	Q_UNUSED(id)
	reload( ForceReload );
}

