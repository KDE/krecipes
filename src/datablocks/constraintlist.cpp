/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "constraintlist.h"

ConstraintList::ConstraintList()
{
}


ConstraintList::~ConstraintList()
{
}

void ConstraintList::add(Constraint &constraint)
{
list.append (new Constraint(constraint));
}

Constraint* ConstraintList::getFirst(void){
return(list.first());
}

Constraint* ConstraintList::getNext(void){
return(list.next());
}

Constraint* ConstraintList::getElement(int index){
return(list.at(index));
}

void ConstraintList::clear(void)
{
list.clear();
}

bool ConstraintList::isEmpty(void)
{
return(list.isEmpty());
}

int ConstraintList::find(Constraint* it)
{
return(list.find(it));
}

Constraint* ConstraintList::at(int pos)
{
return(list.at(pos));
}

void ConstraintList::append(Constraint *constraint)
{
list.append (constraint);
}
