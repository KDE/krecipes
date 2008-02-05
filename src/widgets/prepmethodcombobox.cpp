/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "prepmethodcombobox.h"

#include <q3listbox.h>
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
		kDebug()<<"original makeCompletion( "<<string<<" )"<<endl;

		int comma_index = string.findRev(",");
		QString completion_txt = string;
		if ( comma_index != -1 )
			completion_txt = completion_txt.right( completion_txt.length() - comma_index - 1 ).trimmed();
		if ( completion_txt.isEmpty() )
			return string;

		kDebug()<<"altered makeCompletion( "<<completion_txt<<" )"<<endl;

		completion_txt = KCompletion::makeCompletion(completion_txt);
		kDebug()<<"got: "<<completion_txt<<endl;

		if ( completion_txt.isEmpty() )
			completion_txt = string;
		else if ( comma_index != -1 )
			completion_txt = string.left( comma_index ) + "," + completion_txt;

		kDebug()<<"returning: "<<completion_txt<<endl;
		return completion_txt;
	}
};

PrepMethodComboBox::PrepMethodComboBox( bool b, QWidget *parent, RecipeDB *db, const QString &specialItem ) : 
  KComboBox( b, parent ),
  database( db ), m_specialItem(specialItem)
{
	setAutoDeleteCompletionObject(true);
	setCompletionObject(new PrepMethodCompletion());
}

void PrepMethodComboBox::reload()
{
	QString remember_text;
	if ( editable() )
		remember_text = lineEdit()->text();

	ElementList prepMethodList;
	database->loadPrepMethods( &prepMethodList );

	clear();
	prepMethodComboRows.clear();

	int row = 0;
	if ( !m_specialItem.isNull() ) {
		insertItem(m_specialItem);
		prepMethodComboRows.insert( row, -1 );
		row++;
	}
	for ( ElementList::const_iterator it = prepMethodList.begin(); it != prepMethodList.end(); ++it, ++row ) {
		insertItem((*it).name);
		completionObject()->addItem((*it).name);
		prepMethodComboRows.insert( row,(*it).id );
	}

	if ( editable() )
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
		if ( ing == text( i ) )
			return id(i);
	}
	kDebug()<<"Warning: couldn't find the ID for "<<ing<<endl;
	return -1;
}

void PrepMethodComboBox::createPrepMethod( const Element &element )
{
	int row = findInsertionPoint( element.name );

	QString remember_text;
	if ( editable() )
		remember_text = lineEdit()->text();

	insertItem( element.name, row );
	completionObject()->addItem(element.name);

	if ( editable() )
		lineEdit()->setText( remember_text );

	//now update the map by pushing everything after this item down
	QMap<int, int> new_map;
	for ( QMap<int, int>::iterator it = prepMethodComboRows.begin(); it != prepMethodComboRows.end(); ++it ) {
		if ( it.key() >= row ) {
			new_map.insert( it.key() + 1, it.data() );
		}
		else
			new_map.insert( it.key(), it.data() );
	}
	prepMethodComboRows = new_map;
	prepMethodComboRows.insert( row, element.id );
}

void PrepMethodComboBox::removePrepMethod( int id )
{
	int row = -1;
	for ( QMap<int, int>::iterator it = prepMethodComboRows.begin(); it != prepMethodComboRows.end(); ++it ) {
		if ( it.data() == id ) {
			row = it.key();
			completionObject()->removeItem( text(row) );
			removeItem( row );
			prepMethodComboRows.remove( it );
			break;
		}
	}

	if ( row == -1 )
		return ;

	//now update the map by pushing everything after this item up
	QMap<int, int> new_map;
	for ( QMap<int, int>::iterator it = prepMethodComboRows.begin(); it != prepMethodComboRows.end(); ++it ) {
		if ( it.key() > row ) {
			new_map.insert( it.key() - 1, it.data() );
		}
		else
			new_map.insert( it.key(), it.data() );
	}
	prepMethodComboRows = new_map;
}

int PrepMethodComboBox::findInsertionPoint( const QString &name )
{
	for ( int i = 0; i < count(); i++ ) {
		if ( QString::localeAwareCompare( name, text( i ) ) < 0 )
			return i;
	}

	return count();
}

void PrepMethodComboBox::setSelected( int prepID )
{
	//do a reverse lookup on the row->id map
	QMap<int, int>::const_iterator it;
	for ( it = prepMethodComboRows.begin(); it != prepMethodComboRows.end(); ++it ) {
		if ( it.data() == prepID ) {
			setCurrentIndex(it.key());
			break;
		}
	}
}

#include "prepmethodcombobox.moc"
