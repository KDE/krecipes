/***************************************************************************
*   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                    *
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>             *
*   Copyright © 2009-2016 José Manuel Santamaría Lema <panfaust@gmail.com> *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#ifndef SAVEPAGE_H
#define SAVEPAGE_H

#include <QWidget>

class QLabel;

class SavePage: public QWidget
{
public:
	// Methods
	SavePage( QWidget *parent );
private:
	// Widgets
	QLabel *logo;
	QLabel *saveText;

};

#endif
