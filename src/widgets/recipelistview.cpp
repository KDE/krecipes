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
 
#include "recipelistview.h"

#include <qintdict.h>

#include <kconfig.h>
#include <kglobal.h>

#include "DBBackend/recipedb.h"
 
RecipeListView::RecipeListView( QWidget *parent, RecipeDB *db ) : StdCategoryListView(parent,db),
	flat_list(false)
{
	connect(database,SIGNAL(recipeCreated(const Element &,const ElementList &)),SLOT(createRecipe(const Element &,const ElementList &)));
	connect(database,SIGNAL(recipeRemoved(int)),SLOT(removeRecipe(int)));
	connect(database,SIGNAL(recipeRemoved(int,int)),SLOT(removeRecipe(int,int)));
	connect(database,SIGNAL(recipeModified(const Element &,const ElementList &)),SLOT(modifyRecipe(const Element &,const ElementList &)));

	setColumnText(0,"Recipe");
	/*addColumn( i18n("Recipe") );

	KConfig *config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry("ShowID",false);
	addColumn( i18n("Id"), show_id ? -1 : 0 );*/
}

void RecipeListView::reload()
{
	if ( flat_list ) {
		ElementList recipeList;
		database->loadRecipeList(&recipeList);

		ElementList::const_iterator recipe_it;
		for ( recipe_it=recipeList.begin();recipe_it != recipeList.end();++recipe_it ) {
			Recipe recipe;
			recipe.recipeID = (*recipe_it).id;
			recipe.title = (*recipe_it).name;
			createRecipe(recipe,-1);
		}
	}
	else {
		CategoryListView::reload();
		
		// Now show the recipes
		ElementList recipeList;
		QIntDict <bool> recipeCategorized; recipeCategorized.setAutoDelete(true); // it deletes the bools after finished
		QPtrList <int> recipeCategoryList;
		
		database->loadRecipeList(&recipeList,0,&recipeCategoryList); // Read the whole list of recipes including category
		
		int *categoryID;
		ElementList::const_iterator recipe_it;
		for ( recipe_it=recipeList.begin(),categoryID=recipeCategoryList.first();recipe_it != recipeList.end() && categoryID;++recipe_it,categoryID=recipeCategoryList.next() ) {
			if (items_map[*categoryID]) {
				Recipe recipe;
				recipe.recipeID = (*recipe_it).id;
				recipe.title = (*recipe_it).name;
				createRecipe(recipe,*categoryID);
				recipeCategorized.insert((*recipe_it).id,new bool(true)); // mark the recipe as categorized
			}
		}
		
		
		// Add those recipes that have not been categorised in any categories
		for ( recipe_it=recipeList.begin(),categoryID=recipeCategoryList.first();recipe_it != recipeList.end() && categoryID;++recipe_it,categoryID=recipeCategoryList.next() ) {
			if (!recipeCategorized[(*recipe_it).id]) {
				Recipe recipe;
				recipe.recipeID = (*recipe_it).id;
				recipe.title = (*recipe_it).name;
				createRecipe(recipe,-1);
			}
		}
	}
}

void RecipeListView::createRecipe( const Recipe &recipe, int parent_id )
{
	if ( parent_id == -1 )
		(void)new RecipeListItem(this,recipe);
	else
		(void)new RecipeListItem(items_map[parent_id],recipe);
}

void RecipeListView::createRecipe( const Element &recipe_el, const ElementList &categories )
{
	Recipe recipe;
	recipe.recipeID = recipe_el.id;
	recipe.title = recipe_el.name;

	for ( ElementList::const_iterator cat_it = categories.begin(); cat_it != categories.end(); ++cat_it ) {
		int cur_cat_id = (*cat_it).id;

		QListViewItemIterator iterator(this);
		while(iterator.current())
		{
			if ( iterator.current()->rtti() == 1001 ) {
				CategoryListItem *cat_item = (CategoryListItem*)iterator.current();
				if ( cat_item->categoryId() == cur_cat_id ) {
					createRecipe(recipe,cur_cat_id);
				}
			}
			++iterator;
		}
	}
}

void RecipeListView::modifyRecipe( const Element &recipe, const ElementList &categories )
{
	removeRecipe(recipe.id);
	createRecipe( recipe, categories );
}

void RecipeListView::removeRecipe( int id )
{
	QListViewItemIterator iterator(this);
	while(iterator.current())
	{
		if ( iterator.current()->rtti() == 1000 ) {
			RecipeListItem *recipe_it = (RecipeListItem*)iterator.current();
			if ( recipe_it->recipeID() == id )
				delete recipe_it;
		}
		++iterator;
	}
}

void RecipeListView::removeRecipe( int recipe_id, int cat_id )
{
	QListViewItem *item = items_map[cat_id];

	//find out if this is the only category the recipe belongs to
	int finds = 0;
	QListViewItemIterator iterator(this);
	while(iterator.current())
	{
		if ( iterator.current()->rtti() == 1000 ) {
			RecipeListItem *recipe_it = (RecipeListItem*)iterator.current();
			
			if ( recipe_it->recipeID() == recipe_id ) {
				if ( finds > 1 ) break;
				finds++;
			}
		}
		++iterator;
	}

	//do this to only iterate over children of 'item'
	QListViewItem *pEndItem = NULL;
	QListViewItem *pStartItem = item;
	do
	{
		if(pStartItem->nextSibling())
			pEndItem = pStartItem->nextSibling();
		else
			pStartItem = pStartItem->parent();
	}
	while(pStartItem && !pEndItem);
	
	iterator = QListViewItemIterator(item);
	while(iterator.current() != pEndItem)
	{
		if ( iterator.current()->rtti() == 1000 ) {
			RecipeListItem *recipe_it = (RecipeListItem*)iterator.current();
			
			if ( recipe_it->recipeID() == recipe_id ) {
				if ( finds > 1 ) {
					delete recipe_it;
				}
				else {
					//move this item to the root
					recipe_it->parent()->takeItem(recipe_it);
					insertItem(recipe_it);
				}
				break;
			}
		}
		++iterator;
	}
}

void RecipeListView::removeCategory( int id )
{
	QListViewItem *item = items_map[id];
	if ( !item ) return; //this may have been deleted already by its parent being deleted

	//do this to only iterate over children of 'item'
	//these children will all be moved to the listview root
	QListViewItem *pEndItem = NULL;
	QListViewItem *pStartItem = item;
	do
	{
		if(pStartItem->nextSibling())
			pEndItem = pStartItem->nextSibling();
		else
			pStartItem = pStartItem->parent();
	}
	while(pStartItem && !pEndItem);
	
	QListViewItemIterator iterator(item);
	while(iterator.current() != pEndItem)
	{
		if ( iterator.current()->rtti() == 1000 ) {
			QListViewItem *recipe_it = iterator.current();

			//move this item to the root
			recipe_it->parent()->takeItem(recipe_it);
			insertItem(recipe_it);
		}
		++iterator;
	}

	StdCategoryListView::removeCategory(id);
}

#include "recipelistview.moc"
