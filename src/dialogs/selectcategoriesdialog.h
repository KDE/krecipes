/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef SELECTCATEGORIESDIALOG_H
#define SELECTCATEGORIESDIALOG_H

#include <qwidget.h>
#include <qlayout.h>
#include <klistview.h>
#include "recipedb.h"

/**
@author Unai Garro
*/
class SelectCategoriesDialog:public QWidget{

Q_OBJECT

public:

    SelectCategoriesDialog(QWidget *parent, RecipeDB *db);
    ~SelectCategoriesDialog();
private:

	//Widgets
	KListView *categoryListView;
	QGridLayout *layout;

};

#endif
