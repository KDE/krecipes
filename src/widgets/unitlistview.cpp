/***************************************************************************
 *   Copyright (C) 2004 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
 
#include "unitlistview.h"

#include <kmessagebox.h>

#include "DBBackend/recipedb.h"
 
UnitListView::UnitListView( QWidget *parent, RecipeDB *db ) : KListView(parent),
  database(db)
{
	connect(database,SIGNAL(unitCreated(const Element &)),SLOT(createUnit(const Element &)));
	connect(database,SIGNAL(unitRemoved(int)),SLOT(removeUnit(int)));

	setAllColumnsShowFocus(true);
	setDefaultRenameAction(QListView::Reject);
}

void UnitListView::reload()
{
	clear();

	ElementList unitList;
	database->loadUnits(&unitList);

	for ( ElementList::const_iterator ing_it = unitList.begin(); ing_it != unitList.end(); ++ing_it )
		createUnit(*ing_it);
}



StdUnitListView::StdUnitListView( QWidget *parent, RecipeDB *db, bool editable ) : UnitListView(parent,db)
{
	addColumn(i18n("Id"));
	addColumn(i18n("Unit"));

	if ( editable ) {
		setRenameable(1, true);
	
		connect(this,SIGNAL(doubleClicked( QListViewItem* )),this, SLOT(modUnit( QListViewItem* )));
		connect(this,SIGNAL(itemRenamed(QListViewItem*)),this, SLOT(saveUnit(QListViewItem*)));
	}
}

void StdUnitListView::createUnit(const Element &ing)
{
	(void)new QListViewItem(this,QString::number(ing.id),ing.name);
}

void StdUnitListView::removeUnit(int id)
{
	QListViewItem *item = findItem(QString::number(id),0);
	
	Q_ASSERT(item);
	
	delete item;
}

void StdUnitListView::modUnit(QListViewItem* i)
{
	rename(i, 1);
}

void StdUnitListView::saveUnit(QListViewItem* i)
{
	int existing_id = database->findExistingUnitByName( i->text(1) );
	int unit_id = i->text(0).toInt();
	if ( existing_id != -1 && existing_id != unit_id ) { //category already exists with this label... merge the two
		switch (KMessageBox::warningContinueCancel(this,i18n("This unit already exists.  Continuing will merge these two units into one.  Are you sure?")))
		{
		case KMessageBox::Continue:
		{
			database->mergeUnits(existing_id,unit_id);
			break;
		}
		default: reload(); break;
		}
	}
	else {
		database->modUnit((i->text(0)).toInt(), i->text(1));
	}
}

#include "unitlistview.moc"
