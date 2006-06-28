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

#ifndef RECIPEACTIONSHANDLER_H
#define RECIPEACTIONSHANDLER_H

#include <qobject.h>
#include <qvaluelist.h>
#include <qptrlist.h>

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
	    AllActions = 0xffff,
	    Open = 0x0001,
	    Edit = 0x0002,
	    Export = 0x0004,
	    RemoveFromCategory = 0x0008,
	    Remove = 0x0010,
	    ExpandAll = 0x0020,
	    CollapseAll = 0x0040,
	    AddToShoppingList = 0x0080,
	    CopyToClipboard = 0x0100,
	    Categorize = 0x0200
	};

	RecipeActionsHandler( KListView *parentListView, RecipeDB *db, int actions = AllActions );
	~RecipeActionsHandler()
	{}

	static void exportRecipes( const QValueList<int> &ids, const QString & caption, const QString &selection, RecipeDB *db );
	static void exportRecipe( int id, const QString & caption, const QString &selection, RecipeDB *db );
	static void recipesToClipboard( const QValueList<int> &ids, RecipeDB *db );

signals:
	void recipeSelected( int id, int action );
	void recipesSelected( const QValueList<int> &ids, int action );

public slots:
	void exec( ItemType type, const QPoint &p );
	void showPopup( KListView *, QListViewItem *, const QPoint & );

	void categorize();

	/** Signals an open event (via the recipeSelected() signal) for the recipe(s) currently
	  * selected in the list view 
	  */
	void open();

	/** Signals an edit event (via the recipeSelected() signal) for the recipe currently
	  * selected in the list view 
	  */
	void edit();

	/** Saves the recipe(s) currently selected in the list view, prompting with a file
	         * dialog.
	  */
	void recipeExport();

	/** Removes the recipe(s) currently selected in the list view from its current category */
	void removeFromCategory();

	/** Removes the recipe(s) currently selected in the list view from the database */
	void remove
		();

	/** Add the recipe(s) currently selected in the list view to the shopping list dialog */
	void addToShoppingList();

	/** Expands all items in the list view */
	void expandAll();

	/** Collapses all items in the list view */
	void collapseAll();

	void recipesToClipboard();

private:
	KPopupMenu *kpop;
	KPopupMenu *catPop;

	KListView *parentListView;
	RecipeDB *database;

	int remove_from_cat_item;
	int categorize_item;

	QValueList<int> getAllVisibleItems();
	QValueList<int> recipeIDs( const QPtrList<QListViewItem> &items ) const;
};

#endif //RECIPEACTIONSHANDLER_H

