/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ingredientmatcherdialog.h"

#include "datablocks/recipelist.h"
#include "elementlist.h"
#include "DBBackend/recipedb.h"
#include "widgets/krelistview.h"

#include <qhbox.h>
#include <kiconloader.h>
#include <klocale.h>

IngredientMatcherDialog::IngredientMatcherDialog(QWidget *parent,RecipeDB *db):QVBox(parent)
{
	// Initialize internal variables
	database=db;
	
	//Design the dialog
	setSpacing(10);
	
	ingredientListView=new KreListView(this,i18n("Ingredients"),true,1);
	ingredientListView->listView()->setAllColumnsShowFocus(true);
	ingredientListView->listView()->addColumn("*");
	ingredientListView->listView()->addColumn(i18n("Ingredient"));
	
	recipeListView=new KreListView(this,i18n("Matching Recipes"));
	recipeListView->listView()->setAllColumnsShowFocus(true);
	recipeListView->listView()->addColumn(i18n("Title"));
	recipeListView->listView()->addColumn(i18n("Missing Ingredients"));
	
	KIconLoader il;
	QHBox *buttonBox=new QHBox(this);
	
	okButton=new QPushButton(buttonBox);
	okButton->setIconSet(il.loadIconSet("button_ok", KIcon::Small));
	okButton->setText(i18n("Find matching recipes"));
	
	clearButton=new QPushButton(buttonBox);
	clearButton->setIconSet(il.loadIconSet("editclear", KIcon::Small));
	clearButton->setText(i18n("Clear recipe list"));
	
	// Load the data
	reloadIngredients();
	
	// Connect signals & slots
	connect (okButton,SIGNAL(clicked()), this,SLOT(findRecipes()));
	connect (clearButton,SIGNAL(clicked()),recipeListView->listView(),SLOT(clear()));

}

IngredientMatcherDialog::~IngredientMatcherDialog()
{

}



void IngredientMatcherDialog::findRecipes(void)
{
	RecipeList rlist;
	IngredientList ilist;
	database->loadRecipeDetails(&rlist,true,false);
	
	
	QListViewItem *qlv_it;
	
	// First make a list of the ingredients that we have
	for (qlv_it=ingredientListView->listView()->firstChild();qlv_it;qlv_it=qlv_it->nextSibling())
		{
		IngredientListItem *il_it=(IngredientListItem*) qlv_it;
		if (il_it->isOn())
			{
			Ingredient ing; ing.name=il_it->name(); ing.ingredientID=il_it->id();
			ilist.append(ing);
			}
		}
	
	// Now show the recipes with ingredients that are contained in the previous set
	// of ingredients
	RecipeList::Iterator it;
	for (it=rlist.begin();it!=rlist.end();++it)
		{
		IngredientList il=(*it).ingList;
		
		if (ilist.containsSubSet(il))
			{
			new RecipeListItem(recipeListView->listView(),*it);
			}
		
		}
}

void IngredientMatcherDialog::reloadIngredients(void)
{

	ingredientListView->listView()->clear();
	ElementList ingredientList;
	database->loadIngredients(&ingredientList);
	
	ElementList::Iterator it;
	
	for (it=ingredientList.begin();it!=ingredientList.end();++it)
		{
		Element ingredient=*it;
		new IngredientListItem(ingredientListView->listView(),ingredient);
		}
}
