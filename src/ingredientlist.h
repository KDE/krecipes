/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef INGREDIENTLIST_H
#define INGREDIENTLIST_H

#include <qptrlist.h>
#include <iostream>
#include "ingredient.h"

/**
@author Unai Garro
*/
class IngredientList: QPtrList <Ingredient>{
public:
    IngredientList();

    ~IngredientList();
    Ingredient* getFirst(void);
    Ingredient* getNext(void);
    void add(Ingredient &ing);
    void move(int index1,int index2);
    Ingredient* getLast(void);
    Ingredient* getPrev(void);
    void empty(void);
    int find(int id);
protected:
    virtual int compareItems( QPtrCollection::Item item1, QPtrCollection::Item item2){return (((Ingredient*)item1)->ingredientID-((Ingredient*)item2)->ingredientID);};  // so find() works as expected (not searching for pointers, but id)

};

#endif
