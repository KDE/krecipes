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

#ifndef SELECTCATEGORIESDIALOG_H
#define SELECTCATEGORIESDIALOG_H

#include <qlayout.h>
#include <qpushbutton.h>

#include <k3listview.h>
#include <kdialogbase.h>

#include "datablocks/elementlist.h"

class CategoryTree;
class CategoryCheckListItem;
class CategoryCheckListView;
class RecipeDB;

/**
@author Unai Garro
*/
class SelectCategoriesDialog: public KDialogBase
{

	Q_OBJECT

public:

	SelectCategoriesDialog( QWidget *parent, const ElementList &items_on, RecipeDB* db );
	~SelectCategoriesDialog();
	void getSelectedCategories( ElementList *selected );
private:

	//Widgets
	CategoryCheckListView *categoryListView;

	//Variables
	RecipeDB *database;

	//Private methods
	void loadCategories( const ElementList &items_on );

private slots:
	void createNewCategory( void );
};

#endif
