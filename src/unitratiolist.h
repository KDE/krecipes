/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef UNITRATIOLIST_H
#define UNITRATIOLIST_H
#include <qvaluelist.h>
#include "unitratio.h"


/**
@author Unai Garro
*/
class UnitRatioList : public QValueList <UnitRatio>{
public:
     UnitRatioList();
    ~UnitRatioList();
    
    void add( const UnitRatio &r ){ append(r); }
    double getRatio(int uid1, int uid2);
};

#endif
