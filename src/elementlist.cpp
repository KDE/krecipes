/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "elementlist.h"

ElementList::ElementList()
{
setAutoDelete(true);
}


ElementList::~ElementList()
{
}

void ElementList::add(Element &element)
{
this->append (new Element(element));
}

Element* ElementList::getFirst(void){
return(this->first());
}

Element* ElementList::getLast(void){
return(this->last());
}

Element* ElementList::getPrev(void){
return(this->prev());
}

Element* ElementList::getNext(void){
return(this->next());
}

Element* ElementList::getElement(int index){
return(this->at(index));
}

bool ElementList::containsId(int id) // Search by id (which uses search by item, with comparison defined on header)
{
Element i; i.id=id;
return((find(&i)>=0));
}