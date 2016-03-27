/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                         *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>               *
*   Copyright © 2003, 2004, 2006 Jason Kivlighn <jkivlighn@gmail.com>      *
*   Copyright © 2009-2016 José Manuel Santamaría Lema <panfaust@gmail.com> *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#include "performanceprefs.h"

#include <KConfigGroup>
#include <KGlobal>
#include <KLocale>
#include <KIntNumInput>
#include <KHBox>

#include <QVBoxLayout>
#include <QCheckBox>
#include <QLabel>

PerformancePrefs::PerformancePrefs( QWidget *parent )
		: QWidget( parent )
{
	// Load Current Settings
	KConfigGroup config = KGlobal::config()->group( "Performance" );

	int cat_limit = config.readEntry( "CategoryLimit", -1 );
	int limit = config.readEntry( "Limit", -1 );

	Form1Layout = new QVBoxLayout( this );
	Form1Layout->setMargin( 11 );
	Form1Layout->setSpacing( 6 );

	searchAsYouTypeBox = new QCheckBox( i18n( "Search as you type" ), this );
	searchAsYouTypeBox->setChecked( config.readEntry( "SearchAsYouType", true ) );

	QLabel *explainationLabel = new QLabel( i18n("In most instances these options do not need to be changed.  However, limiting the amount of items displayed at once will <b>allow Krecipes to better perform when the database is loaded with many thousands of recipes</b>."), this );
	explainationLabel->setTextFormat( Qt::RichText );
        explainationLabel->setWordWrap( true );

	KHBox *catLimitHBox = new KHBox( this );
	catLimitInput = new KIntNumInput(catLimitHBox);
	catLimitInput->setLabel( i18n( "Number of categories to display at once:" ) );
	catLimitInput->setRange(0,5000,20);
	catLimitInput->setSliderEnabled( true );
	catLimitInput->setSpecialValueText( i18nc("Unlimited categories", "Unlimited") );

	if ( cat_limit > 0 )
		catLimitInput->setValue( cat_limit );

	KHBox *limitHBox = new KHBox( this );
	limitInput = new KIntNumInput(limitHBox);
	limitInput->setLabel( i18n( "Number of elements to display at once:" ) );
	limitInput->setRange(0,100000,1000);
	limitInput->setSliderEnabled( true );
	limitInput->setSpecialValueText( i18nc("Unlimited elements", "Unlimited") );

	if ( limit > 0 )
		limitInput->setValue( limit );

	Form1Layout->addWidget( searchAsYouTypeBox );
	Form1Layout->addWidget( explainationLabel );
	Form1Layout->addWidget( catLimitHBox );
	Form1Layout->addWidget( limitHBox );

	Form1Layout->addItem( new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

	adjustSize();
}

void PerformancePrefs::saveOptions()
{
	KConfigGroup config = KGlobal::config()->group( "Performance" );

	int catLimit = ( catLimitInput->value() == 0 ) ? -1 : catLimitInput->value();
	config.writeEntry( "CategoryLimit", catLimit );

	int limit = ( limitInput->value() == 0 ) ? -1 : limitInput->value();
	config.writeEntry( "Limit", limit );

	config.writeEntry( "SearchAsYouType", searchAsYouTypeBox->isChecked() );
}

