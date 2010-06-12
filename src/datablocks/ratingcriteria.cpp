/***************************************************************************
 *   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
 *   Copyright © 2010 Martin Engelmann <murphi.oss@googlemail.com>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include "ratingcriteria.h"

class RatingCriteria::Private
{
public:
    int id;
    QString name;
    double stars;
};


RatingCriteria::RatingCriteria()
        : d(new Private())
{
    d->id = -1;
    d->stars = 0.0;
}

RatingCriteria::RatingCriteria(const RatingCriteria& other)
        : d(new Private(*other.d))
{
}


RatingCriteria::~RatingCriteria()
{
    delete d;
}

RatingCriteria& RatingCriteria::operator=(const RatingCriteria & other)
{
    if (this != &other) {
        delete d;
        d = new Private(*other.d);
    }

    return *this;
}

int RatingCriteria::id() const
{
    return d->id;
}

QString RatingCriteria::name() const
{
    return d->name;
}

double RatingCriteria::stars() const
{
    return d->stars;
}

void RatingCriteria::setId(int id)
{
    d->id = id;
}

void RatingCriteria::setName(const QString& name)
{
    d->name = name;
}

void RatingCriteria::setStars(double stars)
{
    d->stars = stars;
}
