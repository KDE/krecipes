/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
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
class IngredientList{
public:
    IngredientList();

    ~IngredientList();
    Ingredient* getFirst(void);
    Ingredient* getNext(void);
    void add(Ingredient &ing);
    void remove(int index);
    void move(int index1,int index2);
    void debug(void);
    Ingredient* getLast(void);
    Ingredient* getPrev(void);
private:
    QPtrList<Ingredient> list;

};

#endif
