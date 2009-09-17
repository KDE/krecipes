/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef SEARCHPARAMETERS_H
#define SEARCHPARAMETERS_H

#include <QString>
#include <QStringList>
#include <QDateTime>

class RecipeSearchParameters
{
public:
	RecipeSearchParameters() : servings(-1)/*, averageRating(-1), averageRatingOffset(0)*/
	{}

	QStringList titleKeywords;
	bool requireAllTitleWords;

	QStringList instructionsKeywords;
	bool requireAllInstructionsWords;

	QStringList ingsOr;
	QStringList catsOr;
	QStringList authorsOr;

	QTime prep_time;

	/** 0 -> greater than given time
	  * 1 -> less than given time
	  * 2 -> about given time
	  */
	int prep_param;

	int servings;

	/** 0 -> greater than given time
	  * 1 -> less than given time
	  * 2 -> about given time
	  */
	int servings_param;

	QDateTime createdDateBegin;
	QDateTime createdDateEnd;
	QDateTime modifiedDateBegin;
	QDateTime modifiedDateEnd;
	QDateTime accessedDateBegin;
	QDateTime accessedDateEnd;

	//RatingCriteriaList criteriaList;
	//double averageRating;
	//double averageRatingOffset;
};


#endif
