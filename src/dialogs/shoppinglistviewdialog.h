/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
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

class IngredientList;

class ShoppingListViewDialog:public QWidget{
Q_OBJECT

public:
    ShoppingListViewDialog(QWidget *parent, const IngredientList &ingredientList);
    ~ShoppingListViewDialog();

public slots:
  void print();

private:

  // Widgets
  QGridLayout* layout;
  QVBox *htmlBox;
  KHTMLPart *shoppingListView;

  // Internal Methods
  void display(const IngredientList &ingredientList);
};

#endif
