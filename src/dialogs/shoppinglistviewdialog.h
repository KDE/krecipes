/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef SHOPPINGLISTVIEWDIALOG_H
#define SHOPPINGLISTVIEWDIALOG_H

#include <khtml_part.h>
#include <khtmlview.h>

#include "recipedb.h"
#include "qwidget.h"
class ShoppingListViewDialog:public QWidget{
public:
    ShoppingListViewDialog(QWidget *parent, RecipeDB *db);
    ~ShoppingListViewDialog();
private:
  // Internal Widgets
  KHTMLPart *shoppingListView;

  // Internal Variables
  RecipeDB  *database;

  // Internal Methods
  void showShoppingList(void);
  void calculateShopping(void);
  void display(void);
};

#endif
