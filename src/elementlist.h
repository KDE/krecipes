/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
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
class ElementList: public QPtrList<Element>{
public:
    ElementList();

    ~ElementList();
    Element* getFirst(void);
    Element* getPrev(void);
    Element* getNext(void);
    Element* getLast(void);
    Element* getElement(int index);
    void add(Element &element);

    protected:
    virtual int compareItems( QPtrCollection::Item item1, QPtrCollection::Item item2){return (((Element*)item1)->id-((Element*)item2)->id);};
};


class IDList: public QPtrList <int>
{
public:
IDList(){setAutoDelete(true);};
~IDList(){};
protected:
 virtual int compareItems( QPtrCollection::Item item1, QPtrCollection::Item item2){return (*((int*)item1)-*((int*)item2));};

};

#endif
