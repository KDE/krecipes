/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef INGREDIENTPROPERTYLIST_H
#define INGREDIENTPROPERTYLIST_H
#include <qptrlist.h>
#include "ingredientproperty.h"

/**
@author Unai Garro
*/
class IngredientPropertyList{
public:
    IngredientPropertyList();

    ~IngredientPropertyList();

    IngredientProperty* getFirst(void);
    IngredientProperty* getNext(void);
    IngredientProperty* getElement(int index);
    void clear(void);
    bool isEmpty(void);
    void add(IngredientProperty &element);
    private:
    QPtrList<IngredientProperty> list;

};

#endif
