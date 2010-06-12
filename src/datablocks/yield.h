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

class Yield
{
    class Private;
    Private* d;

public:
    Yield();
    Yield(const Yield& other);
    ~Yield();

    Yield& operator=(const Yield& other);

    double amount() const;
    void setAmount(double amount);

    double amountOffset() const;
    void setAmountOffset(double amountOffset);

    QString type() const;
    void setType(const QString& type);

    int typeId() const;
    void setTypeId(int typeId);

    QString amountToString() const;
    QString toString() const;
};


#endif // YIELD_H
