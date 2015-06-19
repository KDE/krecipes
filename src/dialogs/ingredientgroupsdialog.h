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

#include <QWidget>

#include "widgets/kregenericlistwidget.h"

class KreHeaderListWidget;
class KreGenericActionsHandler;
class KreHeaderActionsHandler;
class KAction;
class RecipeDB;

class IngredientGroupsDialog : public QWidget
{
public:
	IngredientGroupsDialog( RecipeDB *db, QWidget *parent, const char *name );

	void reload( ReloadFlags flag = Load );

	KreGenericActionsHandler* getActionsHandler() const;

	void addAction( KAction * action );

private:
	KreHeaderListWidget *headerListWidget;
	KreHeaderActionsHandler *headerActionsHandler;
	RecipeDB *database;
};

#endif
