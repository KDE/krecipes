/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
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
#include <qpushbutton.h>
#include <klistview.h>
#include <kiconloader.h>

#include "recipedb.h"
#include "recipe.h"
#include "selectunitdialog.h"
#include "createelementdialog.h"

/**
@author Unai Garro
*/
class SelectRecipeDialog : public QVBox
{
Q_OBJECT
public:
    SelectRecipeDialog(QWidget *parent, RecipeDB *db);

    ~SelectRecipeDialog();

  //Public Methods
  void reload(void);
private:

  // Widgets
  KListView* recipeListView;
  QHBox *buttonBar;
  QPushButton *openButton;
  QPushButton *removeButton;
  QPushButton *editButton;
  KIconLoader *il;
  // Internal Data
  RecipeDB *database;
  ElementList *recipeList;

  // Internal Methods
  void loadRecipeList(void);


signals:
  void recipeSelected(int id, int action);
private slots:
  void open(void);
  void edit(void);
  void remove(void);




};

#endif
