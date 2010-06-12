/***************************************************************************
 *   Copyright © 2004-2006 Jason Kivlighn <jkivlighn@gmail.com>            *
 *   Copyright © 2010 Martin Engelmann <murphi.oss@googlemail.com>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef UNIT_H
#define UNIT_H

#include <QString>
#include <QList>

class Unit
{
    class Private;
    Private* d;

public:
    typedef enum { All = -1, Other = 0, Mass, Volume } Type;

    Unit();
    Unit(const QString &name, const QString &plural, int id = -1);

    /** Use @param amount to determine whether to use @param name as the plural or singlular form */
    Unit(const QString &name, double amount);
    Unit(const Unit& rhs);
    ~Unit();

    Unit& operator=(const Unit& rhs);
    bool operator==(const Unit &u) const;
    bool operator<(const Unit &u) const;

    Type type() const;
    void setType(Type type);

    int id() const;
    void setId(int id);

    QString name() const;
    void setName(const QString & name);

    QString plural() const;
    void setPlural(const QString & plural);

    QString nameAbbrev() const;
    void setNameAbbrev(const QString & nameAbbrev);

    QString pluralAbbrev() const;
    void setPluralAbbrev(const QString & pluralAbbrev);

    QString determineName(double amount, bool useAbbrev) const;
};

typedef QList< Unit > UnitList;

#endif //UNIT_H
