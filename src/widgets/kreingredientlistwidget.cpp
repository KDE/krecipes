/***************************************************************************
*   Copyright © 2015 José Manuel Santamaría Lema <panfaust@gmail.org>      *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#include "kreingredientlistwidget.h"

#include <KConfigGroup>
#include <KLocale>
#include <KGlobal>
#include <QModelIndex>
#include <QStandardItemModel>

#include "backends/recipedb.h"


KreIngredientListWidget::KreIngredientListWidget( QWidget * parent, RecipeDB *db ): 
	KreGenericListWidget( parent, db )
{
	//The horizontal column labels.
	QStringList horizontalLabels;
	horizontalLabels << i18nc( "@title:column", "Id" )
		<< i18nc( "@title:column", "Ingredient" );
	m_sourceModel->setHorizontalHeaderLabels( horizontalLabels );

	//The maximum number of elements to show in the author list.
	KConfigGroup config = KGlobal::config()->group( "Performance" );
	setCurrentLimit( config.readEntry( "Limit", -1 ) );

	//Connect signals and slots.
	connect( m_database, SIGNAL( ingredientCreated( const Element & ) ), 
		SLOT( createIngredient( const Element & ) ) );
	connect( m_database, SIGNAL( ingredientRemoved( int ) ), 
		SLOT( removeIngredient( int ) ) );
}


int KreIngredientListWidget::elementCount()
{
	return m_database->ingredientCount();
}


void KreIngredientListWidget::load( int limit, int offset )
{
	ElementList ingredientList;
	int numberOfIngredients = m_database->loadIngredients( &ingredientList, limit, offset );
	m_sourceModel->setRowCount( numberOfIngredients );

	int current_row = 0;
	QModelIndex index;
	for ( ElementList::const_iterator it = ingredientList.constBegin(); 
	it != ingredientList.constEnd(); ++it ) {
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


void KreIngredientListWidget::cancelLoad()
{
	//We do nothing here because we are not using threads.
}


int KreIngredientListWidget::idColumn()
{
 	//This function must return the column number where the id is stored.
	return 0;
}


void KreIngredientListWidget::createIngredient( const Element &ing )
{
	//FIXME: This could be more efficient
	Q_UNUSED(ing)
	reload( ForceReload );
}


void KreIngredientListWidget::removeIngredient( int id )
{
	//FIXME: This could be more efficient
	Q_UNUSED(id)
	reload( ForceReload );
}

