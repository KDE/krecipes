/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
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
#ifndef INGREDIENTLIST_H
#define INGREDIENTLIST_H

#include <qptrlist.h>

#include "ingredient.h"

/**
@author Unai Garro
*/
class IngredientList: public QPtrList <Ingredient>{
public:
    IngredientList();
    IngredientList(const IngredientList &il);
    ~IngredientList();
    Ingredient* getFirst(void);
    Ingredient* getNext(void);
    void append(const Ingredient &ing);
    void append(Ingredient *ing){QPtrList<Ingredient>::append(ing);} //for the transition to QValueList...
    void addReverse(Ingredient &ing);
    void move(int index1,int index2);
    Ingredient* getLast(void);
    Ingredient* getPrev(void);
    void empty(void);
    int find(int id);
    int findNext(int id);
    IngredientList& operator=(const IngredientList &il);
};

#endif
