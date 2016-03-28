/*****************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>        *
*                                                                            *
*   This program is free software; you can redistribute it and/or modify     *
*   it under the terms of the GNU General Public License as published by     *
*   the Free Software Foundation; either version 2 of the License, or        *
*   (at your option) any later version.                                      *
******************************************************************************/

#include "kresplashscreen.h"

#include "krecipes_version.h"

#include <QPalette>
#include <QLabel>

KreSplashScreen::KreSplashScreen(): QSplashScreen() {

	//Set the pixmap background
	QPixmap pixmap( ":/startlogo.png" );
	setPixmap( pixmap );

	//Set up the QLabel's showing the program version
	QPalette labelPalette;
	labelPalette.setColor( QPalette::WindowText, Qt::white );
	labelPalette.setColor( QPalette::Window, Qt::black );

	//Major version
	QLabel * majorVerLabel = new QLabel( this );
	majorVerLabel->setPalette( labelPalette );
	majorVerLabel->setGeometry( QRect(520, 20, 66, 41) );
	majorVerLabel->setAlignment( Qt::AlignCenter );
	QFont font;
	font.setFamily( "Sans Serif" );
	font.setPointSize( 25 );
	font.setBold( true );
	font.setWeight( 75 );
	majorVerLabel->setFont( font );
	majorVerLabel->setText( KRESPLASH_VERSION_MAJOR );

	//Revision
	QLabel * revLabel = new QLabel( this );
	revLabel->setPalette( labelPalette );
	revLabel->setGeometry( QRect(520, 50, 66, 31) );
	revLabel->setAlignment( Qt::AlignCenter );
	font.setFamily( "Sans Serif" );
	font.setPointSize( 10 );
	font.setBold( true );
	revLabel->setFont( font );
	revLabel->setText( KRESPLASH_VERSION_REVISION );

}
