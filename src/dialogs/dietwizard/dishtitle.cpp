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

#include "dishtitle.h"

#include <KGlobalSettings>

#include <QPainter>
#include <QSize>

DishTitle::DishTitle( QWidget *parent, const QString &title ) : QWidget( parent )
{
	titleText = title;
}


DishTitle::~DishTitle()
{}

void DishTitle::paintEvent( QPaintEvent * )
{
	QPainter painter( this );

	// First draw the decoration
	painter.setPen( KGlobalSettings::activeTitleColor() );
	painter.setBrush( QBrush( KGlobalSettings::activeTitleColor() ) );
	painter.drawRoundRect( 0, 20, 30, height() - 40, 50, ( int ) ( 50.0 / ( height() - 40 ) * 35.0 ) );

	// Now draw the text
	QFont titleFont = KGlobalSettings::windowTitleFont ();
	titleFont.setPointSize( 15 );
	painter.setFont( titleFont );
	painter.rotate( -90 );
	painter.setPen( palette().color( QPalette::WindowText  ));
	painter.drawText( 0, 0, -height(), 30, Qt::AlignCenter, titleText );
	painter.setPen( palette().color( QPalette::Window  ));
	painter.drawText( -1, -1, -height() - 1, 29, Qt::AlignCenter, titleText );
	painter.end();
}

QSize DishTitle::sizeHint () const
{
	return ( QSize( 40, 200 ) );
}

QSize DishTitle::minimumSizeHint() const
{
	return ( QSize( 40, 200 ) );
}

