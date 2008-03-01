/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "dbimportdialog.h"

#include <unistd.h> //for getuid()
#include <pwd.h> //getpwuid()

#include "config-krecipes.h"

#include <qpushbutton.h>
#include <q3buttongroup.h>
#include <qradiobutton.h>
#include <QStackedWidget>
#include <qwidget.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <q3whatsthis.h>

#include <QGridLayout>
#include <QVBoxLayout>

#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kurlrequester.h>
#include <knuminput.h>
#include <kvbox.h>

DBImportDialog::DBImportDialog( QWidget *parent, const char *name )
		: KDialog( parent )
{
	this->setObjectName( name );
	this->setModal( true );
	this->setButtons( KDialog::Ok | KDialog::Cancel );
	this->setDefaultButton( KDialog::Ok  );
	this->setCaption( i18n( "Database Import" ) );

	setButtonsOrientation( Qt::Vertical );

	KHBox *page = new KHBox( this );
	setMainWidget( page );

	dbButtonGroup = new Q3ButtonGroup( page, "dbButtonGroup" );
	dbButtonGroup->setSizePolicy( QSizePolicy( ( QSizePolicy::SizeType ) 4, ( QSizePolicy::SizeType ) 5, 0, 0, dbButtonGroup->sizePolicy().hasHeightForWidth() ) );
	dbButtonGroup->setColumnLayout( 0, Qt::Vertical );
	dbButtonGroup->layout() ->setSpacing( 6 );
	dbButtonGroup->layout() ->setMargin( 11 );
	dbButtonGroupLayout = new QVBoxLayout( dbButtonGroup->layout() );
	dbButtonGroupLayout->setAlignment( Qt::AlignTop );

	liteRadioButton = new QRadioButton( dbButtonGroup );
   liteRadioButton->setObjectName( "liteRadioButton" );
	liteRadioButton->setChecked( TRUE );
	dbButtonGroupLayout->addWidget( liteRadioButton );

	mysqlRadioButton = new QRadioButton( dbButtonGroup );
   mysqlRadioButton->setObjectName( "mysqlRadioButton" );
	dbButtonGroupLayout->addWidget( mysqlRadioButton );

	psqlRadioButton = new QRadioButton( dbButtonGroup );
   psqlRadioButton->setObjectName( "psqlRadioButton" );
	dbButtonGroupLayout->addWidget( psqlRadioButton );

	paramStack = new QStackedWidget( page );
	paramStack->setSizePolicy( QSizePolicy( ( QSizePolicy::SizeType ) 7, ( QSizePolicy::SizeType ) 5, 0, 0, paramStack->sizePolicy().hasHeightForWidth() ) );

	sqlitePage = new QWidget( paramStack );
   sqlitePage->setObjectName( "sqlitePage" );
	serverPageLayout_2 = new QVBoxLayout( sqlitePage );
   serverPageLayout_2->setMargin( 11 );
   serverPageLayout_2->setSpacing( 6 );
   serverPageLayout_2->setObjectName( "serverPageLayout_2" );

	QLabel *sqliteLabel = new QLabel( i18n( "Database file:" ), sqlitePage );
	serverPageLayout_2->addWidget( sqliteLabel );
	sqliteDBRequester = new KUrlRequester( sqlitePage );
	sqliteDBRequester->setObjectName( "sqliteDBRequester" );
	serverPageLayout_2->addWidget( sqliteDBRequester );

	QSpacerItem *vSpacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
	serverPageLayout_2->addItem(vSpacer);

	paramStack->insertWidget( 1, sqlitePage );

	serverPage = new QWidget( paramStack );
   serverPage->setObjectName( "serverPage" );
	serverPageLayout = new QVBoxLayout( serverPage );
   serverPageLayout->setMargin( 11 );
   serverPageLayout->setSpacing( 6 );
   serverPageLayout->setObjectName( "serverPageLayout" );

	layout5 = new QGridLayout();
   layout5->cellRect( 1, 1 );
   layout5->setMargin( 0 );
   layout5->setSpacing( 6 );
   layout5->setObjectName( "layout5" );

	hostEdit = new QLineEdit( serverPage );
   hostEdit->setObjectName( "hostEdit" );
	layout5->addWidget( hostEdit, 0, 1 );
	hostLabel = new QLabel( serverPage );
   hostLabel->setObjectName( "hostLabel" );
	layout5->addWidget( hostLabel, 0, 0 );

	userEdit = new QLineEdit( serverPage );
   userEdit->setObjectName( "userEdit" );
	layout5->addWidget( userEdit, 1, 1 );
	userLabel = new QLabel( serverPage );
   userLabel->setObjectName( "userLabel" );
	layout5->addWidget( userLabel, 1, 0 );

	passwordEdit = new QLineEdit( serverPage );
   passwordEdit->setObjectName( "passwordEdit" );
	passwordEdit->setEchoMode( QLineEdit::Password );
	layout5->addWidget( passwordEdit, 2, 1 );
	passwordLabel = new QLabel( serverPage );
   passwordLabel->setObjectName( "passwordLabel" );
	layout5->addWidget( passwordLabel, 2, 0 );

	portEdit = new KIntNumInput( serverPage );
	portEdit->setObjectName( "portEdit" );
	portEdit->setMinimum(0);
	portEdit->setValue(0);
	layout5->addWidget( portEdit, 3, 1 );
	portLabel = new QLabel( serverPage );
   portLabel->setObjectName( "portLabel" );
	layout5->addWidget( portLabel, 3, 0 );

	nameEdit = new QLineEdit( serverPage );
   nameEdit->setObjectName( "nameEdit" );
	layout5->addWidget( nameEdit, 4, 1 );
	nameLabel = new QLabel( serverPage );
   nameLabel->setObjectName( "nameLabel" );
	layout5->addWidget( nameLabel, 4, 0 );

	serverPageLayout->addLayout( layout5 );
	paramStack->insertWidget(0, serverPage );

	languageChange();


#if (!HAVE_MYSQL)

	mysqlRadioButton->setEnabled( false );
#endif

#if (!HAVE_POSTGRESQL)

	psqlRadioButton->setEnabled( false );
#endif

#if (!(HAVE_SQLITE || HAVE_SQLITE3))

	liteRadioButton->setEnabled( false );
#if (HAVE_MYSQL)

	dbButtonGroup->setButton( 1 ); // Otherwise by default liteCheckBox is checked even if it's disabled
	switchDBPage(1);
#else
	#if (HAVE_POSTGRESQL)

	dbButtonGroup->setButton( 2 );
	switchDBPage(2);
#endif
	#endif
	#endif

	// signals and slots connections
	connect( dbButtonGroup, SIGNAL( clicked( int ) ), this, SLOT( switchDBPage( int ) ) );
}

