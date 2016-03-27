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

#include "numberprefs.h"

#include <KConfigGroup>
#include <KGlobal>
#include <KLocale>

#include <QVBoxLayout>
#include <QRadioButton>
#include <QGroupBox>
#include <QCheckBox>


NumbersPrefs::NumbersPrefs( QWidget *parent )
		: QWidget( parent )
{
	Form1Layout = new QVBoxLayout( this );
	Form1Layout->setMargin( 11 );
	Form1Layout->setSpacing( 6 );

	numberButtonGroup = new QGroupBox( this );
	numberButtonGroupLayout = new QVBoxLayout;
	numberButtonGroup->setLayout( numberButtonGroupLayout );

	fractionRadioButton = new QRadioButton( numberButtonGroup );
	numberButtonGroupLayout->addWidget( fractionRadioButton );

	decimalRadioButton = new QRadioButton( numberButtonGroup );
	numberButtonGroupLayout->addWidget( decimalRadioButton );
	Form1Layout->addWidget( numberButtonGroup );

	//unit display format
	QGroupBox *abbrevGrpBox = new QGroupBox;
	abbrevGrpBox->setTitle( i18n( "Units" ) );
	abbrevButton = new QCheckBox( i18n( "Use abbreviations" ) );
	QGridLayout *abbrevLayout = new QGridLayout;
	abbrevLayout->addWidget( abbrevButton );
	abbrevGrpBox->setLayout( abbrevLayout );
	Form1Layout->addWidget( abbrevGrpBox );


	Form1Layout->addItem( new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

	adjustSize();

	languageChange();

	// Load Current Settings
	KConfigGroup config = KGlobal::config()->group( "Formatting" );

	if ( config.readEntry( "Fraction", true ) )
		fractionRadioButton->click();
	else
		decimalRadioButton->click();

	abbrevButton->setChecked( config.readEntry( "AbbreviateUnits", false ) );
}

void NumbersPrefs::saveOptions()
{
	KConfigGroup config = KGlobal::config()->group( "Formatting" );

	bool fraction = fractionRadioButton->isChecked();
	config.writeEntry( "Fraction", fraction );

	config.writeEntry( "AbbreviateUnits", abbrevButton->isChecked() );
}

void NumbersPrefs::languageChange()
{
	numberButtonGroup->setTitle( i18n( "Number Format" ) );
	fractionRadioButton->setText( i18nc("@option:radio Display fraction", "Fraction" ) );
	decimalRadioButton->setText( i18nc("@option:radio Display decimal", "Decimal" ) );
}

