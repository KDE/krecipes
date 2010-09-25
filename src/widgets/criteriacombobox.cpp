/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "criteriacombobox.h"

#include <q3listbox.h>

#include "backends/recipedb.h"
#include "datablocks/elementlist.h"

CriteriaComboBox::CriteriaComboBox( bool b, QWidget *parent, RecipeDB *db ) : KComboBox( b, parent ),
		database( db )
{
	connect( db, SIGNAL(ratingCriteriaCreated(const Element &)), this, SLOT(addCriteria(const Element &)) );
}

void CriteriaComboBox::addCriteria( const Element &criteria )
{
	idMap.insert(count(),criteria.id);

	insertItem( count(), criteria.name );
	completionObject()->addItem(criteria.name);
}

void CriteriaComboBox::reload()
{
	ElementList criteriaList;
	database->loadRatingCriterion( &criteriaList );

	clear();

	for ( ElementList::const_iterator it = criteriaList.constBegin(); it != criteriaList.constEnd(); ++it ) {
		addCriteria((*it));
	}
}

int CriteriaComboBox::criteriaID( int index )
{
	return idMap[index];
}

#include "criteriacombobox.moc"
