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

#include "advancedsearchdialog.h"
#include "DBBackend/recipedb.h"
#include "recipe.h"
#include "selectunitdialog.h"
#include "createelementdialog.h"

#include "exporters/cookmlexporter.h"
#include "exporters/htmlexporter.h"
#include "exporters/kreexport.h"
#include "exporters/mmfexporter.h"
#include "exporters/recipemlexporter.h"

SelectRecipeDialog::SelectRecipeDialog(QWidget *parent, RecipeDB* db)
 : QWidget(parent)
{
//Store pointer to Recipe Database
database=db;

//Initialize internal data
recipeList=new ElementList;
categoryList=new ElementList;

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
    	recipeListView->addColumn(i18n("Id"));
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
	removeButton->setText(i18n("Delete"));
  removeButton->setDisabled(true);
	removeButton->setMaximumWidth(100);
	pm=il->loadIcon("editshred", KIcon::NoGroup,16); removeButton->setIconSet(pm);

tabWidget->insertTab( basicSearchTab, "Basic" );

AdvancedSearchDialog *advancedSearch = new AdvancedSearchDialog(this,database);
tabWidget->insertTab( advancedSearch, "Advanced" );

// Popup menus
    kpop = new KPopupMenu( recipeListView );
    kpop->insertItem( il->loadIcon("ok", KIcon::NoGroup,16),tr2i18n("&Open"), this, SLOT(open()), CTRL+Key_L );
    kpop->insertItem( il->loadIcon("edit", KIcon::NoGroup,16),tr2i18n("&Edit"), this, SLOT(edit()), CTRL+Key_E );
    kpop->insertItem( il->loadIcon("filesaveas", KIcon::NoGroup,16),tr2i18n("&Save as"), this, SLOT(slotExportRecipe()), CTRL+Key_S );
    kpop->insertItem( il->loadIcon("editshred", KIcon::NoGroup,16),tr2i18n("Remove from &Category"), this, SLOT(removeFromCat()), CTRL+Key_C );
    kpop->insertItem( il->loadIcon("editshred", KIcon::NoGroup,16),tr2i18n("&Remove"), this, SLOT(remove()), CTRL+Key_R );
    kpop->polish();

    catPop = new KPopupMenu( recipeListView );
    catPop->insertItem( tr2i18n("&Expand All"), this, SLOT(expandAll()), CTRL+Key_Plus );
    catPop->insertItem( tr2i18n("&Collapse All"), this, SLOT(collapseAll()), CTRL+Key_Minus );
    catPop->insertItem( il->loadIcon("filesaveas", KIcon::NoGroup,16),tr2i18n("&Save as"), this, SLOT(slotExportRecipeFromCat()), CTRL+Key_S );

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
connect(advancedSearch,SIGNAL(recipeSelected(int,int)),SIGNAL(recipeSelected(int,int)));
}


SelectRecipeDialog::~SelectRecipeDialog()
{
}

void SelectRecipeDialog::showEvent(QShowEvent* e){
  if(!e->spontaneous()){
    openButton->setEnabled(false);
    editButton->setEnabled(false);
    removeButton->setEnabled(false);
    collapseAll();
  }
}

