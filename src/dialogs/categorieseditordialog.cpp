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

#include "widgets/categorylistview.h"
#include "createcategorydialog.h"
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
categoryListView=new StdCategoryListView(this,database,true);
categoryListView->reload();
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

//Connect Signals & Slots

connect (newCategoryButton,SIGNAL(clicked()),this,SLOT(createNewCategory()));
connect (removeCategoryButton,SIGNAL(clicked()),this,SLOT(removeCategory()));
}

CategoriesEditorDialog::~CategoriesEditorDialog()
{
}

void CategoriesEditorDialog::createNewCategory(void)
{
ElementList categories; database->loadCategories(&categories);
CreateCategoryDialog* categoryDialog=new CreateCategoryDialog(this,categories);

if ( categoryDialog->exec() == QDialog::Accepted ) {
   QString result = categoryDialog->newCategoryName();
   int subcategory = categoryDialog->subcategory();
   database->createNewCategory(result,subcategory); // Create the new category in the database
}
delete categoryDialog;
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

}

#include "categorieseditordialog.moc"
