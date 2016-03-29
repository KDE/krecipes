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

#include "datainitializepage.h"

#include "widgets/clickablelabel.h"

#include <KLocale>
#include <KStandardDirs>
#include <KHBox>

#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QGroupBox>

DataInitializePage::DataInitializePage( QWidget *parent ) : QWidget( parent )
{
	QGridLayout * layout = new QGridLayout( this );
	layout->cellRect( 1, 1 );
	layout->setMargin( 0 );
	layout->setSpacing( 0 );
	QSpacerItem *spacer_top = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacer_top, 0, 1 );
	QSpacerItem *spacer_left = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_left, 1, 0 );

	// Explanation Text// Widgets
	QLabel *logo;
	//QLabel *serverSetupText;
	//KLineEdit *usernameEdit;
	//KLineEdit *passwordEdit;
	//KLineEdit *dbNameEdit;
	initializeText = new QLabel( this );
	initializeText->setText( i18n( "<p>Krecipes comes with some delicious default recipes and useful data.</p><p>Would you like to initialize your database with those? Note that this will erase all your previous recipes if you have any.</p>" ) );

	initializeText->setAlignment( Qt::AlignTop  );
	initializeText->setWordWrap(true);
	layout->addWidget( initializeText, 1, 3 );

	// Logo
	QPixmap dataInitializePixmap ( KStandardDirs::locate( "data", "krecipes/pics/recipes.png" ) );
	logo = new QLabel( this );
	logo->setPixmap( dataInitializePixmap );
	logo->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( logo, 1, 1, 8, 1, Qt::AlignTop );

	// Spacer to separate the logo
	QSpacerItem *logoSpacer = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( logoSpacer, 1, 2 );

	// Initialize data checkbox
	//FIXME: We need to do a workaround to have word wrapping in QCheckBox.
	//see http://bugreports.qt.nokia.com/browse/QTBUG-5370
	KHBox * initializeContainer = new KHBox(this);
	initializeCheckBox = new QCheckBox( initializeContainer );
	initializeCheckBox->setObjectName( "initializeCheckBox" );
	initializeCheckBox->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	ClickableLabel * initializeLabel = new ClickableLabel(
		i18n( "Yes please, initialize the database with the examples" ),
		initializeContainer );
	initializeLabel->setWordWrap( true );
	connect( initializeLabel, SIGNAL(clicked()), initializeCheckBox, SLOT(click()) );
	layout->addWidget( initializeContainer, 3, 3 );

	QSpacerItem *textInfoSpacer = new QSpacerItem( 0, 50, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( textInfoSpacer, 4, 3 );

	USDAImportText = new QLabel( i18n( "<p>Krecipes can import nutrient data from the USDA's nutrient database for over 400 foods.  A total of 43 food properties are included for each food, such as energy, fat, vitamin C, etc.</p><p>Would you like to import this data now?  Note that this operation is safe to use on an existing database, and no data loss will occur.  This operation may take several minutes.</p>" ), this );
	USDAImportText->setWordWrap(true);
	layout->addWidget( USDAImportText, 5, 3 );

	QSpacerItem *importInfoSpacer = new QSpacerItem( 0, 50, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( importInfoSpacer, 6, 3 );

	//FIXME: We need to do a workaround to have word wrapping in QCheckBox.
	//see http://bugreports.qt.nokia.com/browse/QTBUG-5370
	KHBox * USDAImportContainer = new KHBox(this);
	USDAImportCheckBox = new QCheckBox( USDAImportContainer );
	USDAImportCheckBox->setObjectName( "USDAImportCheckBox" );
	USDAImportCheckBox->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	ClickableLabel * USDAImportLabel = new ClickableLabel(
		i18n("Yes please, load the database with nutrient data for 400+ foods."),
		USDAImportContainer );
	USDAImportLabel->setWordWrap( true );
	connect( USDAImportLabel, SIGNAL(clicked()), USDAImportCheckBox, SLOT(click()) );
	layout->addWidget( USDAImportContainer, 7, 3 );
}

bool DataInitializePage::doInitialization( void )
{
	return ( initializeCheckBox->isChecked() );
}

bool DataInitializePage::doUSDAImport( void )
{
	return ( USDAImportCheckBox->isChecked() );
}

void DataInitializePage::setUSDAImport( bool import )
{
	USDAImportCheckBox->setChecked(import);
}

