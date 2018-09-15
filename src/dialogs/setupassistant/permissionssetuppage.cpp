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

#include "permissionssetuppage.h"

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


PermissionsSetupPage::PermissionsSetupPage( QWidget *parent ) : QWidget( parent )
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
	permissionsText->setText( i18n( "<p>This dialog will allow you to specify a MySQL account that has the necessary permissions to access the Krecipes MySQL database.</p><p><b><font size=\"+1\">Most users that use Krecipes and MySQL for the first time can just leave the default parameters and press \'Next\'.</font></b></p> <p>If you set a MySQL root password before, or you have already permissions as normal user, click on the appropriate option. Otherwise the account 'root' will be used, with no password.</p><p>[For security reasons, we strongly encourage you to setup a MySQL root password if you have not done so yet. Just type as root: mysqladmin password <i>your_password</i>]</p>" ) );
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
		i18n( "I have already set a MySQL root/admin account" ),
		rootContainer );
	rootLabel->setWordWrap( true );
	connect( rootLabel, SIGNAL(clicked()), rootCheckBox, SLOT(click()) );
	layout->addWidget( rootContainer, 5, 3 );

	QSpacerItem *rootInfoSpacer = new QSpacerItem( 10, 20, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( rootInfoSpacer, 6, 3 );

	// MySQL root/admin info
	QGroupBox *rootInfoGBox = new QGroupBox( this );
	QFormLayout *rootInfoLayout = new QFormLayout;
	rootInfoGBox->setTitle( i18n( "MySQL Administrator Account" ) );
	rootInfoGBox->setEnabled( false ); // Disable by default
	// User Entry
	userEdit = new KLineEdit( rootInfoGBox );
	userEdit->setText( "root" );
	rootInfoLayout->addRow( i18nc("@label:textbox Database Username", "Username:" ), userEdit );
	// Password Entry
	passEdit = new KLineEdit( rootInfoGBox );
	passEdit->setEchoMode( QLineEdit::Password );
	rootInfoLayout->addRow( i18n( "Password:" ), passEdit );
	rootInfoGBox->setLayout(rootInfoLayout);
	layout->addWidget( rootInfoGBox, 7, 3 );

	// Bottom spacer
	QSpacerItem *bottomSpacer = new QSpacerItem( 10, 20, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
	layout->addItem( bottomSpacer, 8, 1 );

	// Connect Signals & slots

	connect( rootCheckBox, SIGNAL(toggled(bool)), rootInfoGBox, SLOT(setEnabled(bool)) );
	connect( rootCheckBox, SIGNAL(toggled(bool)), this, SLOT(rootCheckBoxChanged(bool)) );
	connect( noSetupCheckBox, SIGNAL(toggled(bool)), this, SLOT(noSetupCheckBoxChanged(bool)) );
}

void PermissionsSetupPage::rootCheckBoxChanged( bool on )
{
	if ( on )
		noSetupCheckBox->setChecked( false ); // exclude mutually the options (both can be unset)
}

bool PermissionsSetupPage::doUserSetup()
{
	return ( !noSetupCheckBox->isChecked() );
}

bool PermissionsSetupPage::useAdmin()
{
	return ( rootCheckBox->isChecked() );
}

void PermissionsSetupPage::getAdmin( QString &adminName, QString &adminPass )
{
	adminName = userEdit->text();
	adminPass = passEdit->text();
}

void PermissionsSetupPage::noSetupCheckBoxChanged( bool on )
{
	if ( on )
		rootCheckBox->setChecked( false ); // exclude mutually the options (both can be unset)
}

