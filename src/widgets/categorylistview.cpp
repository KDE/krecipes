/***************************************************************************
 *   Copyright (C) 2004 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "categorylistview.h"

#include <klocale.h>
#include <kmessagebox.h>

#include "DBBackend/recipedb.h"
#include "datablocks/categorytree.h"

CategoryCheckListItem::CategoryCheckListItem(QListView* klv, const Element &category ) : QCheckListItem(klv,QString::null,QCheckListItem::CheckBox),
	ctyStored(category)
{
	setOn(false); // Set unchecked by default
}

CategoryCheckListItem::CategoryCheckListItem(QListViewItem* it, const Element &category ) : QCheckListItem(it,QString::null,QCheckListItem::CheckBox),
	ctyStored(category)
{
	setOn(false); // Set unchecked by default
}

QString CategoryCheckListItem::text(int column) const
{
	if (column==1) return(QString::number(ctyStored.id));
	else return(ctyStored.name);
}

void CategoryCheckListItem::setText(int column, const QString &text)
{
	switch (column)
	{
	case 0: ctyStored.name = text; break;
	default: break;
	}
}

void CategoryCheckListItem::stateChange(bool on)
{
	if ( on )
	{
		CategoryCheckListItem *cat_it;
		for ( cat_it = (CategoryCheckListItem*)parent(); cat_it; cat_it = (CategoryCheckListItem*)cat_it->parent() )
			cat_it->setOn(false);

		//do this to only iterate over this item's children
		QListViewItem *pEndItem = NULL;
		QListViewItem *pStartItem = this;
		do
		{
			if(pStartItem->nextSibling())
				pEndItem = pStartItem->nextSibling();
			else
				pStartItem = pStartItem->parent();
		}
		while(pStartItem && !pEndItem);

		QListViewItemIterator it( this );
		while ( it.current() && it.current() != pEndItem ) {
			cat_it = (CategoryCheckListItem*)it.current();
			if ( cat_it != this ) 
				cat_it->setOn(false);
			++it;
		}
	}
}


CategoryListView::CategoryListView( QWidget *parent, RecipeDB *db ) : KListView(parent),
  database(db)
{
	connect(db,SIGNAL(categoryCreated(const Element &,int)),SLOT(createCategory(const Element &,int)));
	connect(db,SIGNAL(categoryRemoved(int)),SLOT(removeCategory(int)));
	connect(db,SIGNAL(categoryModified(const Element &)),SLOT(modifyCategory(const Element &)));
	connect(db,SIGNAL(categoryModified(int,int)),SLOT(modifyCategory(int,int)));

	setRootIsDecorated(true);
	setAllColumnsShowFocus(true);
	setDefaultRenameAction(QListView::Reject);
}

// (Re)loads the data from the database
void CategoryListView::reload(void)
{
//Clear the listview first
clear();
CategoryTree categoryTree;
database->loadCategories(&categoryTree);
loadListView(&categoryTree);
}

void CategoryListView::loadListView(const CategoryTree *categoryTree, int parent_id )
{
	const CategoryTreeChildren *children = categoryTree->children();
	for ( CategoryTreeChildren::const_iterator child_it = children->begin(); child_it != children->end(); ++child_it )
	{
		const CategoryTree *node = *child_it;

		createCategory(node->category,parent_id);

		loadListView( node, node->category.id );
	}
}



StdCategoryListView::StdCategoryListView( QWidget *parent, RecipeDB *db, bool editable ) : CategoryListView(parent,db)
{
	addColumn(i18n("Category"));
	addColumn(i18n("Id"));

	if ( editable )
	{
		setRenameable(0, true);
		setDragEnabled(true);
		setAcceptDrops(true);

		connect(this,SIGNAL(doubleClicked( QListViewItem*,const QPoint &, int )), SLOT(modCategory( QListViewItem* )));
		connect(this,SIGNAL(itemRenamed (QListViewItem*)), SLOT(saveCategory( QListViewItem* )));
		connect(this,SIGNAL(moved(QListViewItem *,QListViewItem *,QListViewItem *)), SLOT(changeCategoryParent(QListViewItem *,QListViewItem *,QListViewItem *)));
	}
}

void StdCategoryListView::changeCategoryParent(QListViewItem *item,QListViewItem */*afterFirst*/,QListViewItem */*afterNow*/)
{
	int new_parent_id = -1;
	if ( QListViewItem *parent = item->parent() )
		new_parent_id = parent->text(1).toInt();

	int cat_id = item->text(1).toInt();
	
	disconnect(SIGNAL(moved(QListViewItem *,QListViewItem *,QListViewItem *)));
	database->modCategory( cat_id, new_parent_id );
	connect(this,SIGNAL(moved(QListViewItem *,QListViewItem *,QListViewItem *)), SLOT(changeCategoryParent(QListViewItem *,QListViewItem *,QListViewItem *)));

}

