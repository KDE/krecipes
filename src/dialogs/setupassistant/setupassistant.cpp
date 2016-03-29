/***************************************************************************
*   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#include "setupassistant.h"

#include <unistd.h>
#include <pwd.h>

#include <QGroupBox>
#include <QPixmap>
#include <QPushButton>
#include <QGridLayout>
#include <QFormLayout>
#include <QLabel>

#include <kconfig.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kglobal.h>
#include <kvbox.h>

#include "backends/usda_ingredient_data.h"
#include "widgets/clickablelabel.h"

SetupAssistant::SetupAssistant( QWidget *parent, Qt::WFlags f ) : KAssistantDialog(parent, f )
{

	//Don't show the help button, it's not useful at the moment.
	showButton(KDialog::Help, false);

	welcomePage = new WelcomePage( this );
	welcomePageItem = new KPageWidgetItem( welcomePage, i18n( "Welcome to Krecipes" ) );
	addPage( welcomePageItem );

	dbTypeSetupPage = new DBTypeSetupPage( this );
	dbTypeSetupPageItem = new KPageWidgetItem( dbTypeSetupPage, i18n( "Database Type" ) );
	addPage( dbTypeSetupPageItem );

	sqliteSetupPage = new SQLiteSetupPage( this );
	sqliteSetupPageItem = new KPageWidgetItem( sqliteSetupPage, i18n( "Database Management System Settings" ) );
	addPage( sqliteSetupPageItem );

	permissionsSetupPage = new PermissionsSetupPage( this );
	permissionsSetupPageItem = new KPageWidgetItem( permissionsSetupPage, i18n( "Database Permissions" ) );
	addPage( permissionsSetupPageItem );

	pSqlPermissionsSetupPage = new PSqlPermissionsSetupPage( this );
	pSqlPermissionsSetupPageItem = new KPageWidgetItem( pSqlPermissionsSetupPage, i18n( "Database Permissions" ) );
	addPage( pSqlPermissionsSetupPageItem );

	serverSetupPage = new ServerSetupPage( this );
	serverSetupPageItem = new KPageWidgetItem( serverSetupPage, i18n( "Server Settings" ) );
	addPage( serverSetupPageItem );

	dataInitializePage = new DataInitializePage( this );
	dataInitializePageItem = new KPageWidgetItem( dataInitializePage, i18n( "Initialize Database" ) );
	addPage( dataInitializePageItem );

	savePage = new SavePage( this );
	savePageItem = new KPageWidgetItem( savePage, i18n( "Finish & Save Settings" ) );
	addPage( savePageItem );

	setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );

	showPages( SQLite );

	connect( dbTypeSetupPage, SIGNAL( showPages( DBType ) ), this, SLOT( showPages( DBType ) ) );
}


SetupAssistant::~SetupAssistant()
{
}

void SetupAssistant::next()
{
	if ( (currentPage() == dataInitializePageItem) && dataInitializePage->doUSDAImport() ) {
		if ( !USDA::localizedIngredientsAvailable() ) {
			switch ( KMessageBox::warningYesNo( this, i18n("There is currently no localized ingredient data for this locale.\nWould you like to load the English ingredients instead?") ) ) {
			case KMessageBox::No: dataInitializePage->setUSDAImport(false);
			default: break;
			}
		}
	}

	KAssistantDialog::next();
}


void SetupAssistant::showPages( DBType type )
{
	switch ( type ) {
	case MySQL:
		setAppropriate( serverSetupPageItem, true );
		setAppropriate( permissionsSetupPageItem, true );
		setAppropriate( pSqlPermissionsSetupPageItem, false );
		setAppropriate( sqliteSetupPageItem, false );
		break;
	case PostgreSQL:
		setAppropriate( serverSetupPageItem, true );
		setAppropriate( pSqlPermissionsSetupPageItem, true );
		setAppropriate( permissionsSetupPageItem, false );
		setAppropriate( sqliteSetupPageItem, false );
		break;
	case SQLite:
		setAppropriate( serverSetupPageItem, false );
		setAppropriate( permissionsSetupPageItem, false );
		setAppropriate( pSqlPermissionsSetupPageItem, false );
		setAppropriate( sqliteSetupPageItem, true );
		break;
	}
}


WelcomePage::WelcomePage( QWidget *parent ) : QWidget( parent )
{
	QGridLayout * layout = new QGridLayout( this );
	layout->cellRect( 1, 1 );
	layout->setMargin( 0 );
	layout->setSpacing( 0 );
	QSpacerItem *spacer_top = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacer_top, 0, 1 );
	QSpacerItem *spacer_left = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_left, 1, 0 );
	QPixmap logoPixmap ( KStandardDirs::locate( "data", "krecipes/pics/wizard.png" ) );
	logo = new QLabel( this );
	logo->setPixmap( logoPixmap );
	logo->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( logo, 1, 1, Qt::AlignTop );

	QSpacerItem *spacer_from_image = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_from_image, 1, 2 );

	welcomeText = new QLabel( this );
	welcomeText->setText( i18n( "<p><b><font size=\"+1\">Thank you very much for choosing Krecipes.</font></b></p><p>It looks like this is the first time you are using it. This wizard will help you with the initial setup so that you can start using it quickly.</p><p>Welcome, and enjoy cooking!</p>" ) );
	welcomeText->setWordWrap(true);
	welcomeText->setAlignment( Qt::AlignTop );
	layout->addWidget( welcomeText, 1, 3 );

}

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

SQLiteSetupPage::SQLiteSetupPage( QWidget *parent ) : QWidget( parent )
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
	serverSetupText = new QLabel( this );
	serverSetupText->setText( i18n( "In this dialog you can adjust SQLite settings." ) );
	serverSetupText->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	serverSetupText->setAlignment( Qt::AlignTop | Qt::AlignJustify );
	layout->addWidget( serverSetupText, 1, 3 );

	// Text spacer

	QSpacerItem* textSpacer = new QSpacerItem( 10, 30, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( textSpacer, 2, 3 );

	// Input Boxes

	KHBox *hbox = new KHBox( this );

	( void ) new QLabel( i18n( "Database file:" ), hbox );

	fileEdit = new KLineEdit( hbox );
	fileEdit->setText( KStandardDirs::locateLocal ( "appdata", "krecipes.krecdb" ) );
	hbox->setStretchFactor( fileEdit, 2 );

	KIconLoader *il = KIconLoader::global();
	QPushButton *file_select = new QPushButton( il->loadIcon( "document-open", KIconLoader::NoGroup, 16 ), QString(), hbox );
	file_select->setToolTip( i18n( "Open file dialog" ) );
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
	QPointer<KFileDialog> dialog = new KFileDialog( KUrl(), "*.*|All Files", this );
	dialog->setObjectName( "dialog" );
	dialog->setCaption( i18n( "Select SQLite database file") );
	dialog->setModal( true );
	if ( dialog->exec() == QDialog::Accepted ) {
		fileEdit->setText( dialog->selectedFile() );
	}
	delete dialog;
}


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

void SetupAssistant::accept( void )
{
	kDebug() << "Setting parameters in kconfig..." ;

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

	KConfigGroup config = KGlobal::config()->group( "DBType" );
	config.writeEntry( "Type", sDBType );
	kDebug() << "DB type set in kconfig was... " << sDBType ;
	// Save the server data if needed
	if ( !( dbTypeSetupPage->dbType() == SQLite ) ) {
		config = KGlobal::config()->group( "Server" );
		config.writeEntry( "Host", serverSetupPage->server() );
		config.writeEntry( "Username", serverSetupPage->user() );
		config.writeEntry( "Password", serverSetupPage->password() );
		config.writeEntry( "DBName", serverSetupPage->dbName() );
		config.writeEntry( "Port", serverSetupPage->port() );
		kDebug() << "Finished setting the database parameters for MySQL or PostgreSQL (non SQLite)..." ;
	}
	else {
		config = KGlobal::config()->group( "Server" );
		config.writeEntry( "DBFile", sqliteSetupPage->dbFile() );
	}

	// Indicate that settings were already made

	config = KGlobal::config()->group( "Wizard" );
	config.writeEntry( "SystemSetup", true );
	config.writeEntry( "Version", "0.9" );
	kDebug() << "Setting in kconfig the lines to disable wizard startup..." << sDBType ;

	done(QDialog::Accepted);
}

void SetupAssistant::getOptions( bool &setupUser, bool &initializeData, bool &doUSDAImport )
{
	setupUser = permissionsSetupPage->doUserSetup() && pSqlPermissionsSetupPage->doUserSetup();
	initializeData = dataInitializePage->doInitialization();
	doUSDAImport = dataInitializePage->doUSDAImport();
}

void SetupAssistant::getAdminInfo( bool &enabled, QString &adminUser, QString &adminPass, const QString &dbType )
{
	enabled = permissionsSetupPage->useAdmin() || pSqlPermissionsSetupPage->useAdmin();
	if ( dbType == "MySQL" )
		permissionsSetupPage->getAdmin( adminUser, adminPass );
	else
		pSqlPermissionsSetupPage->getAdmin( adminUser, adminPass );
}

void SetupAssistant::getServerInfo( bool &isRemote, QString &host, QString &client, QString &dbName, QString &user, QString &pass, int &port )
{
	serverSetupPage->getServerInfo( isRemote, host, client, dbName, user, pass, port );
	if ( dbTypeSetupPage->dbType() == SQLite )
		dbName = sqliteSetupPage->dbFile();
}

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

#include "setupassistant.moc"
