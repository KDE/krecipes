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

#include <qlayout.h>
#include <qvbox.h>
#include <qwidget.h>
#include <khtml_part.h>
#include <khtmlview.h>

#include "recipedb.h"

class ShoppingListViewDialog:public QWidget{
public:
    ShoppingListViewDialog(QWidget *parent, RecipeDB *db);
    ~ShoppingListViewDialog();
private:

  // Widgets
  QGridLayout* layout;
  QVBox *htmlBox;
  KHTMLPart *shoppingListView;

  // Internal Variables
  RecipeDB  *database;

  // Internal Methods
  void showShoppingList(void);
  void calculateShopping(void);
  void display(void);
};

#endif
