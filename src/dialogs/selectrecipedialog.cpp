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
#include <qtabwidget.h>

#include <klocale.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kprogress.h>
#include <kmessagebox.h>
#include <kglobal.h>
#include <kconfig.h>

#include "advancedsearchdialog.h"
#include "datablocks/categorytree.h"
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
//categoryList=new ElementList;
currentCategory=0;

categoryComboRows.setAutoDelete(true);

QVBoxLayout *tabLayout = new QVBoxLayout( this );
QTabWidget *tabWidget = new QTabWidget( this );
tabWidget->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
tabLayout->addWidget(tabWidget);

QGroupBox *basicSearchTab =new QGroupBox(this);
basicSearchTab->setFrameStyle(QFrame::NoFrame);
basicSearchTab->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));

//Design dialog

layout = new QGridLayout( basicSearchTab, 1, 1, 0, 0);

	// Border Spacers
	QSpacerItem* spacer_left = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );	layout->addMultiCell( spacer_left, 1,4,0,0 );
	QSpacerItem* spacer_top = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addMultiCell(spacer_top,0,0,1,4);

	searchBar=new QHBox(basicSearchTab);
	layout->addWidget(searchBar,1,1);

	searchLabel=new QLabel(searchBar); searchLabel->setText(i18n("Search:")); searchLabel->setFixedWidth(searchLabel->fontMetrics().width(i18n("Search:"))+5);
	searchBox=new KLineEdit(searchBar);

	QSpacerItem* searchSpacer=new QSpacerItem(10,10,QSizePolicy::Fixed,QSizePolicy::Minimum); layout->addItem(searchSpacer,1,2);
	categoryBox=new KComboBox(basicSearchTab);
	layout->addWidget(categoryBox,1,3);


	QSpacerItem* spacerFromSearchBar = new QSpacerItem(10,10,QSizePolicy::Minimum, QSizePolicy::Fixed);
    	layout->addItem(spacerFromSearchBar,2,1);

	il=new KIconLoader;
	recipeListView=new KListView(basicSearchTab);
	recipeListView->addColumn(i18n("Category"));

	KConfig *config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry("ShowID",false);
	recipeListView->addColumn( i18n("Id"), show_id ? -1 : 0 );
	
    	recipeListView->addColumn(i18n("Title"));
    	recipeListView->setGeometry( QRect( 10, 65, 190, 280 ) );
	recipeListView->setRootIsDecorated(true); // Show "+" open icons
	recipeListView->setAllColumnsShowFocus(true);
	layout->addMultiCellWidget(recipeListView,3,3,1,3);

	buttonBar=new QHBox(basicSearchTab);
 	layout->addMultiCellWidget(buttonBar,4,4,1,3);

	openButton=new QPushButton(buttonBar);
	openButton->setText(i18n("Open Recipe(s)"));
  openButton->setDisabled(true);
	QPixmap pm=il->loadIcon("ok", KIcon::NoGroup,16); openButton->setIconSet(pm);
	editButton=new QPushButton(buttonBar);
	editButton->setText(i18n("Edit Recipe"));
  editButton->setDisabled(true);
	pm=il->loadIcon("edit", KIcon::NoGroup,16); editButton->setIconSet(pm);
	removeButton=new QPushButton(buttonBar);
	removeButton->setText(i18n("Remove"));
  removeButton->setDisabled(true);
	removeButton->setMaximumWidth(100);
	pm=il->loadIcon("editshred", KIcon::NoGroup,16); removeButton->setIconSet(pm);

tabWidget->insertTab( basicSearchTab, "Basic" );

advancedSearch = new AdvancedSearchDialog(this,database);
tabWidget->insertTab( advancedSearch, "Advanced" );

//Takes care of all recipe actions and provides a popup menu to 'recipeListView'
actionHandler = new RecipeActionsHandler( recipeListView, database, 2, 1, 0 );

// Load Recipe List
loadRecipeList();
loadCategoryCombo();

// Initialize some internal variables
isFilteringCategories=false;

// Signals & Slots

