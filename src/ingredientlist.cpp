/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *                                                                         *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "ingredientlist.h"

IngredientList::IngredientList():QValueList<Ingredient>()
{
}

IngredientList::~IngredientList()
{
}

bool IngredientList::contains(Ingredient &ing) const
{
return(find(ing.ingredientID)!=-1);
}

bool IngredientList::containsSubSet(IngredientList &il)
{
IngredientList::Iterator it;
for (it=il.begin();it!=il.end();++it)
	{
	if (!contains(*it)) return false;
	}
return true;
}

void IngredientList::empty(void)
{
this->clear();
}

int IngredientList::find(int id) const // Search by id (which uses search by item, with comparison defined on header)
{
Ingredient i; i.ingredientID=id;
return findIndex(i);
}

IngredientList::const_iterator IngredientList::find(IngredientList::const_iterator it,int id) const // Search by id (which uses search by item, with comparison defined on header)
{
Ingredient i; i.ingredientID=id;
return QValueList<Ingredient>::find(it,i);
}

IngredientList::iterator IngredientList::find(IngredientList::iterator it,int id) // Search by id (which uses search by item, with comparison defined on header)
{
Ingredient i; i.ingredientID=id;
return QValueList<Ingredient>::find(it,i);
}

void IngredientList::move(int index1,int index2) //moves element in pos index1, to pos after index2
{
IngredientList::iterator tmp_it = this->at(index1);
Ingredient tmp_ing(*tmp_it);

this->remove( tmp_it );

tmp_it = this->at(index2);
this->insert(tmp_it,tmp_ing);
}