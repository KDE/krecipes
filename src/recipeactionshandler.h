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

#ifndef RECIPEACTIONSHANDLER_H
#define RECIPEACTIONSHANDLER_H
 
#include <qobject.h>
#include <qvaluelist.h>

class QListViewItem;
class KListView;
class KPopupMenu;
class RecipeDB;

/** @brief A class that centralizes common actions for recipes such as saving and editing.
  * 
  * It acts upon a given KListView that is assumed to be a list of recipes.  It 
  * automagically enables this list view with a popup menu for user access to 
  * the provided actions.
  *
  * @author Jason Kivlighn
  */
class RecipeActionsHandler : public QObject
{
Q_OBJECT

public:
	typedef enum ItemType { Category, Recipe };
	typedef enum RecipeActions { 
			AllActions =         0x00ff,
			Open =               0x0001,
			Edit =               0x0002,
			SaveAs =             0x0004,
			RemoveFromCategory = 0x0008,
			Remove =             0x0010,
			ExpandAll =          0x0020,
			CollapseAll =        0x0040,
			SaveCategoryAs =     0x0080
		};

	RecipeActionsHandler( KListView *parentListView, RecipeDB *db, int recipe_name_col, int recipe_id_col, int category_name_col, int actions = AllActions );
	~RecipeActionsHandler(){}

signals:
	void recipeSelected(int id, int action);
	void recipesSelected(const QValueList<int> &ids, int action);
		
	/** When this class is unable to sync the list view with the database after
	  * certain operations, we emit this signal, allowing the appropriate class
	  * to update the list view.
	  */
	void reloadNeeded();
	
public slots:
	void exec( ItemType type, const QPoint &p );
	void showPopup( KListView *, QListViewItem *, const QPoint & );

	/** Signals an open event (via the recipeSelected() signal) for the recipe currently
	  * selected in the list view 
	  */
	void open();

	/** Signals an edit event (via the recipeSelected() signal) for the recipe currently
	  * selected in the list view 
	  */
	void edit();

	/** Saves the recipe currently selected in the list view, prompting with a file
          * dialog.
	  */
	void saveAs();

	/** Removes the recipe currently selected in the list view from its current category */
	void removeFromCategory();

	/** Removes the recipe currently selected in the list view from the database */
	void remove();
	
	/** Expands all items in the list view */
	void expandAll();
	
	/** Collapses all items in the list view */
	void collapseAll();
	
	/** Saves all recipes in the currently selected category */
	void saveCategoryAs();

private:
	KPopupMenu *kpop;
	KPopupMenu *catPop;
	
	KListView *parentListView;
	RecipeDB *database;
	
	int recipe_name_col, recipe_id_col, category_name_col;
	int remove_from_cat_item;

	void exportRecipes( const QValueList<int> &ids, const QString & caption, const QString &selection );
	
	//item is a recipe if the 2nd column is an integer (the recipe's ID)
	bool itemIsRecipe( const QListViewItem *item );

	QValueList<int> getAllVisibleItems();
};

#endif //RECIPEACTIONSHANDLER_H

