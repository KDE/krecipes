/***************************************************************************
 *   Copyright © 2006 Jason Kivlighn <jkivlighn@gmail.com>                 *
 *   Copyright © 2010 Martin Engelmann <murphi.oss@googlemail.com>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "weight.h"

class Weight::Private
{
public:
    int id;
    int ingredientID;
    int perAmountUnitID;
    QString perAmountUnit;
    double perAmount;
    int weightUnitID;
    double weight;
    QString weightUnit;
    int prepMethodID;
    QString prepMethod;
};

Weight::Weight()
        : d(new Private())
{
    d->id = -1;
    d->prepMethodID = -1;
}

Weight::Weight(const Weight& rhs)
        : d(new Private(*rhs.d))
{
}

Weight::~Weight()
{
    delete d;
}

Weight& Weight::operator=(const Weight & rhs)
{
    if(this != &rhs) {
        delete d;
        d = new Private(*rhs.d);
    }
    return *this;
}

int Weight::id() const
{
    return d->id;
}

void Weight::setId(int id)
{
    d->id = id;
}

int Weight::ingredientId() const
{
    return d->ingredientID;
}

void Weight::setIngredientId(int ingredientId)
{
    d->ingredientID = ingredientId;
}

int Weight::perAmountUnitId() const
{
    return d->perAmountUnitID;
}

void Weight::setPerAmountUnitId(int perAmountUnitId)
{
    d->perAmountUnitID = perAmountUnitId;
}

QString Weight::perAmountUnit() const
{
    return d->perAmountUnit;
}

void Weight::setPerAmountUnit(const QString& perAmountUnit)
{
    d->perAmountUnit = perAmountUnit;
}

double Weight::perAmount() const
{
    return d->perAmount;
}

void Weight::setPerAmount(double perAmount)
{
    d->perAmount = perAmount;
}

int Weight::weightUnitId() const
{
    return d->weightUnitID;
}

void Weight::setWeightUnitId(int weightUnitId)
{
    d->weightUnitID = weightUnitId;
}

QString Weight::weightUnit() const
{
    return d->weightUnit;
}

void Weight::setWeightUnit(const QString& weightUnit)
{
    d->weightUnit = weightUnit;
}

double Weight::weight() const
{
    return d->weight;
}

void Weight::setWeight(double weight)
{
    d->weight = weight;
}

int Weight::prepMethodId() const
{
    return d->prepMethodID;
}

void Weight::setPrepMethodId(int prepMethodId)
{
    d->prepMethodID = prepMethodId;
}

QString Weight::prepMethod() const
{
    return d->prepMethod;
}

void Weight::setPrepMethod(const QString& prepMethod)
{
    d->prepMethod = prepMethod;
}
