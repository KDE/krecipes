/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2009 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*   Copyright © 2010 Martin Engelmann <murphi.oss@googlemail.com>         *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "rating.h"

#include <QPainter>
#include <QBitmap>
#include <QPixmap>

#include <kiconloader.h>
#include "widgets/kratingpainter.h"


QPixmap Rating::starsPixmap(double stars_d)
{
    int stars = qRound(stars_d * 2); //multiply by two to make it easier to work with half-stars

    QPixmap generatedPixmap(90, 18);
    generatedPixmap.fill(Qt::white);

    QRect rect(0, 0, 90, 18);

    QPainter painter(&generatedPixmap);
    KRatingPainter ratingPainter;

    ratingPainter.paint(&painter, rect, stars);
    painter.end();
    generatedPixmap.setMask(generatedPixmap.createMaskFromColor(Qt::white));

    return generatedPixmap;
}

class Rating::Private
{
public:
    int id;
    QString comment;
    QString rater;
    RatingCriteriaList ratingCriteriaList;
};

Rating::Rating()
        : d(new Private())
{
    d->id = -1;
}

Rating::Rating(const Rating& other)
        : d(new Private(*other.d))
{
}

Rating::~Rating()
{
    delete d;
}

Rating& Rating::operator=(const Rating & other)
{
    if (this != &other) {
        delete d;
        d = new Private(*other.d);
    }
    return *this;
}

int Rating::id() const
{
    return d->id;
}

void Rating::setId(int id)
{
    d->id = id;
}

QString Rating::comment() const
{
    return d->comment;
}

void Rating::setComment(const QString& comment)
{
    d->comment = comment;
}

QString Rating::rater() const
{
    return d->rater;
}

void Rating::setRater(const QString& rater)
{
    d->rater = rater;
}

void Rating::append(const RatingCriteria &rc)
{
    d->ratingCriteriaList.append(rc);
}

RatingCriteriaList Rating::ratingCriterias() const
{
    return d->ratingCriteriaList;
}

bool Rating::hasRatingCriterias() const
{
    return d->ratingCriteriaList.size() > 0;
}

int Rating::numberOfRatingCriterias() const
{
    return d->ratingCriteriaList.size();
}

double Rating::sum() const
{
    double result = 0.0;
    foreach(RatingCriteria rc, d->ratingCriteriaList) {
        result += rc.stars();
    }
    return result;
}

double Rating::average() const
{
    if (!hasRatingCriterias()) return -1;

    return sum() / numberOfRatingCriterias();
}

bool Rating::setIdOfRatingCriteria(const QString& name, int id)
{
    bool result = false;

    RatingCriteriaList::iterator it = d->ratingCriteriaList.begin();
    for (; it != d->ratingCriteriaList.end(); ++it) {
        if (it->name() == name) {
            result = true;
            it->setId(id);
            break;
        }
    }

    return result;
}



double RatingList::average()
{
    int rating_total = 0;
    double rating_sum = 0;
    for (RatingList::const_iterator rating_it = constBegin(); rating_it != constEnd(); ++rating_it) {
        rating_total += rating_it->numberOfRatingCriterias();
        rating_sum += rating_it->sum();
    }

    if (rating_total > 0)
        return rating_sum / rating_total;
    else
        return -1;
}
