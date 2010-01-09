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

#ifndef SELECTRECIPEDIALOG_H
#define SELECTRECIPEDIALOG_H


#include <qwidget.h>
#include <QLabel>
#include <q3intdict.h>

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
class QShowEvent;
class QVBoxLayout;

class RecipeDB;
class Recipe;
class AdvancedSearchDialog;
class RecipeFilter;
class RecipeListView;
class CategoryComboBox;
class RecipeActionsHandler;


class BasicSearchTab : public QFrame
{
	Q_OBJECT
public:
	BasicSearchTab( QWidget * parent );
	~BasicSearchTab(){};

	void setActionsHandler( RecipeActionsHandler * actionHandler );
protected:
	virtual void showEvent( QShowEvent * event );
private:
	RecipeActionsHandler * actionHandler;
};


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
	QVBoxLayout *layout;
	KTabWidget *tabWidget;
	BasicSearchTab *basicSearchTab;
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
