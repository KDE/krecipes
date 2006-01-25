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

#include <qpainter.h>
#include <qbitmap.h>

#include <kiconloader.h>

QPixmap Rating::starsPixmap( double stars_d, bool include_empty )
{
	int stars = qRound(stars_d * 2); //multiply by two to make it easier to work with half-stars

	QPixmap star = UserIcon(QString::fromLatin1("star_on"));
	QPixmap star_off;
	if ( include_empty )
		star_off = UserIcon(QString::fromLatin1("star_off"));

	int pixmapWidth;
	if ( include_empty )
		pixmapWidth = 18*5;
	else
		pixmapWidth = 18*(stars/2)+((stars%2==1)?9:0);

	QPixmap generatedPixmap(pixmapWidth,18);

	if ( !generatedPixmap.isNull() ) { //there aren't zero stars
		generatedPixmap.fill();
		QPainter painter( &generatedPixmap );

		int pixmapWidth = 18*(stars/2)+((stars%2==1)?9:0);
		if ( include_empty )
			painter.drawTiledPixmap(0,0,18*5,18,star_off); //fill with empty stars
		painter.drawTiledPixmap(0,0,pixmapWidth,18,star); //write over the empty stars to show the rating
	}

	generatedPixmap.setMask( generatedPixmap.createHeuristicMask() );

	return generatedPixmap;
}

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
