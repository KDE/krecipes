/***************************************************************************
*   Copyright © 2003-2005 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2005 Cyril Bosselut <bosselut@b1project.com>         *
*   Copyright © 2003-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef RATINGLISTEDITOR_H
#define RATINGLISTEDITOR_H

#include "datablocks/rating.h"

#include <QWidget>
#include <QHash>

//custom QScrollArea class
//defined in ratinglisteditor.cpp
class MyScrollArea;

class RecipeDB;
class RatingDisplayWidget;

class KPushButton;

class QScrollArea;
class QFrame;
class QVBoxLayout;

class RatingListEditor : public QWidget
{
Q_OBJECT

public:
	RatingListEditor( RatingList * ratingList, RecipeDB * db,
		QWidget * parent = 0 );

	void refresh();
	void clear();

	void showEvent( QShowEvent * event );

signals:
	void changed();

private slots:
	void addRating();
	void editRating();
	void removeRating();

private:

	KPushButton * m_addRatingButton;

	MyScrollArea * m_scrollArea;
	QFrame * m_frame;
	QVBoxLayout * m_ratingsLayout;

	RatingList * m_ratingList;
	RecipeDB * m_database;
	QHash <RatingDisplayWidget*,RatingList::iterator> m_ratingListIteratorMap;
};

#endif
