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

#ifndef RECIPEIMPORTDIALOG_H
#define RECIPEIMPORTDIALOG_H

#include <qdialog.h>
#include <qptrlist.h>
#include <qptrvector.h>
#include <qlistview.h>

class KPushButton;
class KListView;

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QListViewItem;

class Recipe;
class CustomCheckListItem;

/**
  * @author Jason Kivlighn
  */
class RecipeImportDialog : public QDialog
{
public:
	RecipeImportDialog( QPtrList<Recipe> *all_recipes, QWidget *parent = 0 );
	~RecipeImportDialog();

	QPtrList<Recipe> * getSelectedRecipes();

protected slots:
	virtual void languageChange();

private:
	void loadListView( QPtrList<Recipe> * );

	KListView* kListView;
	KPushButton* okButton;
	KPushButton* cancelButton;

	QHBoxLayout* Form1Layout;
	QVBoxLayout* layout2;

	QPtrList<Recipe> *m_recipe_list;
	QPtrVector<CustomCheckListItem> *recipe_items;

	CustomCheckListItem *head_item;
};

/** A specialized QCheckListItem that sets the state of its children to its
  * current state.
  * @author Jason Kivlighn
  */
class CustomCheckListItem : public QCheckListItem
{
public:
	CustomCheckListItem(QListView *parent, const QString &, Type );
	CustomCheckListItem(QCheckListItem *parent, QCheckListItem *after, const QString &, Type);

protected:
	virtual void stateChange(bool);
	bool locked(){ return m_locked; }
	void setLocked( bool b ){ m_locked = b; }

private:
	bool m_locked;
};

#endif //RECIPEIMPORTDIALOG_H
