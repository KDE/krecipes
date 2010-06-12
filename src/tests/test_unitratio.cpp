/***************************************************************************
 *   Copyright © 2010 Martin Engelmann <murphi.oss@googlemail.com>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include "test_unitratio.h"

#include "datablocks/unitratio.h"

#include <QtTest/QTest>

void Test_UnitRatio::testConstructor()
{
    UnitRatio a(1, 2, 0.5);
    QCOMPARE(a.unitId1(), 1);
    QCOMPARE(a.unitId2(), 2);
    QCOMPARE(a.ratio(), 0.5);
}

void Test_UnitRatio::testGetterSetter()
{
    UnitRatio a;
    QCOMPARE(a.unitId1(), -1);
    QCOMPARE(a.unitId2(), -1);
    QCOMPARE(a.ratio(), -1.);

    a.setUnitId1(1);
    QCOMPARE(a.unitId1(), 1);

    a.setUnitId2(2);
    QCOMPARE(a.unitId2(), 2);

    a.setRatio(0.5);
    QCOMPARE(a.ratio(), 0.5);
}

void Test_UnitRatio::testCopyConstructionAndAssignment()
{
    UnitRatio a;
    a.setUnitId1(1);
    a.setUnitId2(2);
    a.setRatio(0.5);

    {
        UnitRatio b(a);
        QCOMPARE(b.unitId1(), a.unitId1());
        QCOMPARE(b.unitId2(), a.unitId2());
        QCOMPARE(b.ratio(), a.ratio());
    }

    {
        UnitRatio b;
        b = a;
        QCOMPARE(b.unitId1(), a.unitId1());
        QCOMPARE(b.unitId2(), a.unitId2());
        QCOMPARE(b.ratio(), a.ratio());
    }
}

void Test_UnitRatio::testReverse()
{
    UnitRatio a(1, 2, 0.5);
    UnitRatio b(a.reverse());

    QCOMPARE(b.unitId1(), 2);
    QCOMPARE(b.unitId2(), 1);
    QCOMPARE(b.ratio(), 2.);
    QCOMPARE(b.inverseRatio(), 0.5);
}


QTEST_MAIN(Test_UnitRatio)

#include "test_unitratio.moc"
