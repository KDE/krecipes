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

IngredientList::IngredientList():QPtrList<Ingredient>()
{
setAutoDelete(true); //Deletes automatically when remove() is used.
}

IngredientList::IngredientList(const IngredientList &il):QPtrList<Ingredient>(il)
{
clear();
setAutoDelete(true);
QPtrListIterator<Ingredient> it(il);
Ingredient *i;
while ( (i = it.current()) != 0 )
{
++it;
this->append(*i);
}
}

IngredientList::~IngredientList()
{
}

void IngredientList::append(const Ingredient &ing)
{
Ingredient *newIng=new Ingredient(ing);
this->append (newIng);
}

void IngredientList::addReverse(Ingredient &ing)
{
Ingredient *newIng=new Ingredient(ing);
this->prepend (newIng);
}

void IngredientList::move(int index1,int index2) //moves element in pos index1, to pos after index2
{
Ingredient* ing=this->take(index1);
this->insert(index2,ing);
}

Ingredient* IngredientList::getFirst(void){
return(this->first());
}

Ingredient* IngredientList::getNext(void){
return(this->next());
}

Ingredient* IngredientList::getLast(void)
{
return(this->last());
}

Ingredient* IngredientList::getPrev(void)
{
return(this->prev());
}

void IngredientList::empty(void)
{
this->clear();
}

int IngredientList::find(int id) // Search by id (which uses search by item, with comparison defined on header)
{
Ingredient i; i.ingredientID=id;
return(QPtrList <Ingredient>::find(&i)); // (If we don't specify class, gcc will only find "find(int)"
}

int IngredientList::findNext(int id) // Search by id (which uses search by item, with comparison defined on header)
{
Ingredient i; i.ingredientID=id;
return(QPtrList <Ingredient>::findNext(&i)); // (If we don't specify class, gcc will only find "findNext(int)"
}

IngredientList& IngredientList::operator=(const IngredientList &il)
{
clear();
QPtrListIterator<Ingredient> it(il);
Ingredient *i;
while ( (i = it.current()) != 0 )
{
++it;
this->append(*i);
}
return *this;
}
