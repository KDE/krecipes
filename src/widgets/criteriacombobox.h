/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef CRITERIACOMBOBOX_H
#define CRITERIACOMBOBOX_H

#include <qmap.h>

#include <kcombobox.h>

#include "datablocks/element.h"

class RecipeDB;

class CriteriaComboBox : public KComboBox
{
	Q_OBJECT

public:
	CriteriaComboBox( bool, QWidget *parent, RecipeDB *db );

	void reload();
	int criteriaID( int index );

protected slots:
	void addCriteria( const Element &criteria );

private:
	RecipeDB *database;
	QMap< int, int > idMap;
};

#endif //CRITERIACOMBOBOX_H

