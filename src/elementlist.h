/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef ELEMENTLIST_H
#define ELEMENTLIST_H

#include <qptrlist.h>
#include "element.h"

/**
@author Unai Garro
*/
class ElementList{
public:
    ElementList();

    ~ElementList();
    Element* getFirst(void);
    Element* getNext(void);
    Element* getElement(int index);
    void clear(void);
    bool isEmpty(void);
    void add(Element &element);
    private:
    QPtrList<Element> list;
};


class IDList: public QPtrList <int>
{
public:
IDList(){};
~IDList(){};
protected:
 virtual int compareItems( QPtrCollection::Item item1, QPtrCollection::Item item2){return (*((int*)item1)-*((int*)item2));};

};

#endif
