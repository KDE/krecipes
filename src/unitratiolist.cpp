/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "unitratiolist.h"



UnitRatioList::UnitRatioList()
{
}


UnitRatioList::~UnitRatioList()
{
}

void UnitRatioList::add(UnitRatio &ratio)
{
list.append (new UnitRatio(ratio));
}

UnitRatio* UnitRatioList::getFirst(void){
return(list.first());
}

UnitRatio* UnitRatioList::getNext(void){
return(list.next());
}

UnitRatio* UnitRatioList::getElement(int index){
return(list.at(index));
}

void UnitRatioList::clear(void)
{
list.clear();
}

bool UnitRatioList::isEmpty(void)
{
return(list.isEmpty());
}
