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

#ifndef SHOPPINGLISTDIALOG_H
#define SHOPPINGLISTDIALOG_H


//Added by qt3to4:
#include <QGridLayout>
#include <QDropEvent>
#include <kpushbutton.h>
#include <kiconloader.h>
#include <kvbox.h>

#include "widgets/krelistview.h"
#include "widgets/dblistviewbase.h"

class RecipeDB;
class RecipeList;
class ShoppingListViewDialog;
class RecipeActionsHandler;
class RecipeListView;

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
	// Internal Methods
	void reloadRecipeList( ReloadFlags flag = Load );
	void addRecipe( Q3ListViewItem *item );
	// Widgets
	QGridLayout* layout;
	KPushButton* addRecipeButton;
	KPushButton* removeRecipeButton;
	KreListView* recipeListView;
	KreListView* shopRecipeListView;
	KHBox *buttonBar;
	KPushButton* okButton;
	KPushButton* clearButton;
	ShoppingListViewDialog *shoppingListDisplay;
	RecipeActionsHandler *actionHandler;
	RecipeListView *listview;

public slots:
	void addRecipeToShoppingList( int recipeID ); // Called from inputdialog-> through krecipesview

private slots:
	void addRecipe( void );
	void removeRecipe( void );
	void showShoppingList( void );
	void clear( void );
	void slotDropped( K3ListView *list, QDropEvent *e, Q3ListViewItem *after );
	void ensurePopulated();
};

#endif
