/***************************************************************************
 *   Copyright (C) 2004 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
 
#include "ingredientlistview.h"

#include <kmessagebox.h>

#include "DBBackend/recipedb.h"
 
IngredientListView::IngredientListView( QWidget *parent, RecipeDB *db ) : KListView(parent),
  database(db)
{
	connect(database,SIGNAL(ingredientCreated(const Element &)),SLOT(createIngredient(const Element &)));
	connect(database,SIGNAL(ingredientRemoved(int)),SLOT(removeIngredient(int)));
	
	setAllColumnsShowFocus(true);
	setDefaultRenameAction(QListView::Reject);
}

void IngredientListView::reload()
{
	clear();

	ElementList ingredientList;
	database->loadIngredients(&ingredientList);

	for ( ElementList::const_iterator ing_it = ingredientList.begin(); ing_it != ingredientList.end(); ++ing_it )
		createIngredient(*ing_it);
}



StdIngredientListView::StdIngredientListView( QWidget *parent, RecipeDB *db, bool editable ) : IngredientListView(parent,db)
{
	addColumn(i18n("Id"));
	addColumn(i18n("Ingredient"));

	if ( editable ) {
		setRenameable(1, true);
	
		connect(this,SIGNAL(doubleClicked( QListViewItem* )),this, SLOT(modIngredient( QListViewItem* )));
		connect(this,SIGNAL(itemRenamed(QListViewItem*)),this, SLOT(saveIngredient(QListViewItem*)));
	}
}

void StdIngredientListView::createIngredient(const Element &ing)
{
	(void)new QListViewItem(this,QString::number(ing.id),ing.name);
}

void StdIngredientListView::removeIngredient(int id)
{
	QListViewItem *item = findItem(QString::number(id),0);
	
	Q_ASSERT(item);
	
	delete item;
}

void StdIngredientListView::modIngredient(QListViewItem* i)
{
	rename(i, 1);
}

void StdIngredientListView::saveIngredient(QListViewItem* i)
{
int existing_id = database->findExistingIngredientByName( i->text(1) );
int ing_id = i->text(0).toInt();
if ( existing_id != -1 && existing_id != ing_id ) //category already exists with this label... merge the two
{  
  switch (KMessageBox::warningContinueCancel(this,i18n("This ingredient already exists.  Continuing will merge these two ingredients into one.  Are you sure?")))
  {
  case KMessageBox::Continue:
  {
  	database->mergeIngredients(existing_id,ing_id);
  	break;
  }
  default: reload(); break;
  }
}
else
{
  database->modIngredient((i->text(0)).toInt(), i->text(1));
}
}



IngredientCheckListView::IngredientCheckListView( QWidget *parent, RecipeDB *db ) : IngredientListView(parent,db)
{
	addColumn("*");
	addColumn(i18n("Ingredient"));
	addColumn(i18n("Id"));
}

void IngredientCheckListView::createIngredient(const Element &ing)
{
	new IngredientCheckListItem(this,ing);
}

void IngredientCheckListView::removeIngredient(int id)
{
	QListViewItem *item = findItem(QString::number(id),2);
	
	Q_ASSERT(item);
	
	delete item;
}

#include "ingredientlistview.moc"
