/***************************************************************************
 *   Copyright © 2010 Martin Engelmann <murphi.oss@googlemail.com>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include "test_ratingcriteria.h"

#include "datablocks/ratingcriteria.h"

#include <QtTest/QTest>


void Test_RatingCriteria::testGetterSetter()
{
    RatingCriteria a;

    a.setId(5);
    QCOMPARE(a.id(), 5);

    a.setName("Taste");
    QCOMPARE(a.name(), QString("Taste"));

    a.setStars(2.5);
    QCOMPARE(a.stars(), 2.5);
}


void Test_RatingCriteria::testCopyConstructionAndAssignment()
{
    RatingCriteria a;
    a.setId(2);
    a.setName("Overall");
    a.setStars(3.0);

    {
        RatingCriteria b(a);
        QCOMPARE(b.id(), a.id());
        QCOMPARE(b.name(), a.name());
        QCOMPARE(b.stars(), a.stars());
    }

    {
        RatingCriteria c;
        c = a;
        QCOMPARE(c.id(), a.id());
        QCOMPARE(c.name(), a.name());
        QCOMPARE(c.stars(), a.stars());
    }
}


QTEST_MAIN(Test_RatingCriteria)

#include "test_ratingcriteria.moc"
