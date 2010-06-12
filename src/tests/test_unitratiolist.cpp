/***************************************************************************
 *   Copyright © 2010 Martin Engelmann <murphi.oss@googlemail.com>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include "test_unitratiolist.h"

#include "datablocks/unitratiolist.h"

#include <QtTest/QTest>

void Test_UnitRatioList::testGetRatio()
{
    UnitRatioList list;
    list.add(UnitRatio(1, 2, 3));
    list.add(UnitRatio(4, 5, 6));

    QCOMPARE(list.first().unitId1(), 1);
    QCOMPARE(list.first().unitId2(), 2);
    QCOMPARE(list.first().ratio(), 3.);

    QCOMPARE(list.last().unitId1(), 4);
    QCOMPARE(list.last().unitId2(), 5);
    QCOMPARE(list.last().ratio(), 6.);

    QCOMPARE(list.getRatio(1, 2), 3.);
    QCOMPARE(list.getRatio(2, 1), 1./3.);
    QCOMPARE(list.getRatio(4, 5), 6.);
    QCOMPARE(list.getRatio(5, 4), 1./6.);
}


QTEST_MAIN(Test_UnitRatioList)

#include "test_unitratiolist.moc"
