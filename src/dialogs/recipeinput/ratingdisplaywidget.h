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

#ifndef RATINGDISPLAYWIDGET_H
#define RATINGDISPLAYWIDGET_H

#include "ui_ratingdisplaywidget.h"

#include "datablocks/rating.h"

class Rating;

namespace Ui {
	class RatingDisplayWidget;
}

class RatingDisplayWidget : public QWidget
{
Q_OBJECT

public:
	RatingDisplayWidget( QWidget *parent = 0);
	RatingList::iterator rating_it;

	void displayRating(const Rating & rating);

signals:
	void editButtonClicked();
	void removeButtonClicked();

private:
	Ui::RatingDisplayWidget * ui;

};

#endif
