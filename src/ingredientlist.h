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

#include <qvaluelist.h>

#include "ingredient.h"

/**
@author Unai Garro
*/
class IngredientList: public QValueList <Ingredient>
{
public:
    IngredientList();
    ~IngredientList();
    bool IngredientList::contains(Ingredient &ing) const;
    bool containsSubSet(IngredientList &il);
    void move(int index1,int index2);
    void empty(void);
    int find(int id) const;
    IngredientList::const_iterator find(IngredientList::const_iterator,int id) const;
    IngredientList::iterator find(IngredientList::iterator,int id);
};

#endif
