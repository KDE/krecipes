/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CATEGORYCOMBOBOX_H
#define CATEGORYCOMBOBOX_H

#include <kcombobox.h>

#include <qmap.h>

#include "element.h"

class RecipeDB;

class CategoryComboBox : public KComboBox
{
Q_OBJECT

public:
	CategoryComboBox( QWidget *parent, RecipeDB *db );

	void reload();
	int id( int row );

private slots:
	void createCategory(const Element &element, int /*parent_id*/);
	void removeCategory(int id);
	void modifyCategory(const Element &element);
	void mergeCategories(int /*to_id*/, int from_id);

	int findInsertionPoint(const QString &name);

private:
	RecipeDB *database;
	QMap<int,int> categoryComboRows; // Contains the category id for every given row in the category combobox
};

#endif //CATEGORYCOMBOBOX_H

