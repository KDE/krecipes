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

#include "categorieseditordialog.h"

#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>

#include "createelementdialog.h"
#include "DBBackend/recipedb.h"

CategoriesEditorDialog::CategoriesEditorDialog(QWidget* parent, RecipeDB *db):QWidget(parent)
{

// Store pointer to database
database=db;

// Top & left spacers
layout = new QGridLayout(this);
QSpacerItem *spacerTop=new QSpacerItem(10,10,QSizePolicy::Minimum,QSizePolicy::Fixed);
layout->addItem(spacerTop,0,1);
QSpacerItem *spacerLeft=new QSpacerItem(10,10,QSizePolicy::Fixed,QSizePolicy::Minimum);
layout->addItem(spacerLeft,1,0);

//Category List
categoryListView=new KListView(this);
categoryListView->addColumn(i18n("Category Name"));
categoryListView->addColumn(i18n("Id"));
categoryListView->setRenameable(0, true);
categoryListView->setDefaultRenameAction(QListView::Reject);
categoryListView->setAllColumnsShowFocus(true);
categoryListView->setRootIsDecorated(true);
categoryListView->setDragEnabled(true);
categoryListView->setAcceptDrops(true);
layout->addWidget(categoryListView,1,1);

//Buttons
buttonBar=new QHBox(this);
layout->addWidget(buttonBar,2,1);

il=new KIconLoader;

newCategoryButton=new QPushButton(buttonBar);
newCategoryButton->setText(i18n("Create New Category"));
QPixmap pm=il->loadIcon("fileNew2", KIcon::NoGroup,16); newCategoryButton->setIconSet(pm);

removeCategoryButton=new QPushButton(buttonBar);
removeCategoryButton->setText(i18n("Remove"));
pm=il->loadIcon("editshred", KIcon::NoGroup,16); removeCategoryButton->setIconSet(pm);
removeCategoryButton->setMaximumWidth(100);

//Load the data from the database
reload();

//Connect Signals & Slots

connect (newCategoryButton,SIGNAL(clicked()),this,SLOT(createNewCategory()));
connect (removeCategoryButton,SIGNAL(clicked()),this,SLOT(removeCategory()));
connect(this->categoryListView,SIGNAL(doubleClicked( QListViewItem*,const QPoint &, int )),this, SLOT(modCategory( QListViewItem* )));
connect(this->categoryListView,SIGNAL(itemRenamed (QListViewItem*)),this, SLOT(saveCategory( QListViewItem* )));
connect(this->categoryListView,SIGNAL(moved(QListViewItem *,QListViewItem *,QListViewItem *)),SLOT(changeCategoryParent(QListViewItem *,QListViewItem *,QListViewItem *)));
}

CategoriesEditorDialog::~CategoriesEditorDialog()
{
}

void CategoriesEditorDialog::changeCategoryParent(QListViewItem *item,QListViewItem */*afterFirst*/,QListViewItem */*afterNow*/)
{
	int new_parent_id = -1;
	if ( QListViewItem *parent = item->parent() )
		new_parent_id = parent->text(1).toInt();

	int cat_id = item->text(1).toInt();
	database->modCategory( cat_id, new_parent_id );
}

// (Re)loads the data from the database
void CategoriesEditorDialog::reload(void)
{

//Clear the listview first
categoryListView->clear();
CategoryTree categoryTree;
database->loadCategories(&categoryTree);
loadListView(&categoryTree);
}

void CategoriesEditorDialog::loadListView(const CategoryTree *categoryTree, QListViewItem *parent )
{
	const CategoryTreeChildren *children = categoryTree->children();
	for ( CategoryTreeChildren::const_iterator child_it = children->begin(); child_it != children->end(); ++child_it )
	{
		const CategoryTree *node = *child_it;
		
		QListViewItem *new_item;
		if ( parent == 0 )
			new_item = new QListViewItem(categoryListView,node->category.name,QString::number(node->category.id));
		else
			new_item = new QListViewItem(parent,node->category.name,QString::number(node->category.id));

		new_item->setOpen(true);
		loadListView( node, new_item );
	}
}

void CategoriesEditorDialog::createNewCategory(void)
{
CreateElementDialog* elementDialog=new CreateElementDialog(this,i18n("New Category"));

if ( elementDialog->exec() == QDialog::Accepted ) {
   QString result = elementDialog->newElementName();
   
   int parent_id = -1;
   if ( QListViewItem *current = categoryListView->selectedItem() )
   	parent_id = current->text(1).toInt();
   database->createNewCategory(result,parent_id); // Create the new category in the database

   reload(); // Reload the list from the database
}
delete elementDialog;
}

void CategoriesEditorDialog::removeCategory(void)
{
// Find the selected category item
QListViewItem *it;
int categoryID=-1;

if ( (it=categoryListView->selectedItem()) ) categoryID=it->text(1).toInt();

if (categoryID>=0) // a category was selected previously
{
database->removeCategory(categoryID);
}

reload();// Reload the list from the database

}

void CategoriesEditorDialog::modCategory(QListViewItem* i)
{
  newCategoryButton->setEnabled(false);
  removeCategoryButton->setEnabled(false);
  categoryListView->rename(i, 0);
}

void CategoriesEditorDialog::saveCategory(QListViewItem* i)
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
  	delete i;
  	break;
  }
  default: reload(); break;
  }
}
else
{
  database->modCategory(cat_id, i->text(0));
}

newCategoryButton->setEnabled(true);
removeCategoryButton->setEnabled(true);
}

#include "categorieseditordialog.moc"
