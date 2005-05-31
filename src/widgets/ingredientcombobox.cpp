/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "ingredientcombobox.h"

#include <qlistbox.h>

#include <kdebug.h>

#include "backends/recipedb.h"
#include "datablocks/elementlist.h"

IngredientComboBox::IngredientComboBox( bool b, QWidget *parent, RecipeDB *db ) : KComboBox( b, parent ),
		database( db )
{
}

void IngredientComboBox::reload()
{
	QString remember_text = currentText();

	ElementList ingredientList;
	database->loadIngredients( &ingredientList );

	clear();
	ingredientComboRows.clear();

	int row = 0;
	for ( ElementList::const_iterator it = ingredientList.begin(); it != ingredientList.end(); ++it, ++row ) {
		insertItem((*it).name);
		completionObject()->addItem((*it).name);
		ingredientComboRows.insert( row, (*it).id );
	}

	if ( listBox()->findItem( remember_text, Qt::ExactMatch ) ) {
		setCurrentText( remember_text );
	}

	connect( database, SIGNAL( ingredientCreated( const Element & ) ), SLOT( createIngredient( const Element & ) ) );
	connect( database, SIGNAL( ingredientRemoved( int ) ), SLOT( removeIngredient( int ) ) );
}

int IngredientComboBox::id( int row )
{
	return ingredientComboRows[ row ];
}

int IngredientComboBox::id( const QString &ing )
{
	for ( int i = 0; i < count(); i++ ) {
		if ( ing == text( i ) )
			return id(i);
	}
	kdDebug()<<"Warning: couldn't find the ID for "<<ing<<endl;
	return -1;
}

void IngredientComboBox::createIngredient( const Element &element )
{
	int row = findInsertionPoint( element.name );

	insertItem( element.name, row );
	completionObject()->addItem(element.name);

	//now update the map by pushing everything after this item down
	QMap<int, int> new_map;
	for ( QMap<int, int>::iterator it = ingredientComboRows.begin(); it != ingredientComboRows.end(); ++it ) {
		if ( it.key() >= row ) {
			new_map.insert( it.key() + 1, it.data() );
		}
		else
			new_map.insert( it.key(), it.data() );
	}
	ingredientComboRows = new_map;
	ingredientComboRows.insert( row, element.id );
}

void IngredientComboBox::removeIngredient( int id )
{
	int row = -1;
	for ( QMap<int, int>::iterator it = ingredientComboRows.begin(); it != ingredientComboRows.end(); ++it ) {
		if ( it.data() == id ) {
			row = it.key();
			completionObject()->removeItem( text(row) );
			removeItem( row );
			ingredientComboRows.remove( it );
			break;
		}
	}

	if ( row == -1 )
		return ;

	//now update the map by pushing everything after this item up
	QMap<int, int> new_map;
	for ( QMap<int, int>::iterator it = ingredientComboRows.begin(); it != ingredientComboRows.end(); ++it ) {
		if ( it.key() > row ) {
			new_map.insert( it.key() - 1, it.data() );
		}
		else
			new_map.insert( it.key(), it.data() );
	}
	ingredientComboRows = new_map;
}

int IngredientComboBox::findInsertionPoint( const QString &name )
{
	for ( int i = 0; i < count(); i++ ) {
		if ( QString::localeAwareCompare( name, text( i ) ) < 0 )
			return i;
	}

	return count();
}

#include "ingredientcombobox.moc"
