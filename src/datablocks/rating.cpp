/***************************************************************************
*   Copyright (C) 2005 by Jason Kivlighn                                  *
*   jkivlighn@gmail.com                                                   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "rating.h"

void Rating::append( const RatingCriteria &rc )
{
	ratingCriteriaList.append( rc );
}

double Rating::average() const
{
	double sum = 0;
	int count = 0;
	for ( RatingCriteriaList::const_iterator rc_it = ratingCriteriaList.begin(); rc_it != ratingCriteriaList.end(); ++rc_it ) {
		count++;
		sum += (*rc_it).stars;
	}

	if ( count > 0 )
		return sum/count;
	else
		return -1;
}