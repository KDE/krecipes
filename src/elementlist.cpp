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
}


ElementList::~ElementList()
{
}

void ElementList::add(Element &element)
{
list.append (new Element(element));
}

Element* ElementList::getFirst(void){
return(list.first());
}

Element* ElementList::getLast(void){
return(list.last());
}

Element* ElementList::getPrev(void){
return(list.prev());
}

Element* ElementList::getNext(void){
return(list.next());
}

Element* ElementList::getElement(int index){
return(list.at(index));
}

void ElementList::clear(void)
{
list.clear();
}

bool ElementList::isEmpty(void)
{
return(list.isEmpty());
}