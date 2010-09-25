/***************************************************************************
 *   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
 *   Copyright © 2010 Martin Engelmann <murphi.oss@googlemail.com>         *
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

void UnitRatioList::add(const UnitRatio& r)
{
    append(r);
}

double UnitRatioList::getRatio(int uid1, int uid2)
{
    if (uid1 == uid2)
        return (1.0);

    for (UnitRatioList::const_iterator ur_it = constBegin(); ur_it != constEnd(); ++ur_it) {
        if (ur_it->unitId1() == uid1 && ur_it->unitId2() == uid2)
            return ur_it->ratio();

        if (ur_it->unitId1() == uid2 && ur_it->unitId2() == uid1)
            return ur_it->inverseRatio();
    }

    return (-1.0);
}
