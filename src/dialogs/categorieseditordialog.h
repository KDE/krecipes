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

#ifndef CATEGORIESEDITORDIALOG_H
#define CATEGORIESEDITORDIALOG_H

#include <qwidget.h>
#include <qpushbutton.h>

#include <qlayout.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <kiconloader.h>
#include <k3listview.h>
#include <kvbox.h>

#include "datablocks/categorytree.h"
#include "widgets/dblistviewbase.h"

class RecipeDB;
class StdCategoryListView;

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

private:
	// Internal data
	RecipeDB *database;
	//Widgets
	Q3GridLayout *layout;
	StdCategoryListView *categoryListView;
	KHBox *buttonBar;
	QPushButton *newCategoryButton;
	QPushButton *removeCategoryButton;
	KIconLoader *il;
};

#endif
