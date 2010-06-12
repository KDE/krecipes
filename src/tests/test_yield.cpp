/***************************************************************************
 *   Copyright © 2010 Martin Engelmann <murphi.oss@googlemail.com>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include "test_yield.h"

#include "datablocks/yield.h"

#include <QtTest/QTest>


void Test_Yield::testGetterSetter()
{
    Yield a;

    a.setAmount(5);
    QCOMPARE(a.amount(), 5.);

    a.setAmountOffset(6);
    QCOMPARE(a.amountOffset(), 6.);

    a.setType("servings");
    QCOMPARE(a.type(), QString("servings"));

    a.setTypeId(2);
    QCOMPARE(a.typeId(), 2);
}

void Test_Yield::testCopyConstructionAndAssignment()
{
    Yield a;
    a.setAmount(2);
    a.setAmountOffset(3);
    a.setType("pieces");
    a.setTypeId(1);

    {
        Yield b(a);
        QCOMPARE(b.amount(), a.amount());
        QCOMPARE(b.amountOffset(), a.amountOffset());
        QCOMPARE(b.type(), a.type());
        QCOMPARE(b.typeId(), a.typeId());
    }

    {
        Yield b;
        b = a;
        QCOMPARE(b.amount(), a.amount());
        QCOMPARE(b.amountOffset(), a.amountOffset());
        QCOMPARE(b.type(), a.type());
        QCOMPARE(b.typeId(), a.typeId());
    }
}

void Test_Yield::testToString()
{
    Yield a;
    a.setAmount(2);
    a.setAmountOffset(3);
    a.setType("pieces");
    a.setTypeId(1);

    QCOMPARE(a.toString(), QString("2-5 pieces"));
}



QTEST_MAIN(Test_Yield)

#include "test_yield.moc"
