/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef CONSTRAINTLIST_H
#define CONSTRAINTLIST_H
#include <qptrlist.h>
#include <qstring.h>

#include "element.h"

/**
@author Unai Garro
*/

class IngredientProperty;

class Constraint{
public:

    Constraint(Constraint *cnt){id=cnt->id;name=cnt->name;units=cnt->units;perUnit=cnt->perUnit; max=cnt->max;min=cnt->min;enabled=cnt->enabled;}

    Constraint(){max=0.0;min=0.0;id=-1;enabled=false;}

    ~Constraint(){};
    int id;
    QString name;
    QString units;
    Element perUnit; // stores the unit ID and Name
    double max;
    double min;
    bool enabled;
};


class ConstraintPtrList: public QPtrList <Constraint>
{
public:
ConstraintPtrList(){setAutoDelete(true);};
~ConstraintPtrList(){};
protected:
 virtual int compareItems( QPtrCollection::Item item1, QPtrCollection::Item item2){return (((Constraint*)item1)->id-((Constraint*)item2)->id);};

};


class ConstraintList{
public:
    ConstraintList();

    ~ConstraintList();

    Constraint* getFirst(void);
    Constraint* getNext(void);
    Constraint* getElement(int index);
    void clear(void);
    bool isEmpty(void);
    void add(Constraint &element);
    void append(Constraint *constraint);
    int find(Constraint* it);
    Constraint* findByPty( IngredientProperty *pty );
    Constraint* at(int pos);
    private:
    ConstraintPtrList list;

};

#endif
