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

#ifndef YIELD_H
#define YIELD_H

#include <QtCore/QString>

#include "deprecated.h"

class MixedNumberRange;

class Yield
{
    class Private;
    Private* d;

public:
    Yield();
    Yield(const Yield& other);
    ~Yield();

    Yield& operator=(const Yield& other);

    //Use amountRange and setAmountRange instead
    /* KRECIPES_DEPRECATED */ double amount() const;
    /* KRECIPES_DEPRECATED */ void setAmount(double amount);

    //Use amountRange and setAmountRange instead
    /* KRECIPES_DEPRECATED */ double amountOffset() const;
    /* KRECIPES_DEPRECATED */ void setAmountOffset(double amountOffset);

    MixedNumberRange amountRange() const;
    void setAmountRange( const MixedNumberRange & amount );

    QString type() const;
    void setType(const QString& type);

    int typeId() const;
    void setTypeId(int typeId);

    QString amountToString() const;
    QString toString() const;
};


#endif // YIELD_H
