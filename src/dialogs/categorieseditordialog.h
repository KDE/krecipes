/*****************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                           *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>                 *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                    *
*   Copyright © 2009-2012 José Manuel Santamaría Lema <panfaust@gmail.com>   *
*                                                                            *
*   This program is free software; you can redistribute it and/or modify     *
*   it under the terms of the GNU General Public License as published by     *
*   the Free Software Foundation; either version 2 of the License, or        *
*   (at your option) any later version.                                      *
******************************************************************************/

#ifndef CATEGORIESEDITORDIALOG_H
#define CATEGORIESEDITORDIALOG_H

#include <qwidget.h>

#include <QGridLayout>
#include <kvbox.h>

#include "datablocks/categorytree.h"
#include "widgets/dblistviewbase.h" //needed for ReloadFlags

class RecipeDB;
class KreCategoriesListWidget;
class KAction;
class KreGenericActionsHandler;
class KreCategoryActionsHandler;
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

	KreCategoryActionsHandler* getActionsHandler() const;
	void addAction( KAction * action );
	void setCategoryPasteAction( KAction * action );
	void setPasteAsSubcategoryAction( KAction * action );

private:
	// Internal data
	RecipeDB *database;
	//Widgets
	QGridLayout *layout;
	KreCategoriesListWidget * m_categoriesListWidget;
	KreCategoryActionsHandler *categoryActionsHandler;
	KHBox *buttonBar;
	KPushButton *newCategoryButton;
	KPushButton *removeCategoryButton;
};

#endif
