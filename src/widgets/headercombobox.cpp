/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "headercombobox.h"

#include <q3listbox.h>

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

	for ( ElementList::const_iterator it = headerList.begin(); it != headerList.end(); ++it ) {
		insertItem((*it).name);
		completionObject()->addItem((*it).name);
	}

	if ( findText( remember_text, Qt::MatchExactly ) ) {
		setCurrentText( remember_text );
	}
}

#include "headercombobox.moc"
