/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef UNITRATIO_H
#define UNITRATIO_H

/**
@author Unai Garro
*/
class UnitRatio{
public:

    UnitRatio();
    UnitRatio(const UnitRatio &ur);
    ~UnitRatio();
    
    bool operator!=(const UnitRatio &r) const { return !(r.uID1==uID1 && r.uID2==uID2); }
    bool operator==(const UnitRatio &r) const { return (r.uID1==uID1 && r.uID2==uID2); }
    
    int uID1,uID2;
    double ratio;
};

#endif
