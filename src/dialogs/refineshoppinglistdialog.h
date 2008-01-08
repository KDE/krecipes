/***************************************************************************
*   Copyright (C) 2004 by Jason Kivlighn                                  *
*   (jkivlighn@gmail.com)                                                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef REFINESHOPPINGLISTDIALOG_H
#define REFINESHOPPINGLISTDIALOG_H

#include <qvariant.h>
#include <qmap.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <QLabel>

#include <kdialogbase.h>

#include "datablocks/ingredientlist.h"

class Q3VBoxLayout;
class Q3HBoxLayout;
class Q3GridLayout;
class QSpacerItem;
class QLabel;
class Q3ListViewItem;
class QPushButton;

class RecipeDB;
class ElementList;
class KreListView;

class RefineShoppingListDialog : public KDialogBase
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

protected:
	Q3VBoxLayout* layout1;
	QSpacerItem* spacer1;

protected slots:
	virtual void languageChange();
	virtual void accept();
	void addIngredient();
	void removeIngredient();
	void itemRenamed( Q3ListViewItem*, const QString &, int );

private:
	void loadData();

	RecipeDB *database;
	IngredientList ingredientList;
	QMap<Q3ListViewItem*, IngredientList::iterator> item_ing_map;
};

#endif // REFINESHOPPINGLISTDIALOG_H
