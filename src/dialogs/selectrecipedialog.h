 /**************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn(mizunoami44@users.sourceforge.net)                     *
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
#include <qcursor.h>

#include <kcombobox.h>
#include <qintdict.h>
#include <klineedit.h>
#include <klistview.h>
#include <kiconloader.h>
#include <kdialog.h>
#include <kfiledialog.h>
#include <kpopupmenu.h>

#include "exporters/kreexport.h"

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
  void getCurrentRecipe( Recipe *recipe );

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
  KPopupMenu *kpop;
  // Internal Data
  RecipeDB *database;
  ElementList *recipeList;
  ElementList *categoryList; // Stores the list of categories corresponding to "recipeList"
  QIntDict <QListViewItem> categoryItems; // Contains the QListViewItem's for every category in the QListView
  QIntDict <int> categoryComboRows; // Contains the category id for every given row in the category combobox
  bool isFilteringCategories;
  // Internal Methods
  void loadRecipeList(void);
  void loadCategoryCombo(void);

signals:
  void recipeSelected(int id, int action);
  void recipeSelected(bool);

private slots:
  void open(void);
  void edit(void);
  void remove(void);
  void filter(const QString &s);
  void filterCategories(int categoryID);
  void showPopup( KListView *, QListViewItem *, const QPoint & );
  void filterComboCategory(int row);
public slots:
  void exportRecipe();
  void haveSelectedItems();
};

#endif
