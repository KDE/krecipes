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
#include <kconfig.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kpopupmenu.h>

#include "DBBackend/recipedb.h"
#include "dialogs/createelementdialog.h"
 
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
	KConfig *config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry("ShowID",false);
	addColumn( i18n("Id"), show_id ? -1 : 0 );

	addColumn(i18n("Ingredient"));

	if ( editable ) {
		setRenameable(1, true);
	
		connect(this,SIGNAL(contextMenu(KListView *, QListViewItem *, const QPoint &)), SLOT(showPopup(KListView *, QListViewItem *, const QPoint &)));
		connect(this,SIGNAL(doubleClicked( QListViewItem* )),this, SLOT(modIngredient( QListViewItem* )));
		connect(this,SIGNAL(itemRenamed(QListViewItem*)),this, SLOT(saveIngredient(QListViewItem*)));
	}
}

void StdIngredientListView::showPopup(KListView */*l*/, QListViewItem *i, const QPoint &p)
{
	if ( i )
		kpop->exec(p);
}

void StdIngredientListView::createNew()
{
	CreateElementDialog* elementDialog=new CreateElementDialog(this,i18n("New Ingredient"));
	
	if ( elementDialog->exec() == QDialog::Accepted ) {
		QString result = elementDialog->newElementName();
		database->createNewIngredient(result); // Create the new author in the database
	}
}

void StdIngredientListView::remove()
{
	QListViewItem *item = currentItem();

	if ( item )
	{
		switch (KMessageBox::warningContinueCancel(this,i18n("Are you sure you want to remove this ingredient?")))
		{
		case KMessageBox::Continue: database->removeIngredient(item->text(0).toInt()); break;
		default: break;
		}
	}
}

void StdIngredientListView::rename()
{
	QListViewItem *item = currentItem();
	
	if ( item )
		IngredientListView::rename( item, 1 );
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
	IngredientListView::rename(i, 1);
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

	KConfig *config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry("ShowID",false);
	addColumn( i18n("Id"), show_id ? -1 : 0 );
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
