/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef RECIPEVIEWDIALOG_H
#define RECIPEVIEWDIALOG_H

#include <qvbox.h>
#include <qstring.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <iostream>

#include "recipedb.h"

/**
@author Unai Garro
*/

class RecipeViewDialog : public QVBox
{
Q_OBJECT

public:
    RecipeViewDialog(QWidget *parent, RecipeDB *db, int recipeID=-1);

    ~RecipeViewDialog();
    void loadRecipe(int recipeID);

private:

  // Internal Variables
  KHTMLPart *recipeView;
  RecipeDB  *database;
  Recipe *loadedRecipe;

  // Internal Methods
  void calculateIngredients(IDList &ingList, QPtrList <int> &unitList, QPtrList <double> &amountList);
  int autoConvert(double amount1,int unit1,double amount2,int unit2, double &newAmount, int &newID);
  void calculatePropertiesOld(void);
  void calculateProperties(void);
  void addPropertyToList(IDList &idList,QPtrList <double> & amountList,QPtrList <QString> &unitList,IngredientPropertyList &list,int ingUnitID, double ingAmount);
  void addPropertyToList(IngredientPropertyList &recipePropertyList,IngredientPropertyList &ingPropertyList,Ingredient &ing);

private slots:
  void showRecipe(void);

};

#endif

