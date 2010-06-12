/***************************************************************************
 *   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
 *   Copyright © 2010 Martin Engelmann <murphi.oss@googlemail.com>         *
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
class UnitRatio
{
    class Private;
    Private* d;

public:

    UnitRatio();
    UnitRatio(int unitId1, int unitId2, double ratio);
    UnitRatio(const UnitRatio & rhs);
    ~UnitRatio();

    UnitRatio& operator=(const UnitRatio & rhs);

    bool operator!=(const UnitRatio &rhs) const;
    bool operator==(const UnitRatio &rhs) const;

    int unitId1() const;
    void setUnitId1(int unitId1);

    int unitId2() const;
    void setUnitId2(int unitId2);

    double ratio() const;
    void setRatio(double ratio);

    double inverseRatio() const;
    UnitRatio reverse() const;
};

#endif
