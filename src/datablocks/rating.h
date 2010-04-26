/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef RATING_H
#define RATING_H

#include <QList>
#include <QString>

#include <QPixmap>

class RatingCriteria
{
public:
	RatingCriteria() : id(-1), stars(0.0){}

	int id;
	QString name;
	double stars;
};

typedef QList< RatingCriteria > RatingCriteriaList;

class Rating
{
public:
	Rating() : id(-1){}

	static QPixmap starsPixmap( double stars_d );

	void append( const RatingCriteria & );

	double average() const;

	int id;
	QString comment;
	QString rater;

	RatingCriteriaList ratingCriteriaList;
};

class RatingList : public QList< Rating >
{
public:
	double average();
};

#endif
