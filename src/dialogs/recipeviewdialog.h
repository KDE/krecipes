/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                                       *
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
  IngredientPropertyList *properties;

  // Internal Methods

private slots:
  void showRecipe(void);

};

#endif

