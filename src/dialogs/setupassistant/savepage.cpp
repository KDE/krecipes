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

#include "savepage.h"

#include <KLocale>
#include <KStandardDirs>

#include <QGridLayout>
#include <QLabel>

SavePage::SavePage( QWidget *parent ) : QWidget( parent )
{
	QGridLayout * layout = new QGridLayout( this );
	layout->cellRect( 1, 1 );
	layout->setMargin( 0 );
	layout->setSpacing( 0 );
	QSpacerItem *spacer_top = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacer_top, 0, 1 );
	QSpacerItem *spacer_left = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_left, 1, 0 );

	QPixmap logoPixmap ( KStandardDirs::locate( "data", "krecipes/pics/save.png" ) );
	logo = new QLabel( this );
	logo->setPixmap( logoPixmap );
	logo->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( logo, 1, 1, Qt::AlignTop );

	QSpacerItem *spacer_from_image = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_from_image, 1, 2 );

	saveText = new QLabel( this );
	saveText->setText( i18n( "Congratulations; all the necessary configuration setup is done. Press 'Finish' to continue, and enjoy cooking!" ) );
	saveText->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
	saveText->setWordWrap(true);
	saveText->setAlignment( Qt::AlignVCenter );
	layout->addWidget( saveText, 1, 3 );


}

