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

ElementList::ElementList():QPtrList <Element>()
{
}

ElementList::~ElementList()
{
}

Element ElementList::getElement(int index) const
{
return *(this->at(index));
}

bool ElementList::containsId(int id) const // Search by id (which uses search by item, with comparison defined on header)
{
Element i; i.id=id;
return ( find(i) != end() );
}
