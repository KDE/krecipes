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

#include "selectrecipedialog.h"

#include <qsignalmapper.h>
#include <klocale.h>

#include "DBBackend/recipedb.h"
#include "recipe.h"
#include "selectunitdialog.h"
#include "createelementdialog.h"

SelectRecipeDialog::SelectRecipeDialog(QWidget *parent, RecipeDB* db)
 : QWidget(parent)
{
//Store pointer to Recipe Database
database=db;

//Initialize internal data
recipeList=new ElementList;
categoryList=new ElementList;
//Design dialog

layout = new QGridLayout( this, 1, 1, 0, 0);

	// Border Spacers
	QSpacerItem* spacer_left = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );	layout->addMultiCell( spacer_left, 1,4,0,0 );
	QSpacerItem* spacer_top = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addMultiCell(spacer_top,0,0,1,4);

	searchBar=new QHBox(this);
	layout->addWidget(searchBar,1,1);

	searchLabel=new QLabel(searchBar); searchLabel->setText(i18n("Search:")); searchLabel->setFixedWidth(searchLabel->fontMetrics().width(i18n("Search:"))+5);
	searchBox=new KLineEdit(searchBar);

	QSpacerItem* searchSpacer=new QSpacerItem(10,10,QSizePolicy::Fixed,QSizePolicy::Minimum); layout->addItem(searchSpacer,1,2);
	categoryBox=new KComboBox(this);
	layout->addWidget(categoryBox,1,3);


	QSpacerItem* spacerFromSearchBar = new QSpacerItem(10,10,QSizePolicy::Minimum, QSizePolicy::Fixed);
    	layout->addItem(spacerFromSearchBar,2,1);

	il=new KIconLoader;
	recipeListView=new KListView(this);
	recipeListView->addColumn(i18n("Category"));
    	recipeListView->addColumn(i18n("Id"));
    	recipeListView->addColumn(i18n("Title"));
    	recipeListView->setGeometry( QRect( 10, 65, 190, 280 ) );
	recipeListView->setRootIsDecorated(true); // Show "+" open icons
	layout->addMultiCellWidget(recipeListView,3,3,1,3);

	buttonBar=new QHBox(this);
 	layout->addMultiCellWidget(buttonBar,4,4,1,3);

	openButton=new QPushButton(buttonBar);
	openButton->setText(i18n("Open Recipe"));
  openButton->setDisabled(true);
	QPixmap pm=il->loadIcon("ok", KIcon::NoGroup,16); openButton->setIconSet(pm);
	editButton=new QPushButton(buttonBar);
	editButton->setText(i18n("Edit Recipe"));
  editButton->setDisabled(true);
	pm=il->loadIcon("edit", KIcon::NoGroup,16); editButton->setIconSet(pm);
	removeButton=new QPushButton(buttonBar);
	removeButton->setText(i18n("Delete"));
  removeButton->setDisabled(true);
	removeButton->setMaximumWidth(100);
	pm=il->loadIcon("editshred", KIcon::NoGroup,16); removeButton->setIconSet(pm);

// Popup menus
    kpop = new KPopupMenu( recipeListView );
    kpop->insertItem( il->loadIcon("ok", KIcon::NoGroup,16),tr2i18n("&Open"), this, SLOT(open()), CTRL+Key_L );
    kpop->insertItem( il->loadIcon("edit", KIcon::NoGroup,16),tr2i18n("&Edit"), this, SLOT(edit()), CTRL+Key_E );
    kpop->insertItem( il->loadIcon("filesaveas", KIcon::NoGroup,16),tr2i18n("&Save as"), this, SLOT(exportRecipe()), CTRL+Key_S );
    kpop->insertItem( il->loadIcon("editshred", KIcon::NoGroup,16),tr2i18n("Remove from &Category"), this, SLOT(removeFromCat()), CTRL+Key_C );
    kpop->insertItem( il->loadIcon("editshred", KIcon::NoGroup,16),tr2i18n("&Remove"), this, SLOT(remove()), CTRL+Key_R );
    kpop->polish();

    catPop = new KPopupMenu( recipeListView );
    catPop->insertItem( il->loadIcon("filesaveas", KIcon::NoGroup,16),tr2i18n("&Save as"), this, SLOT(exportRecipeFromCat()), CTRL+Key_S );

// Load Recipe List
loadRecipeList();
loadCategoryCombo();

// Initialize some internal variables
isFilteringCategories=false;

// Signals & Slots