void SelectRecipeDialog::loadRecipeList(void)
{
recipeListView->clear();
recipeList->clear();
categoryList->clear();
categoryItems.clear();

// First show the categories

ElementList categoryList;

database->loadCategories(&categoryList);

for ( ElementList::const_iterator cat_it = categoryList.begin(); cat_it != categoryList.end(); ++cat_it )
	{
	QListViewItem *it=new QListViewItem(recipeListView,(*cat_it).name,"","");
	categoryItems.insert((*cat_it).id,it);
	}


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

void SelectRecipeDialog::open(void)
{
QListViewItem *it;
it=recipeListView->selectedItem();
if ( it )
{
	if ( !it->firstChild() && itemIsRecipe(it) )
		emit recipeSelected(it->text(1).toInt(),0);
	else if ( it->firstChild() )
	{
		QValueList<int> ids;
		for (QListViewItem *sub_it=it->firstChild();sub_it;sub_it=sub_it->nextSibling())
			ids.append( sub_it->text(1).toInt() );
		emit recipesSelected( ids, 0 );
	}
}
}

void SelectRecipeDialog::edit(void)
{
QListViewItem *it;
it=recipeListView->selectedItem();
if ( it && !it->firstChild() && itemIsRecipe(it) ) emit recipeSelected(it->text(1).toInt(),1);
}

void SelectRecipeDialog::remove(void)
{
QListViewItem *it;
it=recipeListView->selectedItem();
if ( it && !it->firstChild() && itemIsRecipe(it) ) emit recipeSelected(it->text(1).toInt(),2);
}

void SelectRecipeDialog::removeFromCat(void)
{
  QListViewItem *it;
  it=recipeListView->selectedItem();
  if ( it != 0 && !it->firstChild() && itemIsRecipe(it) ){
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
collapseAll();
}

void SelectRecipeDialog::filter(const QString& s)
{
for (QListViewItem *it=recipeListView->firstChild();it;it=it->nextSibling())
	{
	if ( s.isEmpty() ) // Don't filter if the filter text is empty
	{
		if ( !isFilteringCategories )
		{
			it->setVisible(false); //this insures that the call to setVisible(true) will show all the children of 'it'
			it->setVisible(true);
		}
	}
	else if ( !it->firstChild() ) // It's not a category or it's empty
	{
		if (it->text(2).contains(s,false) && !isFilteringCategories) it->setVisible(true);
		else it->setVisible(false);
	}
	else // It's a category. Check the children
	{
		if (!isFilteringCategories) it->setVisible( true ); //make sure it is visible

		bool cat_has_matches = false;
		for (QListViewItem *cit=it->firstChild();cit;cit=cit->nextSibling())
		{
			if (cit->text(2).contains(s,false) && !s.isEmpty())
			{
				cat_has_matches = true;
				cit->setVisible(true);
				it->setOpen(true);
			}
			else
				cit->setVisible(false);
		}
		
		if (!isFilteringCategories && !cat_has_matches) it->setVisible(false);
	}
	}
}


void SelectRecipeDialog::filterCategories(int categoryID)
{
kdDebug()<<"I got category :"<<categoryID<<"\n";
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
for ( ElementList::const_iterator cat_it = categoryList.begin(); cat_it != categoryList.end(); ++cat_it )
	{
	categoryBox->insertItem((*cat_it).name);
	categoryComboRows.insert(row,new int((*cat_it).id)); // store category id's in the combobox position to obtain the category id later
	row++;
	}

}

void SelectRecipeDialog::exportRecipes( const QValueList<int> &ids, const QString & caption, const QString &selection )
{
	KFileDialog* fd = new KFileDialog( QString::null,
	  "*.kre|Gzip Krecipes file (*.kre)\n"
	  "*.kreml|Krecipes xml file (*.kreml)\n"
	  "*.cml|CookML file (*.cml)\n"
	  "*.html|HTML file (*.html)\n"
	  "*.mmf|Meal-Master file (*.mmf)\n"
	  "*.xml|RecipeML file (*.xml)",
	  this, "export_dlg", true);
	fd->setCaption( caption );
	fd->setOperationMode( KFileDialog::Saving );
	fd->setSelection( selection );
	if ( fd->exec() == KFileDialog::Accepted )
	{
		QString fileName = fd->selectedFile();
		if( !fileName.isNull() )
		{
			BaseExporter *exporter;
			if ( fd->currentFilter() == "*.xml" )
				exporter = new RecipeMLExporter(fileName, fd->currentFilter());
			else if ( fd->currentFilter() == "*.mmf" )
				exporter = new MMFExporter(fileName, fd->currentFilter());
			else if ( fd->currentFilter() == "*.html" )
				exporter = new HTMLExporter(database, fileName, fd->currentFilter(), 650);
			else if ( fd->currentFilter() == "*.cml" )
				exporter = new CookMLExporter(fileName, fd->currentFilter());
			else
				exporter = new KreExporter(fileName, fd->currentFilter());

			int overwrite = -1;
			if ( QFile::exists( exporter->fileName() ) )
			{
				overwrite = KMessageBox::warningYesNo( 0,QString(i18n("File \"%1\" exists.  Are you sure you want to overwrite it?")).arg(exporter->fileName()),i18n("Saving recipe") );
			}

			if ( overwrite == KMessageBox::Yes || overwrite == -1 )
			{
				KProgressDialog progress_dialog(this, "export_progress_dialog", QString::null, i18n("Preparing to save recipes...") );
				progress_dialog.setAutoClose(false); progress_dialog.setAutoReset(true);
				RecipeList recipes; database->loadRecipes( &recipes, ids, &progress_dialog );
				progress_dialog.setAutoReset(false);

				progress_dialog.setLabel( i18n("Saving recipes...") );
				exporter->exporter( recipes, &progress_dialog );
			}
			delete exporter;
		}
	}
	delete fd;
}

/*!
    \fn SelectRecipeDialog::slotExportRecipe()
 */
void SelectRecipeDialog::slotExportRecipe()
{
	if ( recipeListView->selectedItem() )
	{
		if ( recipeListView->selectedItem()->firstChild() )
			slotExportRecipeFromCat();
		else
		{
			QValueList<int> id;
			id.append( (recipeListView->selectedItem())->text(1).toInt() );
	
			exportRecipes( id, i18n("Save Recipe"), (recipeListView->selectedItem())->text(2) );
		}
	}
	else //if nothing selected, export all visible recipes
	{
		QValueList<int> ids = getAllVisibleItems();

		if ( ids.count() > 0 )
			exportRecipes( ids, i18n("Save Recipes"), "Recipes" );
		//else TODO: give notice
	}
}

void SelectRecipeDialog::slotExportRecipeFromCat()
{
	if (recipeListView->selectedItem() )
	{
		QValueList<int> ids;
		for (QListViewItem *cit=(recipeListView->selectedItem())->firstChild();cit;cit=cit->nextSibling())
			ids.append(cit->text(1).toInt());

		exportRecipes( ids, i18n("Save Recipes"), (recipeListView->selectedItem())->text(0) );
	}
}

QValueList<int> SelectRecipeDialog::getAllVisibleItems()
{
	QValueList<int> ids;

	for (QListViewItem *it=recipeListView->firstChild();it;it=it->nextSibling())
	{
		if ( it->isVisible() )
		{
			if ( !it->firstChild() ) // It's not a category or it's empty
			{
				if ( !it->text(1).isEmpty() && ids.find(it->text(1).toInt()) == ids.end() )
					ids << it->text(1).toInt();
			}
			else // It's a category. Check the children
			{
				for (QListViewItem *cit=it->firstChild();cit;cit=cit->nextSibling())
				{
					if ( cit->isVisible() && ids.find(cit->text(1).toInt()) == ids.end() )
						ids << cit->text(1).toInt();
				}
			}
		}
	}
	
	return ids;
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

void SelectRecipeDialog::showPopup( KListView */*l*/, QListViewItem *i, const QPoint &p ){
	if (i) // Check if the QListViewItem actually exists
	{
		if ( itemIsRecipe(i) )
			kpop->exec(p);
		else if ( i->firstChild() ) //is a category... don't pop-up for an empty category though
			catPop->exec(p);
	}
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

void SelectRecipeDialog::expandAll(){
  for (QListViewItem *it=recipeListView->firstChild();it;it=it->nextSibling())
  {
    it->setOpen(true);
  }
}

void SelectRecipeDialog::collapseAll(){
  for (QListViewItem *it=recipeListView->firstChild();it;it=it->nextSibling())
  {
    it->setOpen(false);
  }
}

//item is a recipe if the 2nd column is an integer (the recipe's ID)
bool SelectRecipeDialog::itemIsRecipe( const QListViewItem *item )
{
	bool is_recipe = false;
	if ( item )
		item->text(1).toInt(&is_recipe);

	return is_recipe;
}
