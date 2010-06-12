/***************************************************************************
 *   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
 *   Copyright © 2010 Martin Engelmann <murphi.oss@googlemail.com>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef RATINGCRITERIA_H
#define RATINGCRITERIA_H


#include <QtCore/QString>
#include <QtCore/QList>


class RatingCriteria
{
private:
    class Private;
    Private * d;

public:
    RatingCriteria();
    RatingCriteria(const RatingCriteria& other);
    ~RatingCriteria();

    RatingCriteria& operator=(const RatingCriteria& other);

    int id() const;
    QString name() const;
    double stars() const;

    void setId(int id);
    void setName(const QString& name);
    void setStars(double stars);
};


typedef QList< RatingCriteria > RatingCriteriaList;


#endif // RATINGCRITERIA_H
