/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef INGREDIENTSDIALOG_H
#define INGREDIENTSDIALOG_H

#include <qwidget.h>
#include <QGridLayout>
#include <kvbox.h>

#include "widgets/krelistview.h"
#include "widgets/dblistviewbase.h"

class RecipeDB;
class IngredientGroupsDialog;
class KPushButton;
class IngredientActionsHandler;

class IngredientsDialog: public QWidget
{
Q_OBJECT

public:
	IngredientsDialog( QWidget* parent, RecipeDB *db );
	~IngredientsDialog();
	void reload( ReloadFlags flag = Load );

private slots:
	void showPropertyEdit();

private:
	// Widgets
	QGridLayout* layout;
	KPushButton* addIngredientButton;
	KPushButton* removeIngredientButton;
	KreListView* ingredientListView;
	IngredientGroupsDialog *groupsDialog;
	IngredientActionsHandler *ingredientActionsHandler;

	// Internal Methods
	void reloadIngredientList( ReloadFlags flag = Load );

	// Internal Variables
	RecipeDB *database;
};

#endif
