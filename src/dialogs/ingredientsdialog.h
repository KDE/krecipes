/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef INGREDIENTSDIALOG_H
#define INGREDIENTSDIALOG_H

#include <qpushbutton.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qwidget.h>
#include <klistview.h>
#include <knuminput.h>
#include "recipedb.h"
#include "selectunitdialog.h"
#include "createelementdialog.h"
#include "ingredientpropertylist.h"
#include "selectpropertydialog.h"
#include "editbox.h"
#include "unitsdialog.h"

/**
@author Unai Garro
*/
class IngredientsDialog:public QWidget{

Q_OBJECT

public:
    IngredientsDialog(QWidget* parent, RecipeDB *db);
    ~IngredientsDialog();
    void reload(void);

private:
  // Widgets
    QGridLayout* layout;
    QPushButton* addIngredientButton;
    QPushButton* removeIngredientButton;
    QPushButton* addUnitButton;
    QPushButton* removeUnitButton;
    QPushButton* addPropertyButton;
    QPushButton* removePropertyButton;
    KListView* ingredientListView;
    KListView* unitsListView;
    KListView* propertiesListView;
    QPushButton* pushButton5;
    EditBox* inputBox;

  // Internal Methods
  void reloadIngredientList(void);
  void reloadUnitList(void);
  void reloadPropertyList(void);
  int findPropertyNo(QListViewItem *it);
  // Internal Variables
  RecipeDB *database;
  ElementList *ingredientList;
  ElementList *unitList;
  IngredientPropertyList *propertiesList;
  ElementList  *perUnitListBack;

  private slots:
    void addIngredient(void);
    void addUnitToIngredient(void);
    void removeUnitFromIngredient(void);
    void removeIngredient(void);
    void updateLists(void);
    void addPropertyToIngredient(void);
    void removePropertyFromIngredient(void);
    void insertPropertyEditBox(QListViewItem* it);
    void setPropertyAmount(double amount);

};

#endif
