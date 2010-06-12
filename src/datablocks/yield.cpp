/***************************************************************************
 *   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                   *
 *   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>            *
 *   Copyright © 2010 Martin Engelmann <murphi.oss@googlemail.com>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "yield.h"

class Yield::Private
{
public:
    double amount;
    double amount_offset;
    QString type;
    int type_id;
};


Yield::Yield()
        : d(new Private())
{
    d->amount = 0;
    d->amount_offset = 0;
    d->type_id = -1;
}


Yield::Yield(const Yield& other)
        : d(new Private(*other.d))
{
}


Yield::~Yield()
{
    delete d;
}

Yield& Yield::operator=(const Yield & other)
{
    if (this != &other) {
        delete d;
        d = new Private(*other.d);
    }
    return *this;
}

double Yield::amount() const
{
    return d->amount;
}

void Yield::setAmount(double amount)
{
    d->amount = amount;
}

double Yield::amountOffset() const
{
    return d->amount_offset;
}

void Yield::setAmountOffset(double amountOffset)
{
    d->amount_offset = amountOffset;
}

QString Yield::type() const
{
    return d->type;
}

void Yield::setType(const QString& type)
{
    d->type = type;
}

int Yield::typeId() const
{
    return d->type_id;
}

void Yield::setTypeId(int typeId)
{
    d->type_id = typeId;
}

QString Yield::amountToString() const
{
    QString ret = QString::number(d->amount);
    if (d->amount_offset > 0)
        ret += '-' + QString::number(d->amount + d->amount_offset);

    return ret;
}

QString Yield::toString() const
{
    return amountToString() + ' ' + d->type;
}

