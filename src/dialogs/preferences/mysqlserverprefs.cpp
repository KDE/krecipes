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

#include "mysqlserverprefs.h"

#include <KLocale>
#include <KFileDialog>
#include <KConfigGroup>
#include <KNumInput>
#include <KLineEdit>
#include <KUrlRequester>

#include <QVBoxLayout>
#include <QFormLayout>

MySQLServerPrefs::MySQLServerPrefs( QWidget *parent ) : QWidget( parent )
{
	QVBoxLayout * layout = new QVBoxLayout( this );
	
	QFormLayout * dbLayout = new QFormLayout;

	serverEdit = new KLineEdit( this );
	dbLayout->addRow( i18n( "Server:" ),  serverEdit );

	usernameEdit = new KLineEdit( this );
	dbLayout->addRow( i18nc( "@label:textbox Database Username", "Username:" ), usernameEdit );

	passwordEdit = new KLineEdit( this );
	passwordEdit->setEchoMode( KLineEdit::Password );
	dbLayout->addRow( i18n( "Password:" ), passwordEdit );

	portEdit = new KIntNumInput( this );
	portEdit->setMinimum(0);
	portEdit->setSpecialValueText( i18nc("Default Port", "Default") );
	dbLayout->addRow( i18n( "Port:" ), portEdit );

	dbNameEdit = new KLineEdit( this );
	dbLayout->addRow( i18n( "Database name:" ), dbNameEdit );

	layout->addLayout( dbLayout );

	// Backup options
	QGroupBox *backupGBox = new QGroupBox( this );
	backupGBox->setTitle( i18nc( "@title:group Backup options", "Backup" ) );
	QFormLayout *backupGBoxLayout = new QFormLayout;

	dumpPathRequester = new KUrlRequester;
	dumpPathRequester->fileDialog()->setCaption( i18n( "Select MySQL dump command" ) );
	backupGBoxLayout->addRow( i18n( "Path to '%1':" ,QString("mysqldump") ), dumpPathRequester );

	mysqlPathRequester = new KUrlRequester;
	mysqlPathRequester->fileDialog()->setCaption( i18n( "Select MySQL command" ) );
	backupGBoxLayout->addRow( i18n( "Path to '%1':" ,QString("mysql") ),mysqlPathRequester );

	backupGBox->setLayout(backupGBoxLayout);

	layout->addWidget( backupGBox );

	// Load Current Settings
	KConfigGroup config = KGlobal::config()->group( "Server" );
	serverEdit->setText( config.readEntry( "Host", "localhost" ) );
	usernameEdit->setText( config.readEntry( "Username", "" ) );
	passwordEdit->setText( config.readEntry( "Password", "" ) );
	portEdit->setValue( config.readEntry( "Port", 0 ) );
	dbNameEdit->setText( config.readEntry( "DBName", "Krecipes" ) );
	dumpPathRequester->setUrl( config.readEntry( "MySQLDumpPath", "mysqldump" ) );
	dumpPathRequester->setFilter( "mysqldump\n*|" + i18n( "All files" ) );
	mysqlPathRequester->setUrl( config.readEntry( "MySQLPath", "mysql" ) );
	mysqlPathRequester->setFilter( "mysql\n*|" + i18n( "All files" ) );
}

void MySQLServerPrefs::saveOptions( void )
{
	// Save Current Settings
	KConfigGroup config = KGlobal::config()->group( "Server" );
	config.writeEntry( "Host", serverEdit->text() );
	config.writeEntry( "Username", usernameEdit->text() );
	config.writeEntry( "Password", passwordEdit->text() );
	config.writeEntry( "Port", portEdit->value() );
	config.writeEntry( "DBName", dbNameEdit->text() );
	config.writeEntry( "MySQLDumpPath", dumpPathRequester->text() );
	config.writeEntry( "MySQLPath", mysqlPathRequester->text() );
}

