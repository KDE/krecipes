/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef SHOPPINGLISTDIALOG_H
#define SHOPPINGLISTDIALOG_H

#include "datablocks/recipelist.h"
#include "widgets/krelistview.h"
#include "widgets/dblistviewbase.h"

class RecipeDB;
class ShoppingListViewDialog;
class RecipeActionsHandler;
class RecipeListView;

class KreRecipesListWidget;
class KreSearchResultListWidget;

class QGridLayout;
class KPushButton;

/**
@author Unai Garro
*/

class ShoppingListDialog: public QWidget
{
	Q_OBJECT
public:

	ShoppingListDialog( QWidget *parent, RecipeDB *db );
	~ShoppingListDialog();
	void reload( ReloadFlags flag = Load );
	void createShopping( const RecipeList &rlist );

private:

	// Internal variables
	RecipeDB *database;
	// Widgets
	QGridLayout* layout;
	KPushButton* addRecipeButton;
	KPushButton* removeRecipeButton;
	KreListView* recipeListView;
	KreListView* shopRecipeListView;
	KHBox *buttonBar;
	KPushButton* okButton;
	KPushButton* clearButton;
	KreRecipesListWidget * m_sourceListWidget;
	KreSearchResultListWidget * m_destListwidget;

public slots:
	void addRecipeToShoppingList( int recipeID ); // Called from inputdialog-> through krecipesview

private slots:
	void addRecipe( void );
	void removeRecipe( void );
	void showShoppingList( void );
	void clear( void );
};

#endif
