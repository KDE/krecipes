/***************************************************************************
*   Copyright © 2010 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "clickablelabel.h"

#include <QMouseEvent>


ClickableLabel::ClickableLabel( QWidget *parent, Qt::WindowFlags f)
	: QLabel( parent, f )
{
}

ClickableLabel::ClickableLabel( const QString & text, QWidget *parent, Qt::WindowFlags f)
	: QLabel( text, parent, f )
{
}

void ClickableLabel::mousePressEvent(QMouseEvent * e)
{
	if (e->button() == Qt::LeftButton ) {
		down = true;
	}
}

void ClickableLabel::mouseReleaseEvent(QMouseEvent * e)
{
	if ( (e->button() == Qt::LeftButton) && down ) {
		down = false;
		emit clicked();
	}
}

#include "clickablelabel.moc"
