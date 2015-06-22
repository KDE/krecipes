/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2015 José Manuel Santamaría Lemamail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef KRERECIPEACTIONSHANDLER_H
#define KRERECIPEACTIONSHANDLER_H

#include "kregenericactionshandler.h"

class KAction;
class KTempDir;
class KWebPage;
class KreRecipesListWidget;

class KreRecipeActionsHandler : public KreGenericActionsHandler
{
	Q_OBJECT

public:
	KreRecipeActionsHandler( KreRecipesListWidget * listWidget, RecipeDB * db );
	~KreRecipeActionsHandler(){}

	void addRecipeAction( KAction * action );
	void addCategoryAction( KAction * action );
	void setCategorizeAction( KAction * action );
	void setRemoveFromCategoryAction( KAction * action );

	static void exportRecipes( const QList<int> &ids, const QString & caption, const QString &selection, RecipeDB *db );

signals:
	void recipeSelected( bool );
	void recipeSelected( int id, int action );
	void recipesSelected( const QList<int> &ids, int action );
	
	//For internal use only
	void printDone();

public slots:
	void showPopup( const QModelIndex & index, const QPoint & point );

	void createNew();

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

	/** Prints the recipe(s) currently selected in the list view, showing a dialog to preview */
	void recipePrint();
	
	/** Prints the given recipes, showing a dialog to preview the result */
	void printRecipes( const QList<int> &ids, RecipeDB *db );

	/** Removes the recipe(s) currently selected in the list view from its current category */
	void removeFromCategory();

	/** Removes the recipe(s) currently selected in the list view from the database */
	void remove();

	/** Add the recipe(s) currently selected in the list view to the shopping list dialog */
	void addToShoppingList();

	/** Expands all items in the list view */
	void expandAll();

	/** Collapses all items in the list view */
	void collapseAll();

	void recipesToClipboard();
	
	void selectionChangedSlot();

protected slots:
	void saveElement( const QModelIndex& topLeft, 
		const QModelIndex& bottomRight );

private slots:
	void print( bool ok );
	void printDoneSlot();

private:
	KMenu * m_recipesMenu;
	KMenu * m_categoriesMenu;

	KAction * m_categorizeAction;
	KAction * m_removeFromCategoryAction;

	KTempDir * m_tempdir;
	KWebPage * m_printPage;
};

#endif //KRERECIPEACTIONSHANDLER_H

