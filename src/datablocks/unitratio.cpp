/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/


#include "unitratio.h"



class UnitRatio::Private
{
public:
    int uID1;
    int uID2;
    double ratio;
};

UnitRatio::UnitRatio()
        : d(new Private())
{
    d->uID1 = -1;
    d->uID2 = -1;
    d->ratio = -1;
}

UnitRatio::UnitRatio(int unitId1, int unitId2, double ratio)
        : d(new Private())
{
    d->uID1 = unitId1;
    d->uID2 = unitId2;
    d->ratio = ratio;
}

UnitRatio::UnitRatio(const UnitRatio &rhs)
        : d(new Private(*rhs.d))
{
}

UnitRatio::~UnitRatio()
{
    delete d;
}

UnitRatio& UnitRatio::operator=(const UnitRatio & rhs)
{
    if (this != &rhs) {
        delete d;
        d = new Private(*rhs.d);
    }
    return *this;
}

bool UnitRatio::operator!=(const UnitRatio &rhs) const
{
    return !(*this == rhs);
}

bool UnitRatio::operator==(const UnitRatio &rhs) const
{
    return (rhs.d->uID1 == d->uID1 && rhs.d->uID2 == d->uID2);
}

int UnitRatio::unitId1() const
{
    return d->uID1;
}

void UnitRatio::setUnitId1(int unitId1)
{
    d->uID1 = unitId1;
}

int UnitRatio::unitId2() const
{
    return d->uID2;
}

void UnitRatio::setUnitId2(int unitId2)
{
    d->uID2 = unitId2;
}

double UnitRatio::ratio() const
{
    return d->ratio;
}

void UnitRatio::setRatio(double ratio)
{
    d->ratio = ratio;
}

double UnitRatio::inverseRatio() const
{
    return d->ratio == 0. ? 0. : 1. / d->ratio;
}

UnitRatio UnitRatio::reverse() const
{
    return UnitRatio(d->uID2, d->uID1, inverseRatio());
}

