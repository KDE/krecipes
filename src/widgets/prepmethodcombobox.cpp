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

#include <qlistbox.h>

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
		kdDebug()<<"original makeCompletion( "<<string<<" )"<<endl;

		int comma_index = string.findRev(",");
		QString completion_txt = string;
		if ( comma_index != -1 )
			completion_txt = completion_txt.right( completion_txt.length() - comma_index - 1 ).stripWhiteSpace();
		if ( completion_txt.isEmpty() )
			return string;

		kdDebug()<<"altered makeCompletion( "<<completion_txt<<" )"<<endl;

		completion_txt = KCompletion::makeCompletion(completion_txt);
		kdDebug()<<"got: "<<completion_txt<<endl;

		if ( completion_txt.isEmpty() )
			completion_txt = string;
		else if ( comma_index != -1 )
			completion_txt = string.left( comma_index ) + "," + completion_txt;

		kdDebug()<<"returning: "<<completion_txt<<endl;
		return completion_txt;
	}
};

PrepMethodComboBox::PrepMethodComboBox( bool b, QWidget *parent, RecipeDB *db ) : KComboBox( b, parent ),
		database( db )
{
	setAutoDeleteCompletionObject(true);
	setCompletionObject(new PrepMethodCompletion());
}

void PrepMethodComboBox::reload()
{
	QString remember_text = lineEdit()->text();

	ElementList prepMethodList;
	database->loadPrepMethods( &prepMethodList );

	clear();
	prepMethodComboRows.clear();

	int row = 0;
	for ( ElementList::const_iterator it = prepMethodList.begin(); it != prepMethodList.end(); ++it, ++row ) {
		insertItem((*it).name);
		completionObject()->addItem((*it).name);
		prepMethodComboRows.insert( row,(*it).id );
	}

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
	kdDebug()<<"Warning: couldn't find the ID for "<<ing<<endl;
	return -1;
}

void PrepMethodComboBox::createPrepMethod( const Element &element )
{
	int row = findInsertionPoint( element.name );

	QString remember_text = lineEdit()->text();

	insertItem( element.name, row );
	completionObject()->addItem(element.name);

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

#include "prepmethodcombobox.moc"
