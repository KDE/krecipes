/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef INGREDIENTGROUPSDIALOG_H
#define INGREDIENTGROUPSDIALOG_H

#include <qwidget.h>

#include "widgets/dblistviewbase.h"

class KreListView;
class ActionsHandlerBase;
class KAction;
class HeaderActionsHandler;
class RecipeDB;

class IngredientGroupsDialog : public QWidget
{
public:
	IngredientGroupsDialog( RecipeDB *db, QWidget *parent, const char *name );

	void reload( ReloadFlags flag = Load );

	ActionsHandlerBase* getActionsHandler() const;

	void addAction( KAction * action );

private:
	KreListView *headerListView;
	HeaderActionsHandler *headerActionsHandler;
	RecipeDB *database;
};

#endif
