/***************************************************************************
*   Copyright (C) 2003-2004 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "categorycombobox.h"

#include <qlistbox.h>

#include <klocale.h>
#include <kconfig.h>
#include <kglobal.h>

#include "DBBackend/recipedb.h"
#include "elementlist.h"

CategoryComboBox::CategoryComboBox( QWidget *parent, RecipeDB *db ) : KComboBox( parent ),
		database( db )
{
	connect( database, SIGNAL( categoryCreated( const Element &, int ) ), SLOT( createCategory( const Element &, int ) ) );
	connect( database, SIGNAL( categoryRemoved( int ) ), SLOT( removeCategory( int ) ) );
	connect( database, SIGNAL( categoryModified( const Element & ) ), SLOT( modifyCategory( const Element & ) ) );
	connect( database, SIGNAL( categoriesMerged( int, int ) ), SLOT( mergeCategories( int, int ) ) );

	reload();
}

void CategoryComboBox::reload()
{
	QString remember_cat_filter = currentText();

	KConfig * config = KGlobal::config();config->setGroup( "Advanced" );
	int limit = config->readNumEntry( "Limit", -1 );
	ElementList categoryList;
	database->loadCategories( &categoryList, limit, 0 );

	clear();
	categoryComboRows.clear();

	// Insert default "All Categories" (row 0, which will be translated to -1 as category in the filtering process)
	insertItem( i18n( "All Categories" ) );

	//Now load the categories
	int row = 1;
	for ( ElementList::const_iterator cat_it = categoryList.begin(); cat_it != categoryList.end(); ++cat_it ) {
		insertItem( ( *cat_it ).name );
		categoryComboRows.insert( row, ( *cat_it ).id ); // store category id's in the combobox position to obtain the category id later
		row++;
	}

	if ( listBox() ->findItem( remember_cat_filter, Qt::ExactMatch ) ) {
		setCurrentText( remember_cat_filter );
	}
}

int CategoryComboBox::id( int row )
{
	if ( row )
		return categoryComboRows[ row ];
	else
		return -1; // No category filtering
}

void CategoryComboBox::createCategory( const Element &element, int /*parent_id*/ )
{
	int row = findInsertionPoint( element.name );

	insertItem( element.name, row );

	//now update the map by pushing everything after this item down
	QMap<int, int> new_map;
	for ( QMap<int, int>::iterator it = categoryComboRows.begin(); it != categoryComboRows.end(); ++it ) {
		if ( it.key() >= row ) {
			new_map.insert( it.key() + 1, it.data() );
		}
		else
			new_map.insert( it.key(), it.data() );
	}
	categoryComboRows = new_map;
	categoryComboRows.insert( row, element.id );
}

void CategoryComboBox::removeCategory( int id )
{
	int row = -1;
	for ( QMap<int, int>::iterator it = categoryComboRows.begin(); it != categoryComboRows.end(); ++it ) {
		if ( it.data() == id ) {
			row = it.key();
			removeItem( row );
			categoryComboRows.remove( it );
			break;
		}
	}

	if ( row == -1 )
		return ;

	//now update the map by pushing everything after this item up
	QMap<int, int> new_map;
	for ( QMap<int, int>::iterator it = categoryComboRows.begin(); it != categoryComboRows.end(); ++it ) {
		if ( it.key() > row ) {
			new_map.insert( it.key() - 1, it.data() );
		}
		else
			new_map.insert( it.key(), it.data() );
	}
	categoryComboRows = new_map;
}

void CategoryComboBox::modifyCategory( const Element &element )
{
	for ( QMap<int, int>::const_iterator it = categoryComboRows.begin(); it != categoryComboRows.end(); ++it ) {
		if ( it.data() == element.id )
			changeItem( element.name, it.key() );
	}
}

void CategoryComboBox::mergeCategories( int /*to_id*/, int from_id )
{
	removeCategory( from_id );
}

int CategoryComboBox::findInsertionPoint( const QString &name )
{
	for ( int i = 1; i < count(); i++ ) {
		if ( QString::localeAwareCompare( name, text( i ) ) < 0 )
			return i;
	}

	return count();
}

#include "categorycombobox.moc"