connect(openButton,SIGNAL(clicked()),actionHandler, SLOT(open()));
connect(this,SIGNAL(recipeSelected(bool)),openButton, SLOT(setEnabled(bool)));
connect(editButton,SIGNAL(clicked()),actionHandler, SLOT(edit()));
connect(this,SIGNAL(recipeSelected(bool)),editButton, SLOT(setEnabled(bool)));
connect(removeButton,SIGNAL(clicked()),actionHandler, SLOT(remove()));
connect(this,SIGNAL(recipeSelected(bool)),removeButton, SLOT(setEnabled(bool)));
connect(searchBox,SIGNAL(returnPressed(const QString&)),this,SLOT(filter(const QString&)));
connect(searchBox,SIGNAL(textChanged(const QString&)),this,SLOT(filter(const QString&)));
connect(recipeListView,SIGNAL(selectionChanged()),this, SLOT(haveSelectedItems()));
connect(categoryBox,SIGNAL(activated(int)),this,SLOT(filterComboCategory(int)));
connect(advancedSearch,SIGNAL(recipeSelected(int,int)),SIGNAL(recipeSelected(int,int)));
connect(actionHandler,SIGNAL(recipeSelected(int,int)),SIGNAL(recipeSelected(int,int)));
connect(actionHandler,SIGNAL(recipesSelected(const QValueList<int> &,int)),SIGNAL(recipesSelected(const QValueList<int> &,int)));
connect(actionHandler,SIGNAL(reloadNeeded()),SLOT(reload()));
}

SelectRecipeDialog::~SelectRecipeDialog()
{
	delete il;
}

void SelectRecipeDialog::showEvent(QShowEvent* e){
  if(!e->spontaneous()){
    openButton->setEnabled(false);
    editButton->setEnabled(false);
    removeButton->setEnabled(false);
    //collapseAll();
  }
}

void SelectRecipeDialog::loadRecipeList(void)
{
recipeListView->clear();
recipeList->clear();
//categoryList->clear();
categoryItems.clear();

// First show the categories
CategoryTree categoryTree;
database->loadCategories(&categoryTree);
loadListView(&categoryTree);

// Now show the recipes

QIntDict <bool> recipeCategorized; recipeCategorized.setAutoDelete(true); // it deletes the bools after finished
QPtrList <int> recipeCategoryList;

database->loadRecipeList(recipeList,0,&recipeCategoryList); // Read the whole list of recipes including category

int *categoryID;
ElementList::const_iterator recipe_it;
for ( recipe_it=recipeList->begin(),categoryID=recipeCategoryList.first();recipe_it != recipeList->end() && categoryID;++recipe_it,categoryID=recipeCategoryList.next() )
	{
	if (QListViewItem* categoryItem=categoryItems[*categoryID])
	{
	(void)new QListViewItem (categoryItem,"",QString::number((*recipe_it).id),(*recipe_it).name,"");
	bool* b=new bool; *b=true;recipeCategorized.insert((*recipe_it).id,b); // mark the recipe as categorized
	}
	}


// Add those recipes that have not been categorised in any categories
for ( recipe_it=recipeList->begin(),categoryID=recipeCategoryList.first();recipe_it != recipeList->end() && categoryID;++recipe_it,categoryID=recipeCategoryList.next() )
	{
	if (!recipeCategorized[(*recipe_it).id])
	{
	(void)new QListViewItem (recipeListView,"...",QString::number((*recipe_it).id),(*recipe_it).name);
	}
	}

filter(searchBox->text());

}

void SelectRecipeDialog::loadListView(const CategoryTree *categoryTree, QListViewItem *parent )
{
	const CategoryTreeChildren *children = categoryTree->children();
	for ( CategoryTreeChildren::const_iterator child_it = children->begin(); child_it != children->end(); ++child_it )
	{
		CategoryTree *node = *child_it;
		
		QListViewItem *new_item;
		if ( parent == 0 )
			new_item = new QListViewItem(recipeListView,node->category.name,"","");
		else
			new_item = new QListViewItem(parent,node->category.name,"","");

		categoryItems.insert(node->category.id,new_item);
		loadListView( node, new_item );
	}
}

void SelectRecipeDialog::reload()
{
currentCategory=0;
QString remember_cat_filter = categoryBox->currentText();

loadRecipeList();
loadCategoryCombo();

if ( categoryBox->listBox()->findItem(remember_cat_filter,Qt::ExactMatch) ) {
	categoryBox->setCurrentText(remember_cat_filter);
	filterComboCategory(categoryBox->currentItem());
}
}

