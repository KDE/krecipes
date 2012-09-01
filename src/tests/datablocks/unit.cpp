/***************************************************************************
 *   Copyright © 2010 Martin Engelmann <murphi.oss@googlemail.com>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include "unit.h"

#include "datablocks/unit.h"

#include <QtTest/QTest>

void Test_Unit::testConstructor()
{
    Unit a("foo", "bar");
    QCOMPARE(a.id(), -1);
    QCOMPARE(a.name(), QString("foo"));
    QCOMPARE(a.plural(), QString("bar"));

    Unit b("foo", "bar", 0);
    QCOMPARE(b.id(), 0);
    QCOMPARE(b.name(), QString("foo"));
    QCOMPARE(b.plural(), QString("bar"));

    Unit c("foo", 1);
    QCOMPARE(c.name(), QString("foo"));
    QCOMPARE(c.plural(), QString());

    Unit d("bar", 2);
    QCOMPARE(d.name(), QString());
    QCOMPARE(d.plural(), QString("bar"));
}


void Test_Unit::testGetterSetter()
{
    Unit a;

    a.setId(5);
    QCOMPARE(a.id(), 5);

    a.setName("teaspoon");
    QCOMPARE(a.name(), QString("teaspoon"));

    a.setNameAbbrev("tsp.");
    QCOMPARE(a.nameAbbrev(), QString("tsp."));

    a.setPlural("teaspoons");
    QCOMPARE(a.plural(), QString("teaspoons"));

    a.setPluralAbbrev("tsps.");
    QCOMPARE(a.pluralAbbrev(), QString("tsps."));

    a.setType(Unit::Volume);
    QCOMPARE(a.type(), Unit::Volume);
}

void Test_Unit::testCopyConstructionAndAssignment()
{
    Unit a;
    a.setId(5);
    a.setName("teaspoon");
    a.setNameAbbrev("tsp.");
    a.setPlural("teaspoons");
    a.setPluralAbbrev("tsps.");
    a.setType(Unit::Volume);

    {
        Unit b(a);
        QCOMPARE(b.id(), a.id());
        QCOMPARE(b.name(), a.name());
        QCOMPARE(b.nameAbbrev(), a.nameAbbrev());
        QCOMPARE(b.plural(), a.plural());
        QCOMPARE(b.pluralAbbrev(), a.pluralAbbrev());
        QCOMPARE(b.type(), a.type());
    }

    {
        Unit b;
        b = a;
        QCOMPARE(b.id(), a.id());
        QCOMPARE(b.name(), a.name());
        QCOMPARE(b.nameAbbrev(), a.nameAbbrev());
        QCOMPARE(b.plural(), a.plural());
        QCOMPARE(b.pluralAbbrev(), a.pluralAbbrev());
        QCOMPARE(b.type(), a.type());
    }
}

void Test_Unit::testEqual()
{
    QVERIFY(Unit("litre", "litres", 0) == Unit("litre", "litres", 0));
    QVERIFY(Unit("litre", "litres", 0) == Unit("litre", "litres", 1));
    QVERIFY(Unit("litre", ""      , 0) == Unit("litre", ""      , 0));
    QVERIFY(Unit("litre", "foo"   , 0) == Unit("litre", "bar"   , 0));
    QVERIFY(Unit(""     , "litres", 0) == Unit(""     , "litres", 0));
    QVERIFY(Unit("foo"  , "litres", 0) == Unit("bar"  , "litres", 0));
    QVERIFY(Unit() == Unit());

    QVERIFY(!(Unit("litre", "litres", 0) == Unit("pound", "pounds", 1)));
}

void Test_Unit::testLesser()
{
    QVERIFY(Unit() < Unit("bar", "", 0));
    QVERIFY(Unit("bar", "", 0) < Unit("foo", "", 0));
    QVERIFY(!(Unit("foo", "", 0) < Unit("foo", "", 0)));
}

void Test_Unit::testDetermineName()
{
    Unit a;
    a.setId(5);
    a.setName("teaspoon");
    a.setNameAbbrev("tsp.");
    a.setPlural("teaspoons");
    a.setPluralAbbrev("tsps.");
    a.setType(Unit::Volume);

    QCOMPARE(a.determineName(1, false), QString("teaspoon"));
    QCOMPARE(a.determineName(1, true),  QString("tsp."));
    QCOMPARE(a.determineName(2, false), QString("teaspoons"));
    QCOMPARE(a.determineName(2, true),  QString("tsps."));

    Unit b("foo", "bar");
    QCOMPARE(b.determineName(1, false), QString("foo"));
    QCOMPARE(b.determineName(1, true),  QString("foo"));
    QCOMPARE(b.determineName(2, false), QString("bar"));
    QCOMPARE(b.determineName(2, true),  QString("bar"));
}





QTEST_MAIN(Test_Unit)

#include "unit.moc"
