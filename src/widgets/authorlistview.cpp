/***************************************************************************
 *   Copyright (C) 2004 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
 
#include "authorlistview.h"

#include <kmessagebox.h>

#include "DBBackend/recipedb.h"
 
AuthorListView::AuthorListView( QWidget *parent, RecipeDB *db ) : KListView(parent),
  database(db)
{
	connect(database,SIGNAL(authorCreated(const Element &)),SLOT(createAuthor(const Element &)));
	connect(database,SIGNAL(authorRemoved(int)),SLOT(removeAuthor(int)));
	
	setAllColumnsShowFocus(true);
	setDefaultRenameAction(QListView::Reject);
}

void AuthorListView::reload()
{
	clear();

	ElementList authorList;
	database->loadAuthors(&authorList);

	for ( ElementList::const_iterator ing_it = authorList.begin(); ing_it != authorList.end(); ++ing_it )
		createAuthor(*ing_it);
}



StdAuthorListView::StdAuthorListView( QWidget *parent, RecipeDB *db, bool editable ) : AuthorListView(parent,db)
{
	addColumn(i18n("Id"));
	addColumn(i18n("Author"));

	if ( editable ) {
		setRenameable(1, true);
	
		connect(this,SIGNAL(doubleClicked( QListViewItem* )),this, SLOT(modAuthor( QListViewItem* )));
		connect(this,SIGNAL(itemRenamed(QListViewItem*)),this, SLOT(saveAuthor(QListViewItem*)));
	}
}

void StdAuthorListView::createAuthor(const Element &ing)
{
	(void)new QListViewItem(this,QString::number(ing.id),ing.name);
}

void StdAuthorListView::removeAuthor(int id)
{
	QListViewItem *item = findItem(QString::number(id),0);
	
	Q_ASSERT(item);
	
	delete item;
}

void StdAuthorListView::modAuthor(QListViewItem* i)
{
	rename(i, 1);
}

void StdAuthorListView::saveAuthor(QListViewItem* i)
{
int existing_id = database->findExistingAuthorByName( i->text(1) );
int author_id = i->text(0).toInt();
if ( existing_id != -1 && existing_id != author_id ) //category already exists with this label... merge the two
{  
  switch (KMessageBox::warningContinueCancel(this,i18n("This author already exists.  Continuing will merge these two authors into one.  Are you sure?")))
  {
  case KMessageBox::Continue:
  {
  	database->mergeAuthors(existing_id,author_id);
  	break;
  }
  default: reload(); break;
  }
}
else
{
  database->modAuthor((i->text(0)).toInt(), i->text(1));
}
}

#include "authorlistview.moc"
