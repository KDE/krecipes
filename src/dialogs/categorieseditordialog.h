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

#ifndef CATEGORIESEDITORDIALOG_H
#define CATEGORIESEDITORDIALOG_H

#include <qwidget.h>
#include <qpushbutton.h>
#include <qhbox.h>
#include <qlayout.h>
#include <kiconloader.h>
#include <klistview.h>

#include "datablocks/categorytree.h"

class RecipeDB;

/**
@author Unai Garro
*/
class CategoriesEditorDialog:public QWidget{

Q_OBJECT

public:

	CategoriesEditorDialog(QWidget* parent, RecipeDB *db);
	~CategoriesEditorDialog();
	void reload(void);
private:
	// Internal data
	RecipeDB *database;
	//Widgets
	QGridLayout *layout;
	KListView *categoryListView;
	QHBox *buttonBar;
	QPushButton *newCategoryButton;
	QPushButton *removeCategoryButton;
	KIconLoader *il;
	
	void loadListView(const CategoryTree *categoryTree, QListViewItem *parent=0 );

private slots:
	void createNewCategory(void);
	void removeCategory(void);
	void changeCategoryParent(QListViewItem *item,QListViewItem *afterFirst,QListViewItem *afterNow);
  void modCategory(QListViewItem*);
  void saveCategory(QListViewItem*);

};

#endif
