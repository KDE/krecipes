/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
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

/**
@author Unai Garro
*/
class IngredientsDialog:public QWidget{

Q_OBJECT

public:
    IngredientsDialog(QWidget* parent, RecipeDB *db);

    ~IngredientsDialog();

private:
  // Widgets
    QGridLayout* layout;
    QPushButton* addIngredientButton;
    QPushButton* removeIngredientButton;
    QPushButton* addUnitButton;
    QPushButton* removeUnitButton;
    KListView* ingredientListView;
    KListView* unitsListView;
    KListView* characteristicsListView;
    QPushButton* pushButton5;

  // Internal Methods
  void reloadIngredientList(void);
  void reloadPossibleUnitList(int recipeID);

  // Internal Variables
  RecipeDB *database;
  ElementList *ingredientList;
  ElementList *unitList;


  private slots:
    void updateUnitList(void);
    void addIngredient(void);




};

#endif
