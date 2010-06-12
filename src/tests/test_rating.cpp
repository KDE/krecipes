/***************************************************************************
 *   Copyright © 2010 Martin Engelmann <murphi.oss@googlemail.com>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include "test_rating.h"

#include "datablocks/rating.h"

#include <QtTest/QTest>

void Test_Rating::testGetterSetter()
{
    Rating a;
    a.setId(5);
    QCOMPARE(a.id(), 5);

    a.setComment("This is the comment.");
    QCOMPARE(a.comment(), QString("This is the comment."));

    a.setRater("Me");
    QCOMPARE(a.rater(), QString("Me"));
}

void Test_Rating::testCopyConstructionAndAssignment()
{
    Rating a;
    a.setId(9);
    a.setRater("The chef");
    a.setComment("Superb!");

    {
        Rating b(a);
        QCOMPARE(b.id(), a.id());
        QCOMPARE(b.rater(), a.rater());
        QCOMPARE(b.comment(), a.comment());
    }

    {
        Rating c;
        c = a;
        QCOMPARE(c.id(), a.id());
        QCOMPARE(c.rater(), a.rater());
        QCOMPARE(c.comment(), a.comment());
    }
}

void Test_Rating::testRatingCriteriaListOperations()
{
    RatingCriteria taste;
    taste.setId(1);
    taste.setName("Taste");
    taste.setStars(3);

    RatingCriteria overall;
    overall.setId(2);
    overall.setName("Overall");
    overall.setStars(4);

    Rating a;
    a.setId(9);
    a.setRater("The chef");
    a.setComment("Superb!");
    a.append(taste);

    QCOMPARE(1, a.ratingCriterias().size());
    QCOMPARE(QString("Taste"), a.ratingCriterias().first().name());

    a.append(overall);
    QCOMPARE(2, a.ratingCriterias().size());
    QCOMPARE(QString("Overall"), a.ratingCriterias().last().name());
}


void Test_Rating::testSetIdOfRatingCriteria()
{
    RatingCriteria taste;
    taste.setId(1);
    taste.setName("Taste");
    taste.setStars(3);

    RatingCriteria overall;
    overall.setId(2);
    overall.setName("Overall");
    overall.setStars(4);

    Rating a;
    a.setId(9);
    a.setRater("The chef");
    a.setComment("Superb!");
    a.append(taste);
    a.append(overall);


    QCOMPARE(2, a.ratingCriterias().size());
    QCOMPARE(QString("Taste"), a.ratingCriterias().first().name());
    QCOMPARE(1, a.ratingCriterias().first().id());
    QCOMPARE(QString("Overall"), a.ratingCriterias().last().name());
    QCOMPARE(2, a.ratingCriterias().last().id());

    QVERIFY(a.setIdOfRatingCriteria("Taste", 5));
    QVERIFY(! a.setIdOfRatingCriteria("Look", 7));

    QCOMPARE(5, a.ratingCriterias().first().id());
    QCOMPARE(2, a.ratingCriterias().last().id());
}

void Test_Rating::testNumberSumAndAverage()
{
    RatingCriteria taste;
    taste.setId(1);
    taste.setName("Taste");
    taste.setStars(3);

    RatingCriteria overall;
    overall.setId(2);
    overall.setName("Overall");
    overall.setStars(4);

    Rating a;
    a.setId(9);
    a.setRater("The chef");
    a.setComment("Superb!");

    QVERIFY(!a.hasRatingCriterias());
    QCOMPARE(0, a.numberOfRatingCriterias());
    QCOMPARE(0., a.sum());
    QCOMPARE(-1., a.average());

    a.append(taste);
    a.append(overall);

    QVERIFY(a.hasRatingCriterias());
    QCOMPARE(2, a.numberOfRatingCriterias());
    QCOMPARE(7., a.sum());
    QCOMPARE(3.5, a.average());
}


QTEST_MAIN(Test_Rating)

#include "test_rating.moc"
