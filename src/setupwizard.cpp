/***************************************************************************
*   Copyright (C) 2003-2004 by                                            *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>
#include <pwd.h>

#include <qhbox.h>
#include <qvgroupbox.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qtooltip.h>

#include <kconfig.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kfiledialog.h>

#include "setupwizard.h"

SetupWizard::SetupWizard( QWidget *parent, const char *name, bool modal, WFlags f ) : KWizard( parent, name, modal, f )
{
	welcomePage = new WelcomePage( this );
	addPage( welcomePage, i18n( "Welcome to Krecipes" ) );

	dbTypeSetupPage = new DBTypeSetupPage( this );
	addPage( dbTypeSetupPage, i18n( "Database Type" ) );

	sqliteSetupPage = new SQLiteSetupPage( this );
	addPage( sqliteSetupPage, i18n( "Server Settings" ) );

	permissionsSetupPage = new PermissionsSetupPage( this );
	addPage( permissionsSetupPage, i18n( "Database Permissions" ) );

	pSqlPermissionsSetupPage = new PSqlPermissionsSetupPage( this );
	addPage( pSqlPermissionsSetupPage, i18n( "Database Permissions" ) );

	serverSetupPage = new ServerSetupPage( this );
	addPage( serverSetupPage, i18n( "Server Settings" ) );

	dataInitializePage = new DataInitializePage( this );
	addPage( dataInitializePage, i18n( "Initialize Database" ) );

	savePage = new SavePage( this );
	addPage( savePage, i18n( "Finish & Save Settings" ) );

	setFinishEnabled( savePage, true ); // Enable finish button
	setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );

#if (HAVE_SQLITE || HAVE_SQLITE3)
	setAppropriate( permissionsSetupPage, false ); // Disable By Default
	setAppropriate( pSqlPermissionsSetupPage, false ); // Disable By Default
	setAppropriate( serverSetupPage, false ); // if we have SQLite (since it's default, and doesn't require these settings)
	setAppropriate( sqliteSetupPage, true );
#else
	setAppropriate( sqliteSetupPage, false );
#endif



	connect( finishButton(), SIGNAL( clicked() ), this, SLOT( save() ) );
	connect( dbTypeSetupPage, SIGNAL( showPages( DBType ) ), this, SLOT( showPages( DBType ) ) );

}


SetupWizard::~SetupWizard()
{
}


void SetupWizard::showPages( DBType type )
{
	switch ( type ) {
	case MySQL:
		setAppropriate( serverSetupPage, true );
		setAppropriate( permissionsSetupPage, true );
		setAppropriate( pSqlPermissionsSetupPage, false );
		setAppropriate( sqliteSetupPage, false );
		break;
	case PostgreSQL:
		setAppropriate( serverSetupPage, true );
		setAppropriate( pSqlPermissionsSetupPage, true );
		setAppropriate( permissionsSetupPage, false );
		setAppropriate( sqliteSetupPage, false );
		break;
	case SQLite:
		setAppropriate( serverSetupPage, false );
		setAppropriate( permissionsSetupPage, false );
		setAppropriate( pSqlPermissionsSetupPage, false );
		setAppropriate( sqliteSetupPage, true );
		break;
	}
}


WelcomePage::WelcomePage( QWidget *parent ) : QWidget( parent )
{
	QGridLayout * layout = new QGridLayout( this, 1, 1, 0, 0 );
	QSpacerItem *spacer_top = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacer_top, 0, 1 );
	QSpacerItem *spacer_left = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_left, 1, 0 );
	QPixmap logoPixmap ( locate( "data", "krecipes/pics/wizard.png" ) );
	logo = new QLabel( this );
	logo->setPixmap( logoPixmap );
	logo->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( logo, 1, 1, Qt::AlignTop );

	QSpacerItem *spacer_from_image = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_from_image, 1, 2 );

	welcomeText = new QLabel( this );
	welcomeText->setText( i18n( "<b><font size=\"+1\">Thank you very much for choosing Krecipes.</font></b><br>It looks like this is the first time you are using it. This wizard will help you with the initial setup so that you can start using it quickly.<br><br>Welcome, and enjoy cooking!" ) );
	welcomeText->setMinimumWidth( 200 );
	welcomeText->setMaximumWidth( 10000 );
	welcomeText->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	welcomeText->setAlignment( int( QLabel::WordBreak | QLabel::AlignTop ) );
	layout->addWidget( welcomeText, 1, 3 );

}

PermissionsSetupPage::PermissionsSetupPage( QWidget *parent ) : QWidget( parent )
{
	QGridLayout * layout = new QGridLayout( this, 1, 1, 0, 0 );
	QSpacerItem *spacer_top = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacer_top, 0, 1 );
	QSpacerItem *spacer_left = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_left, 1, 0 );


	// Logo
	QPixmap permissionsSetupPixmap ( locate( "data", "krecipes/pics/dbpermissions.png" ) );
	logo = new QLabel( this );
	logo->setPixmap( permissionsSetupPixmap );
	logo->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addMultiCellWidget( logo, 1, 8, 1, 1, Qt::AlignTop );

	// Spacer to separate the logo
	QSpacerItem *logoSpacer = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( logoSpacer, 1, 2 );


	// Explanation Text
	permissionsText = new QLabel( this );
	permissionsText->setText( i18n( "This dialog will allow you to specify a MySQL account that has the necessary permissions to access the Krecipes MySQL database.<br><br><b><font size=\"+1\">Most users that use Krecipes and MySQL for the first time can just leave the default parameters and press \'Next\'.</font></b> <br><br>If you set a MySQL root password before, or you have already permissions as normal user, click on the appropriate option. Otherwise the account 'root' will be used, with no password.<br><br>[For security reasons, we strongly encourage you to setup a MySQL root password if you have not done so yet. Just type as root: mysqladmin password <i>your_password</i>]" ) );

	permissionsText->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	permissionsText->setAlignment( int( QLabel::WordBreak | QLabel::AlignTop ) );
	layout->addWidget( permissionsText, 1, 3 );

	// Text spacer
	QSpacerItem *textSpacer = new QSpacerItem( 10, 30, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( textSpacer, 2, 3 );


	// "The user already has permissions" checkbox
	noSetupCheckBox = new QCheckBox( i18n( "I have already set the necessary permissions" ), this, "noSetupCheckBox" );
	layout->addWidget( noSetupCheckBox, 3, 3 );

	QSpacerItem *checkBoxSpacer = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( checkBoxSpacer, 4, 3 );

	// root checkbox
	rootCheckBox = new QCheckBox( i18n( "I have already set a MySQL root/admin account" ), this, "rootCheckBox" );
	layout->addWidget( rootCheckBox, 5, 3 );

	QSpacerItem *rootInfoSpacer = new QSpacerItem( 10, 20, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( rootInfoSpacer, 6, 3 );

	// MySQL root/admin info
	QGroupBox *rootInfoGBox = new QGroupBox( this, "rootInfoGBox" );
	rootInfoGBox->setTitle( i18n( "MySQL Administrator Account" ) );
	rootInfoGBox->setEnabled( false ); // Disable by default
	rootInfoGBox->setColumns( 2 );
	rootInfoGBox->setInsideSpacing( 10 );
	layout->addWidget( rootInfoGBox, 7, 3 );

	// User Entry
	QLabel *userLabel = new QLabel( rootInfoGBox );
	userLabel->setText( i18n( "Username:" ) );
	userEdit = new KLineEdit( rootInfoGBox );
	userEdit->setText( "root" );

	// Password Entry
	QLabel *passLabel = new QLabel( rootInfoGBox );
	passLabel->setText( i18n( "Password:" ) );
	passEdit = new KLineEdit( rootInfoGBox );
	passEdit->setEchoMode( QLineEdit::Password );

	// Bottom spacer
	QSpacerItem *bottomSpacer = new QSpacerItem( 10, 20, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
	layout->addItem( bottomSpacer, 8, 1 );

	// Connect Signals & slots

	connect( rootCheckBox, SIGNAL( toggled( bool ) ), rootInfoGBox, SLOT( setEnabled( bool ) ) );
	connect( rootCheckBox, SIGNAL( toggled( bool ) ), this, SLOT( rootCheckBoxChanged( bool ) ) );
	connect( noSetupCheckBox, SIGNAL( toggled( bool ) ), this, SLOT( noSetupCheckBoxChanged( bool ) ) );
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


PSqlPermissionsSetupPage::PSqlPermissionsSetupPage( QWidget *parent ) : QWidget( parent )
{
	QGridLayout * layout = new QGridLayout( this, 1, 1, 0, 0 );
	QSpacerItem *spacer_top = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacer_top, 0, 1 );
	QSpacerItem *spacer_left = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_left, 1, 0 );


	// Logo
	QPixmap permissionsSetupPixmap ( locate( "data", "krecipes/pics/dbpermissions.png" ) );
	logo = new QLabel( this );
	logo->setPixmap( permissionsSetupPixmap );
	logo->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addMultiCellWidget( logo, 1, 8, 1, 1, Qt::AlignTop );

	// Spacer to separate the logo
	QSpacerItem *logoSpacer = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( logoSpacer, 1, 2 );


	// Explanation Text
	permissionsText = new QLabel( this );
	permissionsText->setText( i18n( "This dialog will allow you to specify a PostgreSQL account that has the necessary permissions to access the Krecipes PostgreSQL database.  This account may either be a <b>PostgreSQL superuser</b> or have the ability to both <b>create new PostgreSQL users and databases</b>.<br><br>If no superuser or privileged account is given, the account 'postgres' will be attempted, with no password.  If this is insufficient for your PostgreSQL setup, you <b>must</b> select the appropriate option below to enter the information of a privileged PostgreSQL account." ) );

	permissionsText->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	permissionsText->setAlignment( int( QLabel::WordBreak | QLabel::AlignTop ) );
	layout->addWidget( permissionsText, 1, 3 );

	// Text spacer
	QSpacerItem *textSpacer = new QSpacerItem( 10, 30, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( textSpacer, 2, 3 );


	// "The user already has permissions" checkbox
	noSetupCheckBox = new QCheckBox( i18n( "I have already set the necessary permissions" ), this, "noSetupCheckBox" );
	layout->addWidget( noSetupCheckBox, 3, 3 );

	QSpacerItem *checkBoxSpacer = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( checkBoxSpacer, 4, 3 );

	// root checkbox
	rootCheckBox = new QCheckBox( i18n( "I have already set a superuser or privileged account" ), this, "rootCheckBox" );
	layout->addWidget( rootCheckBox, 5, 3 );

	QSpacerItem *rootInfoSpacer = new QSpacerItem( 10, 20, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( rootInfoSpacer, 6, 3 );

	// MySQL root/admin info
	QGroupBox *rootInfoGBox = new QGroupBox( this, "rootInfoGBox" );
	rootInfoGBox->setTitle( i18n( "PostgreSQL Superuser or Privileged Account" ) );
	rootInfoGBox->setEnabled( false ); // Disable by default
	rootInfoGBox->setColumns( 2 );
	rootInfoGBox->setInsideSpacing( 10 );
	layout->addWidget( rootInfoGBox, 7, 3 );

	// User Entry
	QLabel *userLabel = new QLabel( rootInfoGBox );
	userLabel->setText( i18n( "Username:" ) );
	userEdit = new KLineEdit( rootInfoGBox );
	userEdit->setText( "postgres" );

	// Password Entry
	QLabel *passLabel = new QLabel( rootInfoGBox );
	passLabel->setText( i18n( "Password:" ) );
	passEdit = new KLineEdit( rootInfoGBox );
	passEdit->setEchoMode( QLineEdit::Password );

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


ServerSetupPage::ServerSetupPage( QWidget *parent ) : QWidget( parent )
{
	QGridLayout * layout = new QGridLayout( this, 1, 1, 0, 0 );
	QSpacerItem *spacer_top = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacer_top, 0, 1 );
	QSpacerItem *spacer_left = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_left, 1, 0 );


	// Image

	QPixmap serverSetupPixmap ( locate( "data", "krecipes/pics/network.png" ) );
	logo = new QLabel( this );
	logo->setPixmap( serverSetupPixmap );
	logo->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addMultiCellWidget( logo, 1, 8, 1, 1, Qt::AlignTop );

	QSpacerItem *spacer_from_image = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_from_image, 1, 2 );


	// Explanation text
	serverSetupText = new QLabel( this );
	serverSetupText->setText( i18n( "In this dialog you can adjust the database server settings." ) );
	serverSetupText->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	serverSetupText->setAlignment( int( QLabel::AlignTop | QLabel::AlignJustify ) );
	layout->addWidget( serverSetupText, 1, 3 );

	// Text spacer

	QSpacerItem* textSpacer = new QSpacerItem( 10, 30, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( textSpacer, 2, 3 );

	// Input Boxes

	QGroupBox *inputGBox = new QGroupBox( this, "inputGBox" );
	inputGBox->setFrameStyle( QFrame::NoFrame );
	inputGBox->setInsideSpacing( 10 );
	inputGBox->setColumns( 2 );
	layout->addWidget( inputGBox, 3, 3 );

	// Username Input

	QLabel* usernameText = new QLabel( i18n( "Username:" ), inputGBox );
	usernameText->setFixedSize( QSize( 100, 20 ) );
	usernameText->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

	usernameEdit = new KLineEdit( inputGBox );
	usernameEdit->setFixedSize( QSize( 120, 20 ) );
	usernameEdit->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	// get username
	uid_t userID;
	QString username;
	struct passwd *user;
	userID = getuid();
	user = getpwuid ( userID );
	username = user->pw_name;

	usernameEdit->setText( username );


	// Password

	QLabel* passwordText = new QLabel( i18n( "Password:" ), inputGBox );
	passwordText->setFixedSize( QSize( 100, 20 ) );
	passwordText->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

	passwordEdit = new KLineEdit( inputGBox );
	passwordEdit->setEchoMode( QLineEdit::Password );
	passwordEdit->setFixedSize( QSize( 120, 20 ) );
	passwordEdit->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

	// DB Name

	QLabel* dbNameText = new QLabel( i18n( "Database name:" ), inputGBox );
	dbNameText->setFixedSize( QSize( 100, 20 ) );
	dbNameText->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

	dbNameEdit = new KLineEdit( inputGBox );
	dbNameEdit->setFixedSize( QSize( 120, 20 ) );
	dbNameEdit->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	dbNameEdit->setText( "Krecipes" );


	// Spacer from box
	QSpacerItem* spacerFromBox = new QSpacerItem( 10, 20, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerFromBox, 4, 3 );


	// Remote server checkbox

	remoteServerCheckBox = new QCheckBox( i18n( "The server is remote" ), this, "remoteServerCheckBox" );
	layout->addWidget( remoteServerCheckBox, 5, 3 );

	// Spacer from CheckBox
	QSpacerItem* spacerFromCheckBox = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerFromCheckBox, 6, 3 );

	// Server & Client Box
	QGroupBox *serverSettingsGBox = new QGroupBox( this, "serverSettingsGBox" );
	serverSettingsGBox->setTitle( i18n( "Server / Client Settings" ) );
	serverSettingsGBox->setEnabled( false ); // Disable by default
	serverSettingsGBox->setInsideSpacing( 10 );
	serverSettingsGBox->setColumns( 2 );
	layout->addWidget( serverSettingsGBox, 7, 3 );


	// Server
	( void ) new QLabel( i18n( "Server:" ), serverSettingsGBox );
	serverEdit = new KLineEdit( serverSettingsGBox );
	serverEdit->setText( "localhost" );

	// Client
	( void ) new QLabel( i18n( "Client:" ), serverSettingsGBox );
	clientEdit = new KLineEdit( serverSettingsGBox );
	clientEdit->setText( "localhost" );

	// Bottom Spacers

	QSpacerItem* bottomSpacer = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
	layout->addItem( bottomSpacer, 8, 1 );

	QSpacerItem* spacerRight = new QSpacerItem( 10, 10, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
	layout->addItem( spacerRight, 1, 6 );

	// Signals & Slots
	connect( remoteServerCheckBox, SIGNAL( toggled( bool ) ), serverSettingsGBox, SLOT( setEnabled( bool ) ) );

}

QString ServerSetupPage::server( void )
{
	return ( this->serverEdit->text() );
}

QString ServerSetupPage::user( void )
{
	return ( this->usernameEdit->text() );
}

QString ServerSetupPage::password( void )
{
	return ( this->passwordEdit->text() );
}

QString ServerSetupPage::dbName( void )
{
	return ( this->dbNameEdit->text() );
	;
}

void ServerSetupPage::getServerInfo( bool &isRemote, QString &host, QString &client, QString &dbName, QString &user, QString &pass )
{
	isRemote = remoteServerCheckBox->isChecked();
	host = serverEdit->text();
	client = clientEdit->text();
	user = usernameEdit->text();
	pass = passwordEdit->text();
	dbName = this->dbNameEdit->text();
}

SQLiteSetupPage::SQLiteSetupPage( QWidget *parent ) : QWidget( parent )
{
	QGridLayout * layout = new QGridLayout( this, 1, 1, 0, 0 );
	QSpacerItem *spacer_top = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacer_top, 0, 1 );
	QSpacerItem *spacer_left = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_left, 1, 0 );


	// Image

	QPixmap serverSetupPixmap ( locate( "data", "krecipes/pics/network.png" ) );
	logo = new QLabel( this );
	logo->setPixmap( serverSetupPixmap );
	logo->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addMultiCellWidget( logo, 1, 4, 1, 1, Qt::AlignTop );

	QSpacerItem *spacer_from_image = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_from_image, 1, 2 );


	// Explanation text
	serverSetupText = new QLabel( this );
	serverSetupText->setText( i18n( "In this dialog you can adjust SQLite settings." ) );
	serverSetupText->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	serverSetupText->setAlignment( int( QLabel::AlignTop | QLabel::AlignJustify ) );
	layout->addWidget( serverSetupText, 1, 3 );

	// Text spacer

	QSpacerItem* textSpacer = new QSpacerItem( 10, 30, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( textSpacer, 2, 3 );

	// Input Boxes

	QHBox *hbox = new QHBox( this );

	( void ) new QLabel( i18n( "Database file:" ), hbox );

	fileEdit = new KLineEdit( hbox );
	fileEdit->setText( locateLocal ( "appdata", "krecipes.krecdb" ) );
	hbox->setStretchFactor( fileEdit, 2 );

	KIconLoader il;
	QPushButton *file_select = new QPushButton( il.loadIcon( "fileopen", KIcon::NoGroup, 16 ), QString::null, hbox );
	QToolTip::add
		( file_select, i18n( "Open file dialog" ) );
	file_select->setFixedWidth( 25 );

	layout->addWidget( hbox, 3, 3 );

	// Bottom Spacers

	QSpacerItem* bottomSpacer = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
	layout->addItem( bottomSpacer, 4, 1 );

	connect( file_select, SIGNAL( clicked() ), SLOT( selectFile() ) );
}

QString SQLiteSetupPage::dbFile( void ) const
{
	return ( fileEdit->text() );
}

void SQLiteSetupPage::selectFile()
{
	KFileDialog dialog( QString::null, "*.*|All Files", this, "dialog", true );
	if ( dialog.exec() == QDialog::Accepted ) {
		fileEdit->setText( dialog.selectedFile() );
	}
}


SavePage::SavePage( QWidget *parent ) : QWidget( parent )
{
	QGridLayout * layout = new QGridLayout( this, 1, 1, 0, 0 );
	QSpacerItem *spacer_top = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacer_top, 0, 1 );
	QSpacerItem *spacer_left = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_left, 1, 0 );

	QPixmap logoPixmap ( locate( "data", "krecipes/pics/save.png" ) );
	logo = new QLabel( this );
	logo->setPixmap( logoPixmap );
	logo->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( logo, 1, 1, Qt::AlignTop );

	QSpacerItem *spacer_from_image = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_from_image, 1, 2 );

	saveText = new QLabel( this );
	saveText->setText( i18n( "Congratulations; all the necessary configuration setup is done. Press 'Finish' to continue, and enjoy cooking!" ) );
	saveText->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );

	saveText->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
	layout->addWidget( saveText, 1, 3 );


}

void SetupWizard::save( void )
{
	kdDebug() << "Setting parameters in kconfig..." << endl;
	KConfig *config = kapp->config();

	// Save the database type
	QString sDBType;

	switch ( dbTypeSetupPage->dbType() ) {
	case MySQL:
		sDBType = "MySQL";
		break;
	case PostgreSQL:
		sDBType = "PostgreSQL";
		break;
	default:
		sDBType = "SQLite";
		break;
	}

	config->setGroup( "DBType" );
	config->writeEntry( "Type", sDBType );
	kdDebug() << "DB type set in kconfig was... " << sDBType << endl;
	// Save the server data if needed
	if ( !( dbTypeSetupPage->dbType() == SQLite ) ) {
		config->setGroup( "Server" );
		config->writeEntry( "Host", serverSetupPage->server() );
		config->writeEntry( "Username", serverSetupPage->user() );
		config->writeEntry( "Password", serverSetupPage->password() );
		config->writeEntry( "DBName", serverSetupPage->dbName() );
		kdDebug() << "Finished setting the database parameters for MySQL or PostgreSQL (non SQLite)..." << endl;
	}
	else {
		config->setGroup( "Server" );
		config->writeEntry( "DBFile", sqliteSetupPage->dbFile() );
	}

	// Indicate that settings were already made

	config->setGroup( "Wizard" );
	config->writeEntry( "SystemSetup", true );
	config->writeEntry( "Version", "0.6" );
	kdDebug() << "Setting in kconfig the lines to disable wizard startup..." << sDBType << endl;
}

void SetupWizard::getOptions( bool &setupUser, bool &initializeData, bool &doUSDAImport )
{
	setupUser = permissionsSetupPage->doUserSetup() && pSqlPermissionsSetupPage->doUserSetup();
	initializeData = dataInitializePage->doInitialization();
	doUSDAImport = dataInitializePage->doUSDAImport();
}

void SetupWizard::getAdminInfo( bool &enabled, QString &adminUser, QString &adminPass, const QString &dbType )
{
	enabled = permissionsSetupPage->useAdmin() || pSqlPermissionsSetupPage->useAdmin();
	if ( dbType == "MySQL" )
		permissionsSetupPage->getAdmin( adminUser, adminPass );
	else
		pSqlPermissionsSetupPage->getAdmin( adminUser, adminPass );
}

void SetupWizard::getServerInfo( bool &isRemote, QString &host, QString &client, QString &dbName, QString &user, QString &pass )
{
	serverSetupPage->getServerInfo( isRemote, host, client, dbName, user, pass );
}

DataInitializePage::DataInitializePage( QWidget *parent ) : QWidget( parent )
{
	QGridLayout * layout = new QGridLayout( this, 1, 1, 0, 0 );
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
	initializeText->setText( i18n( "Krecipes comes with some delicious default recipes and useful data. <br><br>Would you like to initialize your database with those? Note that this will erase all your previous recipes if you have any. " ) );

	initializeText->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	initializeText->setAlignment( int( QLabel::WordBreak | QLabel::AlignTop ) );
	layout->addWidget( initializeText, 1, 3 );

	// Logo
	QPixmap dataInitializePixmap ( locate( "data", "krecipes/pics/recipes.png" ) );
	logo = new QLabel( this );
	logo->setPixmap( dataInitializePixmap );
	logo->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addMultiCellWidget( logo, 1, 8, 1, 1, Qt::AlignTop );

	// Spacer to separate the logo
	QSpacerItem *logoSpacer = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( logoSpacer, 1, 2 );

	// Initialize data checkbox

	initializeCheckBox = new QCheckBox( i18n( "Yes please, initialize the database with the examples" ), this, "initializeCheckBox" );
	layout->addWidget( initializeCheckBox, 3, 3 );

	QSpacerItem *textInfoSpacer = new QSpacerItem( 0, 50, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( textInfoSpacer, 4, 3 );

	USDAImportText = new QLabel( i18n( "Krecipes can import nutrient data from the USDA's nutrient database for over 400 foods.  A total of 43 food properties are included for each food, such as energy, fat, vitamin C, etc.<br><br>Would you like to import this data now?  Note that this operation is safe to use on an existing database, and no data loss will occur.  This operation may take several minutes." ), this );
	layout->addWidget( USDAImportText, 5, 3 );

	QSpacerItem *importInfoSpacer = new QSpacerItem( 0, 50, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( importInfoSpacer, 6, 3 );

	USDAImportCheckBox = new QCheckBox( i18n( "Yes please, load the database with nutrient data for 400+ foods. (Note: English only.)" ), this, "USDAImportCheckBox" );
	layout->addWidget( USDAImportCheckBox, 7, 3 );
}

bool DataInitializePage::doInitialization( void )
{
	return ( initializeCheckBox->isChecked() );
}

bool DataInitializePage::doUSDAImport( void )
{
	return ( USDAImportCheckBox->isChecked() );
}

DBTypeSetupPage::DBTypeSetupPage( QWidget *parent ) : QWidget( parent )
{
	QGridLayout * layout = new QGridLayout( this, 1, 1, 0, 0 );
	QSpacerItem *spacer_top = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacer_top, 0, 1 );
	QSpacerItem *spacer_left = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_left, 1, 0 );


	// Image

	QPixmap serverSetupPixmap ( locate( "data", "krecipes/pics/network.png" ) );
	logo = new QLabel( this );
	logo->setPixmap( serverSetupPixmap );
	logo->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addMultiCellWidget( logo, 1, 4, 1, 1, Qt::AlignTop );

	QSpacerItem *spacer_from_image = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_from_image, 1, 2 );


	// Explanation text
	dbTypeSetupText = new QLabel( this );
	dbTypeSetupText->setText( i18n( "Choose the type of database that you want to use. Most users will want to choose a simple local database here. However, you can also use remote servers by means of a MySQL or PostgreSQL database." ) );
	dbTypeSetupText->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	dbTypeSetupText->setAlignment( int( QLabel::AlignTop | QLabel::WordBreak ) );
	layout->addWidget( dbTypeSetupText, 1, 3 );

	// Text spacer

	QSpacerItem* textSpacer = new QSpacerItem( 10, 30, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( textSpacer, 2, 3 );



	// Database type choice
	bg = new QVButtonGroup( this );
	layout->addWidget( bg, 3, 3 );

	liteCheckBox = new QRadioButton( i18n( "Simple Local File (SQLite)" ), bg, "liteCheckBox" );
	mysqlCheckBox = new QRadioButton( i18n( "Local or Remote MySQL Database" ), bg, "liteCheckBox" );
	psqlCheckBox = new QRadioButton( i18n( "Local or Remote PostgreSQL Database" ) + i18n(" (**BETA**)"), bg, "psqlCheckBox" );
	bg->setButton( 0 ); // By default, SQLite

#if (!HAVE_MYSQL)
	mysqlCheckBox->setEnabled( false );
#endif

#if (!HAVE_POSTGRESQL)
	psqlCheckBox->setEnabled( false );
#endif

#if (!(HAVE_SQLITE || HAVE_SQLITE3))
	liteCheckBox->setEnabled( false );
#if (HAVE_MYSQL)

	bg->setButton( 1 ); // Otherwise by default liteCheckBox is checked even if it's disabled
#else
	#if (HAVE_POSTGRESQL)

	bg->setButton( 2 );
#endif
	#endif
#endif


	QSpacerItem *spacer_bottom = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::MinimumExpanding );
	layout->addItem( spacer_bottom, 4, 3 );

	connect( bg, SIGNAL( clicked( int ) ), this, SLOT( setPages( int ) ) );

}

int DBTypeSetupPage::dbType( void )
{
	//int id=bg->selectedId(); //QT 3.2
	int id = bg->id( bg->selected() ); //QT 3.1

	switch ( id ) {
	case 1:
		return ( MySQL ); // MySQL (note index=0,1....)
	case 2:
		return ( PostgreSQL );
	default:
		return ( SQLite );
	}
}

/*
** hides/shows pages given the radio button state
*/

void DBTypeSetupPage::setPages( int rb )
{
	switch ( rb ) {
	case 1:
		emit showPages( MySQL );
		break;
	case 2:
		emit showPages( PostgreSQL );
		break;
	default:
		emit showPages( SQLite );
		break;
	}
}

#include "setupwizard.moc"
