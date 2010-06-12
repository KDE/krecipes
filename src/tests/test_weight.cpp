/***************************************************************************
 *   Copyright © 2010 Martin Engelmann <murphi.oss@googlemail.com>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include "test_weight.h"

#include "datablocks/weight.h"

#include <QtTest/QTest>

void Test_Weight::testGetterSetter()
{
    Weight a;

    a.setId(1);
    QCOMPARE(a.id(), 1);

    a.setIngredientId(2);
    QCOMPARE(a.ingredientId(), 2);

    a.setPerAmountUnitId(3);
    QCOMPARE(a.perAmountUnitId(), 3);

    a.setPerAmountUnit("kg");
    QCOMPARE(a.perAmountUnit(), QString("kg"));

    a.setPerAmount(4.0);
    QCOMPARE(a.perAmount(), 4.0);

    a.setWeightUnitId(5);
    QCOMPARE(a.weightUnitId(), 5);

    a.setWeightUnit("g");
    QCOMPARE(a.weightUnit(), QString("g"));

    a.setWeight(6.0);
    QCOMPARE(a.weight(), 6.0);

    a.setPrepMethodId(7);
    QCOMPARE(a.prepMethodId(), 7);

    a.setPrepMethod("chopped");
    QCOMPARE(a.prepMethod(), QString("chopped"));
}

void Test_Weight::testCopyConstructionAndAssignment()
{
    Weight a;
    a.setId(1);
    a.setIngredientId(2);
    a.setPerAmountUnitId(3);
    a.setPerAmountUnit("kg");
    a.setPerAmount(4.0);
    a.setWeightUnitId(5);
    a.setWeightUnit("g");
    a.setWeight(6.0);
    a.setPrepMethodId(7);
    a.setPrepMethod("chopped");

    {
        Weight b(a);
        QCOMPARE(b.id(), a.id());
        QCOMPARE(b.ingredientId(), a.ingredientId());
        QCOMPARE(b.perAmountUnitId(), a.perAmountUnitId());
        QCOMPARE(b.perAmountUnit(), a.perAmountUnit());
        QCOMPARE(b.perAmount(), a.perAmount());
        QCOMPARE(b.weightUnitId(), a.weightUnitId());
        QCOMPARE(b.weightUnit(), a.weightUnit());
        QCOMPARE(b.weight(), a.weight());
        QCOMPARE(b.prepMethodId(), a.prepMethodId());
        QCOMPARE(b.prepMethod(), a.prepMethod());
    }

    {
        Weight b;
        b = a;
        QCOMPARE(b.id(), a.id());
        QCOMPARE(b.ingredientId(), a.ingredientId());
        QCOMPARE(b.perAmountUnitId(), a.perAmountUnitId());
        QCOMPARE(b.perAmountUnit(), a.perAmountUnit());
        QCOMPARE(b.perAmount(), a.perAmount());
        QCOMPARE(b.weightUnitId(), a.weightUnitId());
        QCOMPARE(b.weightUnit(), a.weightUnit());
        QCOMPARE(b.weight(), a.weight());
        QCOMPARE(b.prepMethodId(), a.prepMethodId());
        QCOMPARE(b.prepMethod(), a.prepMethod());
    }
}


QTEST_MAIN(Test_Weight)

#include "test_weight.moc"
