/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef UNITRATIOLIST_H
#define UNITRATIOLIST_H
#include <qptrlist.h>
#include "unitratio.h"


/**
@author Unai Garro
*/
class UnitRatioList{
public:
     UnitRatioList();
    ~UnitRatioList();
    UnitRatio* getFirst(void);
    UnitRatio* getNext(void);
    UnitRatio* getElement(int index);
    void clear(void);
    bool isEmpty(void);
    void add(UnitRatio &ratio);
    private:
    QPtrList<UnitRatio> list;
};

#endif
