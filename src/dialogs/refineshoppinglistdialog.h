/***************************************************************************
 *   Copyright (C) 2004 by Jason Kivlighn                                  *
 *   mizunoami44@users.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef REFINESHOPPINGLISTDIALOG_H
#define REFINESHOPPINGLISTDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
#include <qmap.h>

#include "ingredientlist.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QListViewItem;
class QPushButton;

class RecipeDB;
class ElementList;
class KreListView;

class RefineShoppingListDialog : public QDialog
{
Q_OBJECT
	
public:
	RefineShoppingListDialog( QWidget* parent, RecipeDB *db, const ElementList &recipeList );
	~RefineShoppingListDialog();
	
	QLabel* helpLabel;
	KreListView* allIngListView;
	QPushButton* addButton;
	QPushButton* removeButton;
	KreListView* ingListView;
	QPushButton* doneButton;
	
protected:
	QVBoxLayout* refineShoppingListDialogLayout;
	QHBoxLayout* layout2;
	QVBoxLayout* layout1;
	QSpacerItem* spacer1;
	QHBoxLayout* layout3;
	QSpacerItem* spacer2;
	
protected slots:
	virtual void languageChange();
	virtual void accept();
	void addIngredient();
	void removeIngredient();
	void itemRenamed(QListViewItem*,const QString &,int);

private:
	void loadData();

	RecipeDB *database;
	IngredientList ingredientList;
	QMap<QListViewItem*,IngredientList::iterator> item_ing_map;
};

#endif // REFINESHOPPINGLISTDIALOG_H
