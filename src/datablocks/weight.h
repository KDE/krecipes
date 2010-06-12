/***************************************************************************
 *   Copyright © 2006 Jason Kivlighn <jkivlighn@gmail.com>                 *
 *   Copyright © 2010 Martin Engelmann <murphi.oss@googlemail.com>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef WEIGHT_H
#define WEIGHT_H

#include <QString>
#include <QList>

#include "datablocks/elementlist.h"

class Weight
{
    class Private;
    Private * d;

public:
    Weight();
    Weight(const Weight & rhs);
    ~Weight();

    Weight& operator=(const Weight & rhs);

    int id() const;
    void setId(int id);

    int ingredientId() const;
    void setIngredientId(int ingredientId);

    int perAmountUnitId() const;
    void setPerAmountUnitId(int perAmountUnitId);

    QString perAmountUnit() const;
    void setPerAmountUnit(const QString & perAmountUnit);

    double perAmount() const;
    void setPerAmount(double perAmount);

    int weightUnitId() const;
    void setWeightUnitId(int weightUnitId);

    QString weightUnit() const;
    void setWeightUnit(const QString & weightUnit);

    double weight() const;
    void setWeight(double weight);

    int prepMethodId() const;
    void setPrepMethodId(int prepMethodId);

    QString prepMethod() const;
    void setPrepMethod(const QString & prepMethod);
};

typedef QList<Weight> WeightList;

#endif
