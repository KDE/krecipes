/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "ingredientpropertylist.h"

IngredientPropertyList::IngredientPropertyList()
{
}


IngredientPropertyList::~IngredientPropertyList()
{
}

void IngredientPropertyList::add(IngredientProperty &property)
{
list.append (new IngredientProperty(property));
}

IngredientProperty* IngredientPropertyList::getFirst(void){
return(list.first());
}

IngredientProperty* IngredientPropertyList::getNext(void){
return(list.next());
}

IngredientProperty* IngredientPropertyList::getElement(int index){
return(list.at(index));
}

void IngredientPropertyList::clear(void)
{
list.clear();
}

bool IngredientPropertyList::isEmpty(void)
{
return(list.isEmpty());
}


