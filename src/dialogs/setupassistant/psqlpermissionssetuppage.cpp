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

#include "psqlpermissionssetuppage.h"

#include "widgets/clickablelabel.h"

#include <KLocale>
#include <KStandardDirs>
#include <KLineEdit>
#include <KHBox>

#include <QGridLayout>
#include <QPixmap>
#include <QLabel>
#include <QCheckBox>
#include <QFormLayout>
#include <QGroupBox>


PSqlPermissionsSetupPage::PSqlPermissionsSetupPage( QWidget *parent ) : QWidget( parent )
{
	QGridLayout * layout = new QGridLayout( this );
	layout->cellRect( 1, 1 );
	layout->setMargin( 0 );
	layout->setSpacing( 0 );
	QSpacerItem *spacer_top = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacer_top, 0, 1 );
	QSpacerItem *spacer_left = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_left, 1, 0 );


	// Logo
	QPixmap permissionsSetupPixmap ( KStandardDirs::locate( "data", "krecipes/pics/dbpermissions.png" ) );
	logo = new QLabel( this );
	logo->setPixmap( permissionsSetupPixmap );
	logo->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( logo, 1, 1, 8, 1, Qt::AlignTop );

	// Spacer to separate the logo
	QSpacerItem *logoSpacer = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( logoSpacer, 1, 2 );


	// Explanation Text
	permissionsText = new QLabel( this );
	permissionsText->setText( i18n( "<p>This dialog will allow you to specify a PostgreSQL account that has the necessary permissions to access the Krecipes PostgreSQL database.  This account may either be a <b>PostgreSQL superuser</b> or have the ability to both <b>create new PostgreSQL users and databases</b>.</p><p>If no superuser or privileged account is given, the account 'postgres' will be attempted, with no password.  If this is insufficient for your PostgreSQL setup, you <b>must</b> select the appropriate option below to enter the information of a privileged PostgreSQL account.</p>" ) );
	permissionsText->setWordWrap(true);
	permissionsText->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	permissionsText->setAlignment( Qt::AlignTop );
	layout->addWidget( permissionsText, 1, 3 );

	// Text spacer
	QSpacerItem *textSpacer = new QSpacerItem( 10, 30, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( textSpacer, 2, 3 );


	// "The user already has permissions" checkbox
	//FIXME: We need to do a workaround to have word wrapping in QCheckBox.
	//see http://bugreports.qt.nokia.com/browse/QTBUG-5370
	KHBox * noSetupContainer = new KHBox;
	noSetupCheckBox = new QCheckBox( noSetupContainer );
	noSetupCheckBox->setObjectName( "noSetupCheckBox" );
	noSetupCheckBox->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	ClickableLabel * noSetupLabel = new ClickableLabel(
		i18n( "I have already set the necessary permissions" ),
		noSetupContainer );
	noSetupLabel->setWordWrap( true );
	connect( noSetupLabel, SIGNAL(clicked()), noSetupCheckBox, SLOT(click()) );
	layout->addWidget( noSetupContainer, 3, 3 );

	QSpacerItem *checkBoxSpacer = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( checkBoxSpacer, 4, 3 );

	// root checkbox
	//FIXME: We need to do a workaround to have word wrapping in QCheckBox.
	//see http://bugreports.qt.nokia.com/browse/QTBUG-5370
	KHBox * rootContainer = new KHBox;
	rootCheckBox = new QCheckBox( rootContainer );
	rootCheckBox->setObjectName( "rootCheckBox" );
	rootCheckBox->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	ClickableLabel * rootLabel = new ClickableLabel(
		i18n( "I have already set a superuser or privileged account" ),
		rootContainer );
	rootLabel->setWordWrap( true );
	connect( rootLabel, SIGNAL(clicked()), rootCheckBox, SLOT(click()) );
	layout->addWidget( rootContainer, 5, 3 );


	QSpacerItem *rootInfoSpacer = new QSpacerItem( 10, 20, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( rootInfoSpacer, 6, 3 );

	// PostgreSQL root/admin info
	QGroupBox *rootInfoGBox = new QGroupBox( this );
	QFormLayout *rootInfoLayout = new QFormLayout;
	rootInfoGBox->setTitle( i18n( "PostgreSQL Superuser or Privileged Account" ) );
	rootInfoGBox->setEnabled( false ); // Disable by default
	// User Entry
	userEdit = new KLineEdit( rootInfoGBox );
	userEdit->setText( "postgres" );
	rootInfoLayout->addRow( i18nc("@label:textbox Database Username", "Username:" ), userEdit );
	// Password Entry
	passEdit = new KLineEdit( rootInfoGBox );
	passEdit->setEchoMode( QLineEdit::Password );
	rootInfoLayout->addRow( i18n( "Password:" ), passEdit );

	rootInfoGBox->setLayout( rootInfoLayout );
	layout->addWidget( rootInfoGBox, 7, 3 );

	// Bottom spacer
	QSpacerItem *bottomSpacer = new QSpacerItem( 10, 20, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
	layout->addItem( bottomSpacer, 8, 1 );

	// Connect Signals & slots

	connect( rootCheckBox, SIGNAL( toggled( bool ) ), rootInfoGBox, SLOT( setEnabled( bool ) ) );
	connect( rootCheckBox, SIGNAL( toggled( bool ) ), this, SLOT( rootCheckBoxChanged( bool ) ) );
	connect( noSetupCheckBox, SIGNAL( toggled( bool ) ), this, SLOT( noSetupCheckBoxChanged( bool ) ) );
}

void PSqlPermissionsSetupPage::rootCheckBoxChanged( bool on )
{
	if ( on )
		noSetupCheckBox->setChecked( false ); // exclude mutually the options (both can be unset)
}

bool PSqlPermissionsSetupPage::doUserSetup()
{
	return ( !noSetupCheckBox->isChecked() );
}

bool PSqlPermissionsSetupPage::useAdmin()
{
	return ( rootCheckBox->isChecked() );
}

void PSqlPermissionsSetupPage::getAdmin( QString &adminName, QString &adminPass )
{
	adminName = userEdit->text();
	adminPass = passEdit->text();
}

void PSqlPermissionsSetupPage::noSetupCheckBoxChanged( bool on )
{
	if ( on )
		rootCheckBox->setChecked( false ); // exclude mutually the options (both can be unset)
}

