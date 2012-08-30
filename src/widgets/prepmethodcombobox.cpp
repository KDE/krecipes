/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "prepmethodcombobox.h"

#include <QLineEdit>

#include <kdebug.h>

#include "backends/recipedb.h"
#include "datablocks/elementlist.h"

/** Completion object which allows completing completing items
  * the last item in a comma-separated list
  */
class PrepMethodCompletion : public KCompletion
{
public:
	PrepMethodCompletion() : KCompletion()
	{}

	virtual QString makeCompletion( const QString &string ) {
		kDebug()<<"original makeCompletion( "<<string<<" )";

		int comma_index = string.lastIndexOf(",");
		QString completion_txt = string;
		if ( comma_index != -1 )
			completion_txt = completion_txt.right( completion_txt.length() - comma_index - 1 ).trimmed();
		if ( completion_txt.isEmpty() )
			return string;

		kDebug()<<"altered makeCompletion( "<<completion_txt<<" )";

		completion_txt = KCompletion::makeCompletion(completion_txt);
		kDebug()<<"got: "<<completion_txt;

		if ( completion_txt.isEmpty() )
			completion_txt = string;
		else if ( comma_index != -1 )
			completion_txt = string.left( comma_index ) + ',' + completion_txt;

		kDebug()<<"returning: "<<completion_txt;
		return completion_txt;
	}
};

PrepMethodComboBox::PrepMethodComboBox( bool b, QWidget *parent, RecipeDB *db, const QString &specialItem ):
	KComboBox( b, parent ), database( db ), m_specialItem(specialItem)
{
	setAutoDeleteCompletionObject(true);
	setCompletionObject(new PrepMethodCompletion());
}

void PrepMethodComboBox::reload()
{
	QString remember_text;
	if ( isEditable() )
		remember_text = lineEdit()->text();

	ElementList prepMethodList;
	database->loadPrepMethods( &prepMethodList );

	clear();
	prepMethodComboRows.clear();

	int row = 0;
	if ( !m_specialItem.isEmpty() ) {
		insertItem( count(), m_specialItem );
		prepMethodComboRows.insert( row, -1 );
		row++;
	}
	for ( ElementList::const_iterator it = prepMethodList.constBegin(); it != prepMethodList.constEnd(); ++it, ++row ) {
		insertItem( count(), (*it).name );
		completionObject()->addItem((*it).name);
		prepMethodComboRows.insert( row,(*it).id );
	}

	if ( isEditable() )
		lineEdit()->setText( remember_text );

	database->disconnect( this );
	connect( database, SIGNAL( prepMethodCreated( const Element & ) ), SLOT( createPrepMethod( const Element & ) ) );
	connect( database, SIGNAL( prepMethodRemoved( int ) ), SLOT( removePrepMethod( int ) ) );
}

int PrepMethodComboBox::id( int row )
{
	return prepMethodComboRows[ row ];
}

int PrepMethodComboBox::id( const QString &ing )
{
	for ( int i = 0; i < count(); i++ ) {
		if ( ing == itemText( i ) )
			return id(i);
	}
	kDebug()<<"Warning: couldn't find the ID for "<<ing;
	return -1;
}

void PrepMethodComboBox::createPrepMethod( const Element &element )
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
	for ( QMap<int, int>::iterator it = prepMethodComboRows.begin(); it != prepMethodComboRows.end(); ++it ) {
		if ( it.key() >= row ) {
			new_map.insert( it.key() + 1, it.value() );
		}
		else
			new_map.insert( it.key(), it.value() );
	}
	prepMethodComboRows = new_map;
	prepMethodComboRows.insert( row, element.id );
}

void PrepMethodComboBox::removePrepMethod( int id )
{
	int row = -1;
	for ( QMap<int, int>::iterator it = prepMethodComboRows.begin(); it != prepMethodComboRows.end(); ++it ) {
		if ( it.value() == id ) {
			row = it.key();
			completionObject()->removeItem( itemText(row) );
			removeItem( row );
			prepMethodComboRows.erase( it );
			break;
		}
	}

	if ( row == -1 )
		return ;

	//now update the map by pushing everything after this item up
	QMap<int, int> new_map;
	for ( QMap<int, int>::iterator it = prepMethodComboRows.begin(); it != prepMethodComboRows.end(); ++it ) {
		if ( it.key() > row ) {
			new_map.insert( it.key() - 1, it.value() );
		}
		else
			new_map.insert( it.key(), it.value() );
	}
	prepMethodComboRows = new_map;
}

int PrepMethodComboBox::findInsertionPoint( const QString &name )
{
	for ( int i = 0; i < count(); i++ ) {
		if ( QString::localeAwareCompare( name, itemText( i ) ) < 0 )
			return i;
	}

	return count();
}

void PrepMethodComboBox::setSelected( int prepID )
{
	//do a reverse lookup on the row->id map
	QMap<int, int>::const_iterator it;
	for ( it = prepMethodComboRows.constBegin(); it != prepMethodComboRows.constEnd(); ++it ) {
		if ( it.value() == prepID ) {
			setCurrentIndex(it.key());
			break;
		}
	}
}

#include "prepmethodcombobox.moc"