bool SelectRecipeDialog::hideIfEmpty(QListViewItem *parent)
{
	QListViewItem *it;
	if ( parent == 0 )
		it = recipeListView->firstChild();
	else
		it = parent->firstChild();
	
	bool parent_should_show = false;
	for ( ; it; it = it->nextSibling() ) {
		if ( itemIsRecipe(it) && it->isVisible() ) {
				parent_should_show = true;
		}
		else {
			bool result = hideIfEmpty(it);
			if ( parent_should_show == false )
				parent_should_show = result;
		}
	}
	
	if ( parent && !itemIsRecipe(parent) )
	{
		if ( parent_should_show )
			parent->setOpen(true);
		parent->setVisible(parent_should_show);
	}
	return parent_should_show;
}

void SelectRecipeDialog::filter(const QString& s)
{
//do this to only iterate over children of 'currentCategory'
QListViewItem *pEndItem = NULL;
if ( currentCategory ) {
	QListViewItem *pStartItem = currentCategory;
	do
	{
		if(pStartItem->nextSibling())
			pEndItem = pStartItem->nextSibling();
		else
			pStartItem = pStartItem->parent();
	}
	while(pStartItem && !pEndItem);
}

//Re-show everything
QListViewItemIterator list_it;
if ( currentCategory ) list_it = QListViewItemIterator(currentCategory);
else list_it = QListViewItemIterator(recipeListView);
while ( list_it.current() != pEndItem ) {
	list_it.current()->setVisible(true);
	list_it++;
}

// Only filter if the filter text isn't empty
if ( !s.isEmpty() ) {
	QListViewItemIterator list_it( recipeListView );
	while ( QListViewItem *it = list_it.current() ) {
		if ( !it->firstChild() ) // It's not a category or it's empty
		{
			if (it->text(2).contains(s,false))
			{
				if ( currentCategory ) {
					if ( isParentOf( currentCategory, it ) )
						it->setVisible(true);
					else it->setVisible(false);
				}
				else it->setVisible(true);
			}
			else
				it->setVisible(false);
		}
		
		++list_it;
	}
	hideIfEmpty();
}
}

void SelectRecipeDialog::filterCategories(int categoryID)
{
kdDebug()<<"I got category :"<<categoryID<<"\n";

if ( categoryID == -1 )
	currentCategory=0;
else
	currentCategory=categoryItems[categoryID];

QListViewItemIterator list_it( recipeListView );
while ( QListViewItem *it = list_it.current() ) {
	if (categoryID==-1) it->setVisible(true); // We're not filtering categories
	else if (it==categoryItems[categoryID] || isParentOf(it,categoryItems[categoryID]) || isParentOf(categoryItems[categoryID],it)) it->setVisible(true);
	else it->setVisible(false);

	++list_it;
}
}

bool SelectRecipeDialog::isParentOf(QListViewItem *parent, QListViewItem *to_check)
{
	for ( QListViewItem *it = to_check->parent(); it; it = it->parent() )
	{
		if ( it == parent )
			return true;
	}
	
	return false;
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
for ( ElementList::const_iterator cat_it = categoryList.begin(); cat_it != categoryList.end(); ++cat_it )
	{
	categoryBox->insertItem((*cat_it).name);
	categoryComboRows.insert(row,new int((*cat_it).id)); // store category id's in the combobox position to obtain the category id later
	row++;
	}

}

void SelectRecipeDialog::haveSelectedItems()
{
	if( recipeListView->selectedItem() )
	{
		if ( itemIsRecipe(recipeListView->selectedItem()) || recipeListView->selectedItem()->firstChild() )
			emit recipeSelected(true);
		else
			emit recipeSelected(false);
	}
}

void SelectRecipeDialog::getCurrentRecipe( Recipe *recipe )
{
	if (recipeListView->selectedItem() && itemIsRecipe(recipeListView->selectedItem()) )
		database->loadRecipe( recipe, (recipeListView->selectedItem())->text(1).toInt() );
}

void SelectRecipeDialog::filterComboCategory(int row)
{
kdDebug()<<"I got row "<<row<<"\n";

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

filter(searchBox->text());
}

//item is a recipe if the 2nd column is an integer (the recipe's ID)
bool SelectRecipeDialog::itemIsRecipe( const QListViewItem *item )
{
	bool is_recipe = false;
	if ( item )
		item->text(1).toInt(&is_recipe);

	return is_recipe;
}

#include "selectrecipedialog.moc"
