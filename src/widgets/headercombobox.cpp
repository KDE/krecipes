/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2010 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "headercombobox.h"

#include <QLineEdit>

#include "backends/recipedb.h"
#include "datablocks/elementlist.h"


HeaderComboBox::HeaderComboBox( bool b, QWidget *parent, RecipeDB *db ) : KComboBox( b, parent ),
		database( db )
{
	connect( database, SIGNAL( ingGroupCreated(const Element &) ),
		this, SLOT( createHeader(const Element &) ) );
	connect( database, SIGNAL( ingGroupRemoved(int) ),
		this, SLOT( removeHeader(int) ) );
}

void HeaderComboBox::reload()
{
	QString remember_text = currentText();

	ElementList headerList;
	database->loadIngredientGroups( &headerList );

	clear();

	for ( ElementList::const_iterator it = headerList.constBegin(); it != headerList.constEnd(); ++it ) {
		addItem( it->name, it->id );
		completionObject()->addItem((*it).name);
	}

	if ( findText( remember_text, Qt::MatchExactly ) && isEditable()) {
		setEditText( remember_text );
	}
}

void HeaderComboBox::createHeader( const Element & element)
{
	int row = findInsertionPoint( element.name );

	QString remember_text;
	if ( isEditable() )
		remember_text = lineEdit()->text();

	insertItem( row, element.name, element.id );
	completionObject()->addItem(element.name);

	if ( isEditable() )
		lineEdit()->setText( remember_text );
}

void HeaderComboBox::removeHeader( int id )
{
	int row = findData( id );
	completionObject()->removeItem( itemText(row) );
	removeItem( row );
}

int HeaderComboBox::findInsertionPoint( const QString &name )
{
	int c = count();
	for ( int i = 0; i < c; i++ ) {
		if ( QString::localeAwareCompare( name, itemText( i ) ) < 0 )
			return i;
	}

	return c;
}
#include "headercombobox.moc"
