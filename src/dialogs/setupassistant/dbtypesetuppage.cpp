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

#include "dbtypesetuppage.h"

#include <KLocale>
#include <KStandardDirs>

#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QRadioButton>


DBTypeSetupPage::DBTypeSetupPage( QWidget *parent ) : QWidget( parent )
{
	QGridLayout * layout = new QGridLayout( this );
	layout->cellRect( 1, 1 );
	layout->setMargin( 0 );
	layout->setSpacing( 0 );
	QSpacerItem *spacer_top = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacer_top, 0, 1 );
	QSpacerItem *spacer_left = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_left, 1, 0 );


	// Image

	QPixmap serverSetupPixmap ( KStandardDirs::locate( "data", "krecipes/pics/network.png" ) );
	logo = new QLabel( this );
	logo->setPixmap( serverSetupPixmap );
	logo->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( logo, 1, 1, 4, 1, Qt::AlignTop );

	QSpacerItem *spacer_from_image = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_from_image, 1, 2 );


	// Explanation text
	dbTypeSetupText = new QLabel( this );
	dbTypeSetupText->setText( i18n( "Choose the type of database that you want to use. Most users will want to choose a simple local database here. However, you can also use remote servers by means of a MySQL or PostgreSQL database." ) );
	dbTypeSetupText->setWordWrap(true);
	dbTypeSetupText->setAlignment( Qt::AlignTop  );
	layout->addWidget( dbTypeSetupText, 1, 3 );

	// Text spacer

	QSpacerItem* textSpacer = new QSpacerItem( 10, 30, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( textSpacer, 2, 3 );



	// Database type choice
	bg = new QGroupBox( this );
	QVBoxLayout * vbox = new QVBoxLayout;
	layout->addWidget( bg, 3, 3 );

	liteCheckBox = new QRadioButton( i18n( "Simple Local File (SQLite)" ), bg );
	liteCheckBox->setObjectName( "liteCheckBox" );
	vbox->addWidget( liteCheckBox );
	mysqlCheckBox = new QRadioButton( i18n( "Local or Remote MySQL Database" ), bg );
	mysqlCheckBox->setObjectName( "mysqlCheckBox" );
	vbox->addWidget( mysqlCheckBox );
	psqlCheckBox = new QRadioButton( i18n( "Local or Remote PostgreSQL Database" ), bg );
	psqlCheckBox->setObjectName( "psqlCheckBox" );
	vbox->addWidget( psqlCheckBox );
	
	bg->setLayout(vbox);

	liteCheckBox->click();


	QSpacerItem *spacer_bottom = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::MinimumExpanding );
	layout->addItem( spacer_bottom, 4, 3 );

	connect( liteCheckBox, SIGNAL( clicked() ), this, SLOT( setSQLitePages() ) );
	connect( mysqlCheckBox, SIGNAL( clicked() ), this, SLOT( setMySQLPages() ) );
	connect( psqlCheckBox, SIGNAL( clicked() ), this, SLOT( setPostgreSQLPages() ) );

}

int DBTypeSetupPage::dbType( void )
{
	if (mysqlCheckBox->isChecked()) {
		return ( MySQL ); // MySQL (note index=0,1....)
	} else if (psqlCheckBox->isChecked()) {
		return ( PostgreSQL );
	} else { //liteCheckBox->isChecked()
		return ( SQLite );
	}
}

void DBTypeSetupPage::setSQLitePages()
{
	emit showPages( SQLite );
}

void DBTypeSetupPage::setMySQLPages()
{
	emit showPages( MySQL );
}

void DBTypeSetupPage::setPostgreSQLPages()
{
	emit showPages( PostgreSQL );
}

