/***************************************************************************
 *   Copyright (C) 2004 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
 
#include "prepmethodlistview.h"

#include <kmessagebox.h>

#include "DBBackend/recipedb.h"
 
PrepMethodListView::PrepMethodListView( QWidget *parent, RecipeDB *db ) : KListView(parent),
  database(db)
{
	connect(database,SIGNAL(prepMethodCreated(const Element &)),SLOT(createPrepMethod(const Element &)));
	connect(database,SIGNAL(prepMethodRemoved(int)),SLOT(removePrepMethod(int)));
	
	setAllColumnsShowFocus(true);
	setDefaultRenameAction(QListView::Reject);
}

void PrepMethodListView::reload()
{
	clear();

	ElementList prepMethodList;
	database->loadPrepMethods(&prepMethodList);

	for ( ElementList::const_iterator ing_it = prepMethodList.begin(); ing_it != prepMethodList.end(); ++ing_it )
		createPrepMethod(*ing_it);
}



StdPrepMethodListView::StdPrepMethodListView( QWidget *parent, RecipeDB *db, bool editable ) : PrepMethodListView(parent,db)
{
	addColumn(i18n("Id"));
	addColumn(i18n("Preparation Method"));

	if ( editable ) {
		setRenameable(1, true);
	
		connect(this,SIGNAL(doubleClicked( QListViewItem* )),this, SLOT(modPrepMethod( QListViewItem* )));
		connect(this,SIGNAL(itemRenamed(QListViewItem*)),this, SLOT(savePrepMethod(QListViewItem*)));
	}
}

void StdPrepMethodListView::createPrepMethod(const Element &ing)
{
	(void)new QListViewItem(this,QString::number(ing.id),ing.name);
}

void StdPrepMethodListView::removePrepMethod(int id)
{
	QListViewItem *item = findItem(QString::number(id),0);
	
	Q_ASSERT(item);
	
	delete item;
}

void StdPrepMethodListView::modPrepMethod(QListViewItem* i)
{
	rename(i, 1);
}

void StdPrepMethodListView::savePrepMethod(QListViewItem* i)
{
int existing_id = database->findExistingPrepByName( i->text(1) );
int prep_id = i->text(0).toInt();
if ( existing_id != -1 && existing_id != prep_id ) //category already exists with this label... merge the two
{  
  switch (KMessageBox::warningContinueCancel(this,i18n("This preparation method already exists.  Continuing will merge these two into one.  Are you sure?")))
  {
  case KMessageBox::Continue:
  {
  	database->mergePrepMethods(existing_id,prep_id);
  	break;
  }
  default: reload(); break;
  }
}
else
{
  database->modPrepMethod((i->text(0)).toInt(), i->text(1));
}
}

#include "prepmethodlistview.moc"
