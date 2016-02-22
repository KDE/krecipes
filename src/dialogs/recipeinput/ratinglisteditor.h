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

#include <QWidget>

class RatingList;
class RecipeDB;

class KWidgetListbox;
class KPushButton;

class RatingListEditor : public QWidget
{
Q_OBJECT

public:
	RatingListEditor( RatingList * ratingList, RecipeDB * db,
		QWidget * parent = 0 );

	void refresh();
	void clear();

signals:
	void changed();

private slots:
	void addRating();
	void editRating();
	void removeRating();

private:
	KWidgetListbox * m_ratingListDisplayWidget;
	KPushButton * m_addRatingButton;

	RatingList * m_ratingList;
	RecipeDB * m_database;
};

#endif
