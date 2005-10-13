/***************************************************************************
*   Copyright (C) 2005 by Jason Kivlighn                                  *
*   jkivlighn@gmail.com                                                   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef RATING_H
#define RATING_H

#include <qvaluelist.h>
#include <qstring.h>

#include <qpixmap.h>

class RatingCriteria
{
public:
	RatingCriteria() : id(-1), stars(0.0){}

	int id;
	QString name;
	double stars;
};

typedef QValueList< RatingCriteria > RatingCriteriaList;

class Rating
{
public:
	Rating() : id(-1){}

	static QPixmap starsPixmap( double stars_d, bool include_empty = false );

	void append( const RatingCriteria & );

	double average() const;

	int id;
	QString comment;
	QString rater;

	RatingCriteriaList ratingCriteriaList;
};

typedef QValueList< Rating > RatingList;

#endif
