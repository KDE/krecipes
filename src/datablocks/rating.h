/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2010 Martin Engelmann <murphi.oss@googlemail.com>         *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef RATING_H
#define RATING_H

#include <QtCore/QList>
#include <QtCore/QString>

#include <QtGui/QPixmap>

#include "ratingcriteria.h"

class Rating
{
public:
    static QPixmap starsPixmap(double stars_d);

private:
    class Private;
    Private* d;

public:
    Rating();
    Rating(const Rating& other);
    ~Rating();

    Rating& operator=(const Rating& other);

    int id() const;
    void setId(int id);

    QString comment() const;
    void setComment(const QString & comment);

    QString rater() const;
    void setRater(const QString & rater);

    void append(const RatingCriteria &);
    RatingCriteriaList ratingCriterias() const;

    bool hasRatingCriterias() const;
    int numberOfRatingCriterias() const;
    double sum() const;
    double average() const;

    bool setIdOfRatingCriteria(const QString & name, int id);
};

class RatingList : public QList< Rating >
{
public:
    double average();
};

#endif
