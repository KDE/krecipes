/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef SHOPPINGLISTDIALOG_H
#define SHOPPINGLISTDIALOG_H

#include <qlayout.h>
#include <qpushbutton.h>
#include <klistview.h>
#include "recipedb.h"
/**
@author Unai Garro
*/
class ShoppingListDialog:public QWidget{
Q_OBJECT
public:

    ShoppingListDialog(QWidget *parent,RecipeDB *db);
    ~ShoppingListDialog();
    void reload(void);

private:

// Internal variables
    RecipeDB *database;
// Internal Methods
    void reloadRecipeList(void);
// Widgets
    QGridLayout* layout;
    QPushButton* addRecipeButton;
    QPushButton* removeRecipeButton;
    KListView* recipeListView;
    KListView* shopRecipeListView;
    QPushButton* okButton;

};

#endif
