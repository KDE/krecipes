/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef SELECTRECIPEDIALOG_H
#define SELECTRECIPEDIALOG_H

#include <qwidget.h>
#include <qvbox.h>
#include <klistview.h>
#include "recipedb.h"
#include "recipe.h"

/**
@author Unai Garro
*/
class SelectRecipeDialog : public QVBox
{
public:
    SelectRecipeDialog(QWidget *parent, RecipeDB *db);

    ~SelectRecipeDialog();
private:

  // Widgets
  KListView* recipeListView;
  // Internal Data
  RecipeDB *database;
  ElementList *recipeList;
  // Internal Methods
  void loadRecipeList(void);



};

#endif
