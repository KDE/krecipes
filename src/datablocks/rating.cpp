/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2009 José Manuel Santamaría Lema <panfaust@gmail.com>     *
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
#include <nepomuk/kratingpainter.h>

QPixmap Rating::starsPixmap( double stars_d )
{
	int stars = qRound(stars_d * 2); //multiply by two to make it easier to work with half-stars

	QPixmap generatedPixmap(90,18);
	generatedPixmap.fill( Qt::white );

	QRect rect(0,0,90,18);

	QPainter painter( &generatedPixmap );
	KRatingPainter ratingPainter;

	ratingPainter.paint( &painter, rect, stars );
	painter.end();
	generatedPixmap.setMask( generatedPixmap.createMaskFromColor( Qt::white ) );

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


double RatingList::average()
{
	int rating_total = 0;
	double rating_sum = 0;
	for ( RatingList::const_iterator rating_it = begin(); rating_it != end(); ++rating_it ) {
		for ( RatingCriteriaList::const_iterator rc_it = (*rating_it).ratingCriteriaList.begin(); rc_it != (*rating_it).ratingCriteriaList.end(); ++rc_it ) {
			rating_total++;
			rating_sum += (*rc_it).stars;
		}
	}

	if ( rating_total > 0 )
		return rating_sum/rating_total;
	else
		return -1;
}