connect(openButton,SIGNAL(clicked()),this, SLOT(open()));
connect(this,SIGNAL(recipeSelected(bool)),openButton, SLOT(setEnabled(bool)));
connect(editButton,SIGNAL(clicked()),this, SLOT(edit()));
connect(this,SIGNAL(recipeSelected(bool)),editButton, SLOT(setEnabled(bool)));
connect(removeButton,SIGNAL(clicked()),this, SLOT(remove()));
connect(this,SIGNAL(recipeSelected(bool)),removeButton, SLOT(setEnabled(bool)));
connect(searchBox,SIGNAL(returnPressed(const QString&)),this,SLOT(filter(const QString&)));
connect(searchBox,SIGNAL(textChanged(const QString&)),this,SLOT(filter(const QString&)));
connect(recipeListView,SIGNAL(selectionChanged()),this, SLOT(haveSelectedItems()));
connect(recipeListView,SIGNAL(doubleClicked( QListViewItem*,const QPoint &, int )),this, SLOT(open()));
connect(recipeListView,SIGNAL(contextMenu (KListView *, QListViewItem *, const QPoint &)),this, SLOT(showPopup(KListView *, QListViewItem *, const QPoint &)));
connect(categoryBox,SIGNAL(activated(int)),this,SLOT(filterComboCategory(int)));
}


SelectRecipeDialog::~SelectRecipeDialog()
{
}

void SelectRecipeDialog::showEvent(QShowEvent* e){
  if(!e->spontaneous()){
    openButton->setEnabled(false);
    editButton->setEnabled(false);
    removeButton->setEnabled(false);
  }
}

void SelectRecipeDialog::loadRecipeList(void)
{
recipeListView->clear();
recipeList->clear();
categoryList->clear();

// First show the categories

ElementList categoryList;

database->loadCategories(&categoryList);

for ( Element *category=categoryList.getFirst(); category; category=categoryList.getNext())
	{
	QListViewItem *it=new QListViewItem(recipeListView,category->name,"","");
	categoryItems.insert(category->id,it);
	}


// Now show the recipes

int *categoryID;
Element *recipe;
QPtrList <int> recipeCategoryList;


database->loadRecipeList(recipeList,0,&recipeCategoryList); // Read the whole list of recipes including category

for ( recipe=recipeList->getFirst(),categoryID=recipeCategoryList.first();(recipe && categoryID);recipe=recipeList->getNext(),categoryID=recipeCategoryList.next())
	{
	if (QListViewItem* categoryItem=categoryItems[*categoryID])
	{
	QListViewItem *it=new QListViewItem (categoryItem,"",QString::number(recipe->id),recipe->name,"");
	}
	else
	{
	QListViewItem *it=new QListViewItem (recipeListView,"...",QString::number(recipe->id),recipe->name);
	}
	}



filter(searchBox->text());

}

void SelectRecipeDialog::open(void)
{
QListViewItem *it;
it=recipeListView->selectedItem();
if ( it != 0 && !it->firstChild() && it->text(1).toInt() != NULL) emit recipeSelected(it->text(1).toInt(),0);
}

void SelectRecipeDialog::edit(void)
{
QListViewItem *it;
it=recipeListView->selectedItem();
if ( it != 0 && !it->firstChild() && it->text(1).toInt() != NULL) emit recipeSelected(it->text(1).toInt(),1);
}

void SelectRecipeDialog::remove(void)
{
QListViewItem *it;
it=recipeListView->selectedItem();
if ( it != 0 && !it->firstChild() && it->text(1).toInt() != NULL) emit recipeSelected(it->text(1).toInt(),2);
}

void SelectRecipeDialog::removeFromCat(void)
{
  QListViewItem *it;
  it=recipeListView->selectedItem();
  if ( it != 0 && !it->firstChild() && it->text(1).toInt() != NULL){
    if(it->parent() != 0){
      int categoryID;
      categoryID = database->findExistingCategoryByName((it->parent())->text(0));
      database->removeRecipeFromCategory(it->text(1).toInt(), categoryID);
    }
    else{
      database->removeRecipeFromCategory(it->text(1).toInt(), -1);
    }
    reload();
  }
}

void SelectRecipeDialog::reload()
{
loadRecipeList();
loadCategoryCombo();
}

void SelectRecipeDialog::filter(const QString& s)
{
for (QListViewItem *it=recipeListView->firstChild();it;it=it->nextSibling())
	{
	if (!it->firstChild()) // It's not a category or it's empty
	{
		if (s.isNull()) it->setVisible(true); // Don't filter if the filter text is empty
		else if (it->text(2).contains(s,false)) it->setVisible(true);

		else it->setVisible(false);
	}
	else // It's a category. Check the children
	{
		for (QListViewItem *cit=it->firstChild();cit;cit=cit->nextSibling())
		{
		if (s==QString::null) cit->setVisible(true); // Don't filter if the filter text is empty

		else if (cit->text(2).contains(s,false)) {
								cit->setVisible(true);
								if (!isFilteringCategories) it->setOpen(true);
								}

		else cit->setVisible(false);

		}
	}


	}
}


