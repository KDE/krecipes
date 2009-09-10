/**************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn(jkivlighn@gmail.com)                                   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef SELECTRECIPEDIALOG_H
#define SELECTRECIPEDIALOG_H


#include <qwidget.h>
#include <QLabel>
#include <q3intdict.h>
//Added by qt3to4:
#include <QGridLayout>

#include <kcombobox.h>
#include <klineedit.h>
#include <k3listview.h>
#include <kdialog.h>
#include <kfiledialog.h>
#include <kmenu.h>
#include <kvbox.h>

#include "actionshandlers/recipeactionshandler.h"
#include "widgets/dblistviewbase.h"

class KHBox;
class QFrame;
class KPushButton;
class KTabWidget;
class KAction;

class RecipeDB;
class Recipe;
class AdvancedSearchDialog;
class RecipeFilter;
class RecipeListView;
class CategoryComboBox;

/**
@author Unai Garro
*/
class SelectRecipeDialog : public QWidget
{
	Q_OBJECT
public:
	SelectRecipeDialog( QWidget *parent, RecipeDB *db );
	~SelectRecipeDialog();


	//Public Methods
	void getCurrentRecipe( Recipe *recipe );

	RecipeActionsHandler * getActionsHandler() const;
	void addSelectRecipeAction( KAction * action );
	void addFindRecipeAction( KAction * action );
	void addCategoryAction( KAction * action );
	void setCategorizeAction( KAction * action );
	void setRemoveFromCategoryAction( KAction * action );

private:

	// Widgets
	QGridLayout *layout;
	KTabWidget *tabWidget;
	QFrame *basicSearchTab;
	KHBox *searchBar;
	RecipeListView* recipeListView;
	KHBox *buttonBar;
	KPushButton *openButton;
	KPushButton *removeButton;
	KPushButton *editButton;
	QLabel *searchLabel;
	KLineEdit *searchBox;
	CategoryComboBox *categoryBox;
	AdvancedSearchDialog *advancedSearch;
	// Internal Data
	RecipeDB *database;
	RecipeActionsHandler *actionHandler;
	RecipeFilter *recipeFilter;

signals:
	void recipeSelected( int id, int action );
	void recipesSelected( const QList<int> &ids, int action );
	void recipeSelected( bool );

private slots:
	void filterComboCategory( int row );
	void refilter();
	void ensurePopulated();

public slots:
	void haveSelectedItems();
	void reload( ReloadFlags flag = Load );
	void clearSearch();
};

#endif
