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

#ifndef IMAGEDROPLABEL_H
#define IMAGEDROPLABEL_H

#include <QLabel>

class ImageDropLabel : public QLabel
{
	Q_OBJECT

public:
	ImageDropLabel( QWidget *parent = 0 );

	void setPhoto ( QPixmap * photo );
	void refresh();

signals:
	void changed();

protected:
	void dragEnterEvent( QDragEnterEvent* event );
	void dropEvent( QDropEvent* event );

private:
	QPixmap * m_sourcePhoto;
};

#endif