void StdCategoryListView::removeCategory(int id)
{
	QListViewItem *item = findItem(QString::number(id),1);
	
	//Q_ASSERT(item);
	
	delete item;
}

void StdCategoryListView::createCategory(const Element &category, int parent_id)
{
	QListViewItem *new_item;
	if ( parent_id == -1 )
		new_item = new QListViewItem(this,category.name,QString::number(category.id));
	else
	{
		QListViewItem *parent = findItem(QString::number(parent_id),1);

		Q_ASSERT(parent);

		new_item = new QListViewItem(parent,category.name,QString::number(category.id));
	}

	new_item->setOpen(true);
}

void StdCategoryListView::modifyCategory(const Element &category)
{
	QListViewItem *item = findItem(QString::number(category.id),1);

	Q_ASSERT(item);

	item->setText(0,category.name);
}

void StdCategoryListView::modifyCategory(int id, int parent_id)
{
	QListViewItem *item = findItem(QString::number(id),1);
	if ( !item->parent() )
		takeItem(item);
	else
		item->parent()->takeItem(item);
	
	Q_ASSERT(item);

	if ( parent_id == -1 )
		insertItem(item);
	else
		findItem(QString::number(parent_id),1)->insertItem(item);
}

void StdCategoryListView::modCategory(QListViewItem* i)
{
	rename(i, 0);
}

void StdCategoryListView::saveCategory(QListViewItem* i)
{
	int existing_id = database->findExistingCategoryByName( i->text(0) );
	int cat_id = i->text(1).toInt();
	if ( existing_id != -1 && existing_id != cat_id ) //category already exists with this label... merge the two
	{  
		switch (KMessageBox::warningContinueCancel(this,i18n("This category already exists.  Continuing will merge these two categories into one.  Are you sure?")))
		{
		case KMessageBox::Continue:
		{
			database->mergeCategories(existing_id,cat_id);
			break;
		}
		default: reload(); break;
		}
	}
	else
		database->modCategory(cat_id, i->text(0));
}



CategoryCheckListView::CategoryCheckListView( QWidget *parent, RecipeDB *db ) : CategoryListView(parent,db)
{
	addColumn(i18n("Category"));
	addColumn(i18n("Id"));
}

void CategoryCheckListView::removeCategory(int id)
{
	QListViewItem *item = findItem(QString::number(id),1);
	
	//Q_ASSERT(item);
	
	delete item;
}

void CategoryCheckListView::createCategory(const Element &category, int parent_id)
{
	CategoryCheckListItem *new_item;
	if ( parent_id == -1 )
		new_item = new CategoryCheckListItem(this,category);
	else
	{
		QListViewItem *parent = findItem(QString::number(parent_id),1);

		Q_ASSERT(parent);

		new_item = new CategoryCheckListItem(parent,category);
	}

	new_item->setOpen(true);
}

void CategoryCheckListView::modifyCategory(const Element &category)
{
	QListViewItem *item = findItem(QString::number(category.id),1);

	Q_ASSERT(item);

	item->setText(0,category.name);
}

void CategoryCheckListView::modifyCategory(int id, int parent_id)
{
	QListViewItem *item = findItem(QString::number(id),1);
	if ( !item->parent() )
		takeItem(item);
	else
		item->parent()->takeItem(item);
	
	Q_ASSERT(item);

	if ( parent_id == -1 )
		insertItem(item);
	else
		findItem(QString::number(parent_id),1)->insertItem(item);
}

#include "categorylistview.moc"
