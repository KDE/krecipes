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

#include "postgresqlserverprefs.h"

#include <KLocale>
#include <KFileDialog>
#include <KConfigGroup>
#include <KNumInput>
#include <KLineEdit>
#include <KUrlRequester>

#include <QVBoxLayout>
#include <QFormLayout>

PostgreSQLServerPrefs::PostgreSQLServerPrefs( QWidget *parent ) : QWidget( parent )
{
	QVBoxLayout * layout = new QVBoxLayout( this );

	QFormLayout * dbLayout = new QFormLayout;

	serverEdit = new KLineEdit( this );
	dbLayout->addRow( i18n( "Server:" ), serverEdit );

	usernameEdit = new KLineEdit( this );
	dbLayout->addRow ( i18nc("@label:textbox Database Username", "Username:" ), usernameEdit );

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
	QFormLayout *backupGBoxLayout = new QFormLayout;
	backupGBox->setTitle( i18nc( "@title:group Backup options", "Backup" ) );

	dumpPathRequester = new KUrlRequester;
	dumpPathRequester->fileDialog()->setCaption( i18n( "Select PostgreSQL dump command" ) );
	backupGBoxLayout->addRow( i18n( "Path to '%1':" ,QString("pg_dump") ), dumpPathRequester );

	psqlPathRequester = new KUrlRequester;
	dumpPathRequester->fileDialog()->setCaption( i18n( "Select PostgreSQL command" ) );
	backupGBoxLayout->addRow( i18n( "Path to '%1':" ,QString("psql") ), psqlPathRequester );

	backupGBox->setLayout( backupGBoxLayout );
	
	layout->addWidget( backupGBox );

	// Load & Save Current Settings
	KConfigGroup config = KGlobal::config()->group( "Server" );
	serverEdit->setText( config.readEntry( "Host", "localhost" ) );
	usernameEdit->setText( config.readEntry( "Username", "" ) );
	passwordEdit->setText( config.readEntry( "Password", "" ) );
	portEdit->setValue( config.readEntry( "Port", 0 ) );
	dbNameEdit->setText( config.readEntry( "DBName", "Krecipes" ) );
	dumpPathRequester->setUrl( config.readEntry( "PgDumpPath", "pg_dump" ) );
	dumpPathRequester->setFilter( "pg_dump\n*|" + i18n( "All files" ) );
	psqlPathRequester->setUrl( config.readEntry( "PsqlPath", "psql" ) );
	psqlPathRequester->setFilter( "psql\n*|" + i18n( "All files" ) );
}

void PostgreSQLServerPrefs::saveOptions( void )
{
	KConfigGroup config = KGlobal::config()->group( "Server" );
	config.writeEntry( "Host", serverEdit->text() );
	config.writeEntry( "Username", usernameEdit->text() );
	config.writeEntry( "Password", passwordEdit->text() );
	config.writeEntry( "Port", portEdit->value() );
	config.writeEntry( "DBName", dbNameEdit->text() );
	config.writeEntry( "PgDumpPath", dumpPathRequester->text() );
	config.writeEntry( "PsqlPath", psqlPathRequester->text() );
}

