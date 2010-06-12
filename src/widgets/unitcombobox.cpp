/***************************************************************************
*   Copyright © 2006 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "unitcombobox.h"

#include <q3listbox.h>

#include <klocale.h>
#include <kconfig.h>
#include <kglobal.h>

#include "backends/recipedb.h"
#include "datablocks/elementlist.h"

UnitComboBox::UnitComboBox( QWidget *parent, RecipeDB *db, Unit::Type type ) : KComboBox( parent ),
		database( db ), m_type(type)
{
	connect( database, SIGNAL( unitCreated( const Unit & ) ), SLOT( createUnit( const Unit & ) ) );
	connect( database, SIGNAL( unitRemoved( int ) ), SLOT( removeUnit( int ) ) );
}

void UnitComboBox::popup()
{
	if ( count() == 1 )
		reload();
	KComboBox::showPopup();
}

Unit UnitComboBox::unit() const
{
	Unit u;
	u.setName(currentText());
	u.setId(id(currentIndex()));
	return u;
}

void UnitComboBox::reload()
{
	QString remember_filter = currentText();

	UnitList unitList;
	database->loadUnits( &unitList, m_type );

	clear();
	unitComboRows.clear();

	//Now load the categories
	loadUnits(unitList);

	if ( findText( remember_filter, Qt::MatchExactly ) ) {
		setEditText( remember_filter );
	}
}

void UnitComboBox::loadUnits( const UnitList &unitList )
{
	int row = 0;
	for ( UnitList::const_iterator it = unitList.begin(); it != unitList.end(); ++it ) {
		insertItem( count(), (*it).name() );
		unitComboRows.insert( row, (*it).id() ); // store unit id's in the combobox position to obtain the unit id later
		row++;
	}
}

void UnitComboBox::setSelected( int unitID )
{
	//do a reverse lookup on the row->id map
	QMap<int, int>::const_iterator it;
	for ( it = unitComboRows.constBegin(); it != unitComboRows.constEnd(); ++it ) {
		if ( it.value() == unitID ) {
			//KDE4 port  setCurrentItem(it.key());
			setCurrentIndex(it.key());
			break;
		}
	}
}

int UnitComboBox::id( int row ) const
{
	return unitComboRows[ row ];
}

void UnitComboBox::createUnit( const Unit &element )
{
	int row = findInsertionPoint( element.name() );

	insertItem( row, element.name() );

	//now update the map by pushing everything after this item down
	QMap<int, int> new_map;
	for ( QMap<int, int>::iterator it = unitComboRows.begin(); it != unitComboRows.end(); ++it ) {
		if ( it.key() >= row ) {
			new_map.insert( it.key() + 1, it.value() );
		}
		else
			new_map.insert( it.key(), it.value() );
	}
	unitComboRows = new_map;
	unitComboRows.insert( row, element.id() );
}

void UnitComboBox::removeUnit( int id )
{
	int row = -1;
	for ( QMap<int, int>::iterator it = unitComboRows.begin(); it != unitComboRows.end(); ++it ) {
		if ( it.value() == id ) {
			row = it.key();
			removeItem( row );
			unitComboRows.erase( it );
			break;
		}
	}

	if ( row == -1 )
		return ;

	//now update the map by pushing everything after this item up
	QMap<int, int> new_map;
	for ( QMap<int, int>::iterator it = unitComboRows.begin(); it != unitComboRows.end(); ++it ) {
		if ( it.key() > row ) {
			new_map.insert( it.key() - 1, it.value() );
		}
		else
			new_map.insert( it.key(), it.value() );
	}
	unitComboRows = new_map;
}

int UnitComboBox::findInsertionPoint( const QString &name )
{
	for ( int i = 1; i < count(); i++ ) {
		if ( QString::localeAwareCompare( name, itemText( i ) ) < 0 )
			return i;
	}

	return count();
}

#include "unitcombobox.moc"
