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
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>

#include <kcombobox.h>
#include <klineedit.h>
#include <klistview.h>
#include <kiconloader.h>
#include <kdialog.h>
#include <kfiledialog.h>

#include "importers/kremanager.h"

class QHBox;

class RecipeDB;
class ElementList;

/**
@author Unai Garro
*/
class SelectRecipeDialog : public QWidget
{
Q_OBJECT
public:
    SelectRecipeDialog(QWidget *parent, RecipeDB *db);

    ~SelectRecipeDialog();

  //Public Methods
  void reload(void);
private:

  // Widgets
  QGridLayout *layout;
  QHBox *searchBar;
  KListView* recipeListView;
  QHBox *buttonBar;
  QPushButton *openButton;
  QPushButton *removeButton;
  QPushButton *editButton;
  KIconLoader *il;
  QLabel *searchLabel;
  KLineEdit *searchBox;
  KComboBox *categoryBox;
  // Internal Data
  RecipeDB *database;
  ElementList *recipeList;
  ElementList *categoryList; // Stores the list of categories corresponding to "recipeList"
  // Internal Methods
  void loadRecipeList(void);


signals:
  void recipeSelected(int id, int action);
private slots:
  void open(void);
  void edit(void);
  void remove(void);
  void filter(const QString &s);
public slots:
  void exportRecipe();
};

#endif
