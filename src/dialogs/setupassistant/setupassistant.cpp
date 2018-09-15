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
#include "setupassistant.h"

#include "welcomepage.h"
#include "dbtypesetuppage.h"
#include "sqlitesetuppage.h"
#include "permissionssetuppage.h"
#include "psqlpermissionssetuppage.h"
#include "serversetuppage.h"
#include "datainitializepage.h"
#include "savepage.h"

#include "backends/usda_ingredient_data.h"
#include "widgets/clickablelabel.h"

#include <KMessageBox>
#include <KConfigGroup>
#include <KLocale>
#include <KGlobal>

#include <kdebug.h>


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

	connect( dbTypeSetupPage, SIGNAL(showPages(DBType)), this, SLOT(showPages(DBType)) );
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

#include "setupassistant.moc"
