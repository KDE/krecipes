/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "ingredientcombobox.h"

#include <QTimer>
#include <QLineEdit>

#include <kdebug.h>
#include <kapplication.h>
#include <kglobal.h>
#include <kconfiggroup.h>

#include "backends/recipedb.h"
#include "datablocks/elementlist.h"

IngredientComboBox::IngredientComboBox( bool b, QWidget *parent, RecipeDB *db, const QString &specialItem ) : KComboBox( b, parent ),
		database( db ), loading_at(0), load_timer(new QTimer(this)), m_specialItem(specialItem)
{
	connect( load_timer, SIGNAL(timeout()), SLOT(loadMore()) );
	completionObject()->setIgnoreCase(true);
}

void IngredientComboBox::reload()
{
	QString remember_text;
	if ( isEditable() )
		remember_text = lineEdit()->text();

	ElementList ingredientList;
	database->loadIngredients( &ingredientList );

	clear();
	ingredientComboRows.clear();

	int row = 0;
	if ( !m_specialItem.isEmpty() ) {
		insertItem( count(), m_specialItem );
		ingredientComboRows.insert( row, -1 );
		row++;
	}
	for ( ElementList::const_iterator it = ingredientList.constBegin(); it != ingredientList.constEnd(); ++it, ++row ) {
		insertItem( count(), (*it).name );
		completionObject()->addItem((*it).name);
		ingredientComboRows.insert( row, (*it).id );
	}

	if ( isEditable() )
		setEditText( remember_text );

	database->disconnect( this );
	connect( database, SIGNAL( ingredientCreated( const Element & ) ), SLOT( createIngredient( const Element & ) ) );
	connect( database, SIGNAL( ingredientRemoved( int ) ), SLOT( removeIngredient( int ) ) );
}

void IngredientComboBox::loadMore()
{
	if ( loading_at >= ing_count-1 ) {
		endLoad();
		return;
	}

	ElementList ingredientList;
	database->loadIngredients( &ingredientList, load_limit, loading_at );

	for ( ElementList::const_iterator it = ingredientList.constBegin(); it != ingredientList.constEnd(); ++it, ++loading_at ) {
		insertItem( count(), (*it).name );
		completionObject()->addItem((*it).name);
		ingredientComboRows.insert( loading_at, (*it).id );
	}
}

void IngredientComboBox::startLoad()
{
	//don't receive ingredient created/removed events from the database
	database->disconnect( this );

	KConfigGroup config = KGlobal::config()->group( "Performance" );
	load_limit = config.readEntry( "Limit", -1 );
	if ( load_limit == -1 ) {
		reload();
		endLoad();
	}
	else {
		loading_at = 0;
		ing_count = database->ingredientCount();

		load_timer->start( 0 );
	}
}

void IngredientComboBox::endLoad()
{
	load_timer->stop();

	//now we're ready to receive ingredient created/removed events from the database
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
		if ( ing == itemText( i ) )
			return id(i);
	}
	kDebug()<<"Warning: couldn't find the ID for "<<ing;
	return -1;
}

void IngredientComboBox::createIngredient( const Element &element )
{
	int row = findInsertionPoint( element.name );

	QString remember_text;
	if ( isEditable() )
		remember_text = lineEdit()->text();

	insertItem( row, element.name );
	completionObject()->addItem(element.name);

	if ( isEditable() )
		lineEdit()->setText( remember_text );

	//now update the map by pushing everything after this item down
	QMap<int, int> new_map;
	for ( QMap<int, int>::iterator it = ingredientComboRows.begin(); it != ingredientComboRows.end(); ++it ) {
		if ( it.key() >= row ) {
			new_map.insert( it.key() + 1, it.value() );
		}
		else
			new_map.insert( it.key(), it.value() );
	}
	ingredientComboRows = new_map;
	ingredientComboRows.insert( row, element.id );
}

void IngredientComboBox::removeIngredient( int id )
{
	int row = -1;
	for ( QMap<int, int>::iterator it = ingredientComboRows.begin(); it != ingredientComboRows.end(); ++it ) {
		if ( it.value() == id ) {
			row = it.key();
			completionObject()->removeItem( itemText(row) );
			removeItem( row );
			ingredientComboRows.erase( it );
			break;
		}
	}

	if ( row == -1 )
		return ;

	//now update the map by pushing everything after this item up
	QMap<int, int> new_map;
	for ( QMap<int, int>::iterator it = ingredientComboRows.begin(); it != ingredientComboRows.end(); ++it ) {
		if ( it.key() > row ) {
			new_map.insert( it.key() - 1, it.value() );
		}
		else
			new_map.insert( it.key(), it.value() );
	}
	ingredientComboRows = new_map;
}

int IngredientComboBox::findInsertionPoint( const QString &name )
{
	for ( int i = 0; i < count(); i++ ) {
		if ( QString::localeAwareCompare( name, itemText( i ) ) < 0 )
			return i;
	}

	return count();
}

#include "ingredientcombobox.moc"
