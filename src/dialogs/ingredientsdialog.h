/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef INGREDIENTSDIALOG_H
#define INGREDIENTSDIALOG_H

#include <qwidget.h>
#include <kvbox.h>

#include "widgets/krelistview.h"
#include "widgets/dblistviewbase.h"

class RecipeDB;
class IngredientGroupsDialog;
class KPushButton;
class ActionsHandlerBase;
class IngredientActionsHandler;
class KTabWidget;
class KAction;
class QHBoxLayout;
class QVBoxLayout;

class IngredientsDialog: public QWidget
{
Q_OBJECT

public:
	IngredientsDialog( QWidget* parent, RecipeDB *db );
	~IngredientsDialog();
	void reload( ReloadFlags flag = Load );
	ActionsHandlerBase *getActionsHandler() const;
	void addAction( KAction * action );

private slots:
	void showPropertyEdit();

private:
	// Widgets
	QHBoxLayout* layout;
	QVBoxLayout* listLayout;
	KPushButton* addIngredientButton;
	KPushButton* removeIngredientButton;
	KreListView* ingredientListView;
	IngredientGroupsDialog *groupsDialog;
	IngredientActionsHandler *ingredientActionsHandler;
	KTabWidget* tabWidget;
	QWidget* ingredientTab;

	// Internal Methods
	void reloadIngredientList( ReloadFlags flag = Load );

	// Internal Variables
	RecipeDB *database;
};

#endif