void DBImportDialog::languageChange()
{
	dbButtonGroup->setTitle( i18n( "Database" ) );
	liteRadioButton->setText( "SQLite" );
	mysqlRadioButton->setText( "MySQL" );
	psqlRadioButton->setText( "PostgreSQL" );
	hostLabel->setText( i18n( "Server:" ) );
	userLabel->setText( i18n( "Username:" ) );
	passwordLabel->setText( i18n( "Password:" ) );
	nameLabel->setText( i18n( "Database name:" ) );
	portLabel->setText( i18n( "Port:" ) );
	portEdit->setSpecialValueText( i18n("Default") );

	//set defaults
	hostEdit->setText( "localhost" );
	nameEdit->setText( "Krecipes" );

	// get username
	uid_t userID;
	struct passwd *user;
	userID = getuid();
	user = getpwuid ( userID );
	QString username(user->pw_name);

	userEdit->setText( username );
}

void DBImportDialog::switchDBPage( int id )
{
	switch ( id ) {
	case 0:  //SQLite
		paramStack->setCurrentWidget( sqlitePage );
		break;
	case 1:  //MySQL
	case 2:  //PostgreSQL
		paramStack->setCurrentWidget( serverPage );
		break;
	}
}

QString DBImportDialog::dbType() const
{
	//int id=dbButtonGroup->selectedId(); //QT 3.2
	int id = dbButtonGroup->id( dbButtonGroup->selected() ); //QT 3.1
	switch ( id ) {
	case 0:
		return "SQLite";
	case 1:
		return "MySQL";
	case 2:
		return "PostgreSQL";
	default:
		return QString::null;
	}
}

void DBImportDialog::serverParams( QString &host, QString &user, QString &pass, int &port, QString &table ) const
{
	host = hostEdit->text();
	user = userEdit->text();
	pass = passwordEdit->text();
	table = nameEdit->text();
	port = portEdit->value();
}

QString DBImportDialog::dbFile() const
{
	return sqliteDBRequester->url().path();
}

#include "dbimportdialog.moc"
