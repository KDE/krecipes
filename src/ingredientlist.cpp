/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "ingredientlist.h"

IngredientList::IngredientList()
{
list.setAutoDelete(true); //Deletes automatically when remove() is used.
}


IngredientList::~IngredientList()
{
}

void IngredientList::add(Ingredient &ing)
{
list.append (new Ingredient(ing));
}

void IngredientList::remove(int index)
{
list.remove (index);
}

void IngredientList::move(int index1,int index2) //moves element in pos index1, to pos after index2
{
Ingredient* ing=list.take(index1);
list.insert(index2,ing);
}

Ingredient* IngredientList::getFirst(void){
return(list.first());
}

Ingredient* IngredientList::getNext(void){
return(list.next());
}

Ingredient* IngredientList::getLast(void)
{
return(list.last());
}

Ingredient* IngredientList::getPrev(void)
{
return(list.prev());
}

void IngredientList::empty(void)
{
list.clear();
}