void SelectRecipeDialog::filterCategories(int categoryID)
{
std::cerr<<"I got category :"<<categoryID<<"\n";
for (QListViewItem *it=recipeListView->firstChild();it;it=it->nextSibling())
	{
	if (categoryID==-1) it->setVisible(true); // We're not filtering categories
	else if (it!=categoryItems[categoryID]) it->setVisible(false);
	else it->setVisible(true);
	}

}




void SelectRecipeDialog::loadCategoryCombo(void)
{

ElementList categoryList;
database->loadCategories(&categoryList);

categoryBox->clear();
categoryComboRows.clear();

// Insert default "All Categories" (row 0, which will be translated to -1 as category in the filtering process)
categoryBox->insertItem(i18n("All Categories"));

//Now load the categories
int row=1;
for (Element *category=categoryList.getFirst();category;category=categoryList.getNext())
	{
	categoryBox->insertItem(category->name);
	categoryComboRows.insert(row,&category->id); // store category id's in the combobox position to obtain the category id later
	row++;
	}

}


/*!
    \fn SelectRecipeDialog::exportRecipe()
 */
void SelectRecipeDialog::exportRecipe()
{
  if((recipeListView->selectedItem())->text(1).toInt() != NULL){
    KFileDialog* fd = new KFileDialog((recipeListView->selectedItem())->text(2), "*.kre|Gzip Krecipes file (*.kre)\n*.kre|Krecipes xml file (*.kreml)", 0, "Save recipe", true);
    QString fileName = fd->getSaveFileName((recipeListView->selectedItem())->text(2), "*.kre|Gzip Krecipes file (*.kre)\n*.kre|Krecipes xml file (*.kreml)", 0, "Save recipe");
    if(fileName != NULL){
      KreExporter* kre = new KreExporter(database, fileName, fd->currentFilter());
      kre->exporter( (recipeListView->selectedItem())->text(1).toInt() );
    }
  }
}

void SelectRecipeDialog::exportRecipeFromCat()
{
  if((recipeListView->selectedItem())->text(1).toInt() == NULL){
    KFileDialog* fd = new KFileDialog((recipeListView->selectedItem())->text(0), "*.kre|Gzip Krecipes file (*.kre)\n*.kre|Krecipes xml file (*.kreml)", 0, "Save recipe", true);
    QString fileName = fd->getSaveFileName((recipeListView->selectedItem())->text(0), "*.kre|Gzip Krecipes file (*.kre)\n*.kre|Krecipes xml file (*.kreml)", 0, "Save recipe");
    if(fileName != NULL){
      KreExporter* kre = new KreExporter(database, fileName, fd->currentFilter());
      QValueList<int>* l = new QValueList<int>;
      for (QListViewItem *cit=(recipeListView->selectedItem())->firstChild();cit;cit=cit->nextSibling())
      {
        l->append(cit->text(1).toInt());
      }
      kre->categoryExporter(l);
      //kre->exporter( (recipeListView->selectedItem())->text(1).toInt() );
    }
  }
}

void SelectRecipeDialog::haveSelectedItems(){
  if( recipeListView->selectedItem() != 0 && !(recipeListView->selectedItem())->firstChild() && (recipeListView->selectedItem())->text(1).toInt() != NULL){
    emit recipeSelected(true);
  }
  else{
    emit recipeSelected(false);
  }
}

void SelectRecipeDialog::getCurrentRecipe( Recipe *recipe )
{
	if (recipeListView->selectedItem())
	{
		if((recipeListView->selectedItem())->text(1).toInt() != NULL)
			database->loadRecipe( recipe, (recipeListView->selectedItem())->text(1).toInt() );
	}
}

void SelectRecipeDialog::showPopup( KListView *l, QListViewItem *i, const QPoint &p ){
  if(!i->firstChild() && i->text(1).toInt() != NULL){
    kpop->exec(p);
  }
  else{
    catPop->exec(p);
  }
}

void SelectRecipeDialog::filterComboCategory(int row)
{
std::cerr<<"I got row "<<row<<"\n";

//First get the category ID corresponding to this combo row
int categoryID;
if (row) categoryID=*(categoryComboRows[row]);
else categoryID=-1; // No category filtering

//Now filter

filterCategories(categoryID); // if categoryID==-1 doesn't filter

// Indicate that we are filtering by category so that
// the rest of the trees are not opened while filtering recipes
if (row>=1)
{
// Open the corresponding category tree
categoryItems[categoryID]->setOpen(true);
isFilteringCategories=true;
}
else isFilteringCategories=false;

}
