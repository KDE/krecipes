/***************************************************************************
 *   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
 *   Copyright © 2010 Martin Engelmann <murphi.oss@googlemail.com>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "unit.h"

class Unit::Private
{
public:
    Type type;
    int id;
    QString name;
    QString plural;

    QString name_abbrev;
    QString plural_abbrev;
};

Unit::Unit()
        : d(new Private())
{
    d->type = Unit::Other;
    d->id = -1;
}

Unit::Unit(const QString &_name, const QString &_plural, int _id)
        : d(new Private())
{
    d->type = Unit::Other;
    d->id = _id;
    d->name = _name;
    d->plural = _plural;
}

Unit::Unit(const QString &_name, double amount)
        : d(new Private())
{
    d->type = Unit::Other;
    if (amount > 1)
        d->plural = _name;
    else
        d->name = _name;
}

Unit::Unit(const Unit& rhs)
        : d(new Private(*rhs.d))
{
}

Unit::~Unit()
{
    delete d;
}

Unit& Unit::operator=(const Unit & rhs)
{
    if (this != &rhs) {
        delete d;
        d = new Private(*rhs.d);
    }
    return *this;
}

Unit::Type Unit::type() const
{
    return d->type;
}

void Unit::setType(Unit::Type type)
{
    d->type = type;
}

int Unit::id() const
{
    return d->id;
}

void Unit::setId(int id)
{
    d->id = id;
}

QString Unit::name() const
{
    return d->name;
}

void Unit::setName(const QString& name)
{
    d->name = name;
}

QString Unit::plural() const
{
    return d->plural;
}

void Unit::setPlural(const QString& plural)
{
    d->plural = plural;
}

QString Unit::nameAbbrev() const
{
    return d->name_abbrev;
}

void Unit::setNameAbbrev(const QString& nameAbbrev)
{
    d->name_abbrev = nameAbbrev;
}

QString Unit::pluralAbbrev() const
{
    return d->plural_abbrev;
}

void Unit::setPluralAbbrev(const QString& pluralAbbrev)
{
    d->plural_abbrev = pluralAbbrev;
}

QString Unit::determineName(double amount, bool useAbbrev) const
{
    if (useAbbrev) {
        QString unit = (amount > 1) ? d->plural_abbrev : d->name_abbrev;
        if (unit.isEmpty())
            unit = (amount > 1) ? d->plural : d->name;
        return unit;
    } else
        return (amount > 1) ? d->plural : d->name;
}

bool Unit::operator==(const Unit &u) const
{
    //treat QString() and "" as the same
    QString plural_test1 = u.d->plural.toLower();
    if (plural_test1.isEmpty())
        plural_test1 = "";

    QString plural_test2 = d->plural.toLower();
    if (plural_test2.isEmpty())
        plural_test2 = "";

    QString single_test1 = u.d->name.toLower();
    if (single_test1.isEmpty())
        single_test1 = "";

    QString single_test2 = d->name.toLower();
    if (single_test2.isEmpty())
        single_test2 = "";

    if (plural_test1.isEmpty() && plural_test2.isEmpty() && single_test1.isEmpty() && single_test2.isEmpty())
        return true;
    else if (plural_test1.isEmpty() && plural_test2.isEmpty())
        return single_test1 == single_test2;
    else if (single_test1.isEmpty() && single_test2.isEmpty())
        return plural_test1 == plural_test2;
    else
        return (plural_test1 == plural_test2 || single_test1 == single_test2);
}

bool Unit::operator<(const Unit &u) const
{
    return (QString::localeAwareCompare(d->name.toLower(), u.d->name.toLower()) < 0);
}
