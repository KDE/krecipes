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

#include "serversetuppage.h"

#include <KLocale>
#include <KStandardDirs>
#include <KLineEdit>
#include <KIntNumInput>

#include <QGridLayout>
#include <QPixmap>
#include <QGroupBox>
#include <QFormLayout>
#include <QCheckBox>
#include <QLabel>

#include <unistd.h>
#include <pwd.h>


ServerSetupPage::ServerSetupPage( QWidget *parent ) : QWidget( parent )
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
	layout->addWidget( logo, 1, 1, 8, 1, Qt::AlignTop );

	QSpacerItem *spacer_from_image = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_from_image, 1, 2 );


	// Explanation text
	serverSetupText = new QLabel( this );
	serverSetupText->setText( i18n( "<p>In this dialog you can adjust the database server settings.</p><p><b>Warning: Passwords are stored in plain text and could potentially be compromised.  We recommend that you create a username and password combination solely for use by Krecipes.</b></p>" ) );
	serverSetupText->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	serverSetupText->setAlignment( Qt::AlignTop  );
	serverSetupText->setWordWrap(true);
	layout->addWidget( serverSetupText, 1, 3 );

	// Text spacer

	QSpacerItem* textSpacer = new QSpacerItem( 10, 30, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( textSpacer, 2, 3 );

	// Input Boxes

	QGroupBox *inputGBox = new QGroupBox( this );
	QFormLayout *inputLayout = new QFormLayout;
	inputGBox->setLayout( inputLayout );
	layout->addWidget( inputGBox, 3, 3 );

	// Username Input

	usernameEdit = new KLineEdit( inputGBox );
	// get username
	uid_t userID;
	QString username;
	struct passwd *user;
	userID = getuid();
	user = getpwuid ( userID );
	username = user->pw_name;

	usernameEdit->setText( username );
	inputLayout->addRow( i18nc( "@label:textbox Database Username", "Username:" ), usernameEdit );


	// Password

	passwordEdit = new KLineEdit( inputGBox );
	passwordEdit->setEchoMode( QLineEdit::Password );
	inputLayout->addRow( i18n( "Password:" ), passwordEdit );


	// DB Name

	dbNameEdit = new KLineEdit( inputGBox );
	dbNameEdit->setText( "Krecipes" );
	inputLayout->addRow( i18n( "Database name:" ), dbNameEdit );


	// Spacer from box
	QSpacerItem* spacerFromBox = new QSpacerItem( 10, 20, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerFromBox, 4, 3 );


	// Remote server checkbox

	remoteServerCheckBox = new QCheckBox( i18n( "The server is remote" ), this );
	remoteServerCheckBox->setObjectName( "remoteServerCheckBox" );
	layout->addWidget( remoteServerCheckBox, 5, 3 );

	// Spacer from CheckBox
	QSpacerItem* spacerFromCheckBox = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerFromCheckBox, 6, 3 );

	// Server & Client Box
	QGroupBox *serverSettingsGBox = new QGroupBox( this );
	QFormLayout *serverSettingsLayout = new QFormLayout;
	serverSettingsGBox->setLayout( serverSettingsLayout );
	serverSettingsGBox->setTitle( i18n( "Server / Client Settings" ) );
	serverSettingsGBox->setEnabled( false ); // Disable by default
	layout->addWidget( serverSettingsGBox, 7, 3 );


	// Server
	serverEdit = new KLineEdit( serverSettingsGBox );
	serverEdit->setText( "localhost" );
	serverSettingsLayout->addRow( i18n( "Server:" ), serverEdit );

	// Client
	clientEdit = new KLineEdit( serverSettingsGBox );
	clientEdit->setText( "localhost" );
	serverSettingsLayout->addRow( i18n( "Client:" ), clientEdit );

	//Port
	portEdit = new KIntNumInput( serverSettingsGBox );
	portEdit->setMinimum(0);
	portEdit->setSpecialValueText( i18nc("@label:textbox Default Port", "Default") );
	serverSettingsLayout->addRow( i18n( "Port:" ), portEdit );

	// Bottom Spacers

	QSpacerItem* bottomSpacer = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
	layout->addItem( bottomSpacer, 8, 1 );

	//QSpacerItem* spacerRight = new QSpacerItem( 10, 10, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
	//layout->addItem( spacerRight, 7, 4 );

	// Signals & Slots
	connect( remoteServerCheckBox, SIGNAL( toggled( bool ) ), serverSettingsGBox, SLOT( setEnabled( bool ) ) );

}

QString ServerSetupPage::server( void ) const
{
	return ( serverEdit->text() );
}

QString ServerSetupPage::user( void ) const
{
	return ( usernameEdit->text() );
}

QString ServerSetupPage::password( void ) const
{
	return ( passwordEdit->text() );
}

QString ServerSetupPage::dbName( void ) const
{
	return ( dbNameEdit->text() );
}

int ServerSetupPage::port( void ) const
{
	return ( portEdit->value() );
}

void ServerSetupPage::getServerInfo( bool &isRemote, QString &host, QString &client, QString &dbName, QString &user, QString &pass, int &port )
{
	isRemote = remoteServerCheckBox->isChecked();
	host = serverEdit->text();
	client = clientEdit->text();
	user = usernameEdit->text();
	pass = passwordEdit->text();
	dbName = dbNameEdit->text();
	port = portEdit->value();
}

