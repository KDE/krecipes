/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef DISHTITLE_H
#define DISHTITLE_H

#include <QWidget>

class DishTitle: public QWidget
{
	Q_OBJECT

public:
	DishTitle( QWidget *parent, const QString &title );
	~DishTitle();
	virtual QSize sizeHint () const;
	virtual QSize minimumSizeHint() const;
protected:
	//Variables
	QString titleText;
	//Methods
	virtual void paintEvent( QPaintEvent *p );
};

#endif
