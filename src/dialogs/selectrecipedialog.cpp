/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "selectrecipedialog.h"

SelectRecipeDialog::SelectRecipeDialog(QWidget *parent, RecipeDB* db)
 : QVBox(parent)
{
//Store pointer to Recipe Database
database=db;

//Initialize internal data
recipeList=new ElementList;

// Load Recipe List
loadRecipeList();
}


SelectRecipeDialog::~SelectRecipeDialog()
{
}

void SelectRecipeDialog::loadRecipeList(void)
{
database->loadRecipeList(recipeList);

for ( Element *recipe =recipeList->getFirst(); recipe; recipe =recipeList->getNext() ) int a=1;
	//ingredientBox->insertItem(ing->name);


}
