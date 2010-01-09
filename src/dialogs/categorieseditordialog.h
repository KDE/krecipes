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

#ifndef CATEGORIESEDITORDIALOG_H
#define CATEGORIESEDITORDIALOG_H

#include <qwidget.h>

//Added by qt3to4:
#include <QGridLayout>
#include <k3listview.h>
#include <kvbox.h>

#include "datablocks/categorytree.h"
#include "widgets/dblistviewbase.h"

class RecipeDB;
class StdCategoryListView;
class ActionsHandlerBase;
class KAction;
class CategoryActionsHandler;
class KPushButton;

/**
@author Unai Garro
*/
class CategoriesEditorDialog: public QWidget
{

	Q_OBJECT

public:

	CategoriesEditorDialog( QWidget* parent, RecipeDB *db );
	~CategoriesEditorDialog();

	void reload( ReloadFlags flag = Load );

	CategoryActionsHandler* getActionsHandler() const;
	void addAction( KAction * action );
	void setCategoryPasteAction( KAction * action );
	void setPasteAsSubcategoryAction( KAction * action );

private:
	// Internal data
	RecipeDB *database;
	//Widgets
	QGridLayout *layout;
	StdCategoryListView *categoryListView;
	CategoryActionsHandler *categoryActionsHandler;
	KHBox *buttonBar;
	KPushButton *newCategoryButton;
	KPushButton *removeCategoryButton;
};

#endif
