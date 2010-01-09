/***************************************************************************
*   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2004 Cyril Bosselut <bosselut@b1project.com>         *
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "categorycombobox.h"

#include <q3listbox.h>

#include <klocale.h>
#include <kconfiggroup.h>
#include <kglobal.h>

#include "backends/recipedb.h"
#include "backends/progressinterface.h"
#include "datablocks/elementlist.h"
#include "datablocks/categorytree.h"

CategoryComboBox::CategoryComboBox( QWidget *parent, RecipeDB *db ) : KComboBox( parent ),
		database( db ),
		m_offset(0)
{
	connect( database, SIGNAL( categoryCreated( const Element &, int ) ), SLOT( createCategory( const Element &, int ) ) );
	connect( database, SIGNAL( categoryRemoved( int ) ), SLOT( removeCategory( int ) ) );
	connect( database, SIGNAL( categoryModified( const Element & ) ), SLOT( modifyCategory( const Element & ) ) );
	connect( database, SIGNAL( categoriesMerged( int, int ) ), SLOT( mergeCategories( int, int ) ) );

	// Insert default "All Categories" (row 0, which will be translated to -1 as category in the filtering process)
	// the rest of the items are loaded when needed in order to significantly speed up startup
	insertItem( count(), i18n( "All Categories" ) );
}

void CategoryComboBox::popup()
{
	if ( count() == 1 )
		reload();
	KComboBox::showPopup();
}

void CategoryComboBox::reload()
{
	QString remember_cat_filter = currentText();

	KConfigGroup config = KGlobal::config()->group( "Performance" );
	int limit = config.readEntry( "CategoryLimit", -1 );

	//ProgressInterface pi(this);
	//pi.listenOn(database);

	CategoryTree categoryList;
	database->loadCategories( &categoryList, limit, m_offset, -1 );

	clear();
	categoryComboRows.clear();

	// Insert default "All Categories" (row 0, which will be translated to -1 as category in the filtering process)
	insertItem( count(), i18n( "All Categories" ) );

	//Now load the categories
	int row = 1;
	loadCategories(&categoryList,row);

	if ( findText( remember_cat_filter, Qt::MatchExactly ) && isEditable() ) {
		setEditText( remember_cat_filter );
	}
}

void CategoryComboBox::loadCategories( CategoryTree *categoryTree, int &row )
{
	for ( CategoryTree * child_it = categoryTree->firstChild(); child_it; child_it = child_it->nextSibling() ) {
		insertItem( count(), child_it->category.name );
		categoryComboRows.insert( row, child_it->category.id ); // store category id's in the combobox position to obtain the category id later
		row++;
		loadCategories( child_it, row );
	}
}

void CategoryComboBox::loadNextGroup()
{
	KConfigGroup config = KGlobal::config()->group( "Performance" );
	int limit = config.readEntry( "CategoryLimit", -1 );

	m_offset += limit;

	reload();
}

void CategoryComboBox::loadPrevGroup()
{
	KConfigGroup config = KGlobal::config()->group( "Performance" );
	int limit = config.readEntry( "CategoryLimit", -1 );

	m_offset -= limit;

	reload();
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

	insertItem( row, element.name );

	//now update the map by pushing everything after this item down
	QMap<int, int> new_map;
	for ( QMap<int, int>::iterator it = categoryComboRows.begin(); it != categoryComboRows.end(); ++it ) {
		if ( it.key() >= row ) {
			new_map.insert( it.key() + 1, it.value() );
		}
		else
			new_map.insert( it.key(), it.value() );
	}
	categoryComboRows = new_map;
	categoryComboRows.insert( row, element.id );
}

void CategoryComboBox::removeCategory( int id )
{
	int row = -1;
	for ( QMap<int, int>::iterator it = categoryComboRows.begin(); it != categoryComboRows.end(); ++it ) {
		if ( it.value() == id ) {
			row = it.key();
			removeItem( row );
			categoryComboRows.erase( it );
			break;
		}
	}

	if ( row == -1 )
		return ;

	//now update the map by pushing everything after this item up
	QMap<int, int> new_map;
	for ( QMap<int, int>::iterator it = categoryComboRows.begin(); it != categoryComboRows.end(); ++it ) {
		if ( it.key() > row ) {
			new_map.insert( it.key() - 1, it.value() );
		}
		else
			new_map.insert( it.key(), it.value() );
	}
	categoryComboRows = new_map;
}

void CategoryComboBox::modifyCategory( const Element &element )
{
	for ( QMap<int, int>::const_iterator it = categoryComboRows.constBegin(); it != categoryComboRows.constEnd(); ++it ) {
		if ( it.value() == element.id )
			setItemText( it.key(), element.name );
	}
}

void CategoryComboBox::mergeCategories( int /*to_id*/, int from_id )
{
	removeCategory( from_id );
}

int CategoryComboBox::findInsertionPoint( const QString &name )
{
	for ( int i = 1; i < count(); i++ ) {
		if ( QString::localeAwareCompare( name, itemText( i ) ) < 0 )
			return i;
	}

	return count();
}

#include "categorycombobox.moc"
