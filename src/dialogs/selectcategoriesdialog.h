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

#ifndef SELECTCATEGORIESDIALOG_H
#define SELECTCATEGORIESDIALOG_H


#include <k3listview.h>
#include <kdialog.h>

#include "datablocks/elementlist.h"

class CategoryCheckListView;
class RecipeDB;

/**
@author Unai Garro
*/
class SelectCategoriesDialog: public KDialog
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
