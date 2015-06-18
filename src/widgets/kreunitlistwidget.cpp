/***************************************************************************
*   Copyright © 2014-2015 José Manuel Santamaría Lema <panfaust@gmail.com> *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
***************************************************************************/

#include "kreunitlistwidget.h"

#include "ui_kregenericlistwidget.h"
#include "backends/recipedb.h"

//#include <kdebug.h>

#include <KLocale>
#include <KComboBox>
#include <QStandardItemModel>

#include "widgets/unittypedelegate.h"


KreUnitListWidget::KreUnitListWidget( QWidget *parent, RecipeDB *db ) : KreGenericListWidget( parent, db )
{
        //The horizontal column labels.
        QStringList horizontalLabels;
        horizontalLabels << i18nc( "@title:column", "Id" )
		<< i18nc( "@title:column Unit name", "Unit" )
		<< i18nc( "@title:column Unit abbreviation", "Abbreviation" )
		<< i18nc( "@title:column Unit plural", "Plural" )
		<< i18nc( "@title:column unit plural abbreviation", "Abbreviation" )
		<< i18nc( "@title:column Unit type", "Type" );
        m_sourceModel->setHorizontalHeaderLabels( horizontalLabels );

	//Connect signals and slots.
	connect( m_database, SIGNAL( unitCreated( const Unit & ) ),
		this, SLOT( createUnit( const Unit & ) ) );
	connect( m_database, SIGNAL( unitRemoved( int ) ),
		this, SLOT( removeUnit( int ) ) );
}

void KreUnitListWidget::load( int limit, int offset )
{
	UnitList unitList;
	int numberOfUnits = m_database->loadUnits( &unitList, Unit::All, limit, offset );
	m_sourceModel->setRowCount( numberOfUnits );

	UnitTypeDelegate * unitTypeDelegate = new UnitTypeDelegate;
	ui->m_treeView->setItemDelegateForColumn(5, unitTypeDelegate);

	int current_row = 0;
	QModelIndex index;
	for ( UnitList::const_iterator it = unitList.constBegin(); it != unitList.constEnd(); ++it ) {
		// Write the database id in the model.
		index = m_sourceModel->index( current_row, 0 );
		m_sourceModel->setData( index, QVariant(it->id()), Qt::EditRole );
		m_sourceModel->itemFromIndex( index )->setEditable( false );
		// Write the name of the unit (singular) in the model.
		index = m_sourceModel->index( current_row, 1 );
		m_sourceModel->setData( index, QVariant(it->name()), Qt::EditRole );
		m_sourceModel->itemFromIndex( index )->setEditable( true );
		// Write the singular abbreviation in the model.
		index = m_sourceModel->index( current_row, 2 );
		m_sourceModel->setData( index, QVariant(it->nameAbbrev()), Qt::EditRole );
		m_sourceModel->itemFromIndex( index )->setEditable( true );
		// Write the name of the unit (plural) in the model.
		index = m_sourceModel->index( current_row, 3 );
		m_sourceModel->setData( index, QVariant(it->plural()), Qt::EditRole );
		m_sourceModel->itemFromIndex( index )->setEditable( true );
		// Write the name of the plural abbreviation in the model.
		index = m_sourceModel->index( current_row, 4 );
		m_sourceModel->setData( index, QVariant(it->pluralAbbrev()), Qt::EditRole );
		m_sourceModel->itemFromIndex( index )->setEditable( true );
		// Set the unit Type in the model.
		index = m_sourceModel->index( current_row, 5 );
		m_sourceModel->setData( index, QVariant(it->type()), Qt::UserRole );
		m_sourceModel->setData( index, QVariant(Unit::typeToString(it->type())), Qt::EditRole );
		m_sourceModel->itemFromIndex( index )->setEditable( true );
		// Increment the row counter.
		++current_row;
	}
	emit loadFinishedPrivate();
}


void KreUnitListWidget::createUnit( const Unit &unit )
{
	//FIXME: This could be more efficient.
	Q_UNUSED(unit)
	reload( ForceReload );
}

void KreUnitListWidget::removeUnit( int id )
{
	//FIXME: This could be more efficient.
	Q_UNUSED(id)
	reload( ForceReload );
}

int KreUnitListWidget::elementCount()
{
	return m_database->unitCount();
}

void KreUnitListWidget::cancelLoad()
{
	//We do nothing here beacuse we are not using threads. 
}

int KreUnitListWidget::idColumn()
{
	return 0;
}
