/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef INGREDIENTGROUPSDIALOG_H
#define INGREDIENTGROUPSDIALOG_H

#include <qwidget.h>

class KreListView;
class RecipeDB;

class IngredientGroupsDialog : public QWidget
{
public:
	IngredientGroupsDialog( RecipeDB *db, QWidget *parent, const char *name );

	void reload();

private:
	KreListView *headerListView;
	RecipeDB *database;
};

#endif
