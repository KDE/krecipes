/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "headercombobox.h"

#include <qlistbox.h>

#include "backends/recipedb.h"
#include "datablocks/elementlist.h"

HeaderComboBox::HeaderComboBox( bool b, QWidget *parent, RecipeDB *db ) : KComboBox( b, parent ),
		database( db )
{
}

void HeaderComboBox::reload()
{
	QString remember_text = currentText();

	ElementList headerList;
	database->loadIngredientGroups( &headerList );

	clear();

	QValueList<QString> used;
	for ( ElementList::const_iterator it = headerList.begin(); it != headerList.end(); ++it ) {
		if ( used.find( ( *it ).name ) == used.end() ) {
			insertItem((*it).name);
			completionObject()->addItem((*it).name);
			used << (*it).name;
		}
	}

	if ( listBox()->findItem( remember_text, Qt::ExactMatch ) ) {
		setCurrentText( remember_text );
	}
}

#include "headercombobox.moc"
