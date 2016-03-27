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

#include "sqliteserverprefs.h"

#include <KHBox>
#include <KUrlRequester>
#include <KConfigGroup>
#include <KLocale>
#include <KFileDialog>
#include <KStandardDirs>

#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>


SQLiteServerPrefs::SQLiteServerPrefs( QWidget *parent ) : QWidget( parent )
{
	QVBoxLayout * Form1Layout = new QVBoxLayout( this );

	KHBox *hbox = new KHBox( this );
	( void ) new QLabel( i18n( "Database file:" ), hbox );

	Form1Layout->addWidget( hbox );

	QSpacerItem* spacerRow5 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
	Form1Layout->addItem( spacerRow5 );

	QString sqliteBinary = "sqlite3";

	// Backup options
	QGroupBox *backupGBox = new QGroupBox( this );
	backupGBox->setTitle( i18nc( "@title:group Backup options", "Backup" ) );
	
	QFormLayout *backupGBoxLayout = new QFormLayout;
	backupGBox->setLayout( backupGBoxLayout );

	dumpPathRequester = new KUrlRequester;
	dumpPathRequester->fileDialog()->setCaption( i18n( "Select SQLite command" ) );
	backupGBoxLayout->addRow( i18n( "Path to '%1':", sqliteBinary ), dumpPathRequester );
	
	Form1Layout->addWidget( backupGBox );

	// SQLite converter options
	QGroupBox *converterGBox = new QGroupBox( this );
	converterGBox->setTitle( i18n( "SQLite converter" ) );

	QFormLayout *converterGBoxLayout = new QFormLayout;
	converterGBox->setLayout( converterGBoxLayout );

	oldPathRequester = new KUrlRequester;
	oldPathRequester->fileDialog()->setCaption( i18n( "Select old SQLite version command" ) );
	converterGBoxLayout->addRow( i18n( "Path to SQLite old version command:" ), oldPathRequester );
	
	newPathRequester = new KUrlRequester;
	newPathRequester->fileDialog()->setCaption( i18n( "Select new SQLite version command" ) );
	converterGBoxLayout->addRow( i18n( "Path to SQLite new version command:" ), newPathRequester );
	
	Form1Layout->addWidget( converterGBox );


	// Load Current Settings
	KConfigGroup config = KGlobal::config()->group( "Server" );
	fileRequester = new KUrlRequester( config.readEntry( "DBFile", KStandardDirs::locateLocal( "appdata", "krecipes.krecdb" ) ), hbox );
	fileRequester->fileDialog()->setCaption( i18n( "Select SQLite database file" ) );
	hbox->setStretchFactor( fileRequester, 2 );
	dumpPathRequester->setUrl( config.readEntry( "SQLitePath", sqliteBinary ) );
	dumpPathRequester->setFilter( sqliteBinary + "\n*|" + i18n( "All files" ) );
	oldPathRequester->setUrl( config.readEntry( "SQLiteOldVersionPath", "sqlite" ) );
	oldPathRequester->setFilter( "sqlite\n*|" + i18n( "All files" ) );
	newPathRequester->setUrl( config.readEntry( "SQLiteNewVersionPath", "sqlite3" ) );
	newPathRequester->setFilter( "sqlite3\n*|" + i18n( "All files" ) );
}

void SQLiteServerPrefs::saveOptions( void )
{
	KConfigGroup config = KGlobal::config()->group( "Server" );
	config.writeEntry( "DBFile", fileRequester->text() );
	config.writeEntry( "SQLitePath", dumpPathRequester->text() );
	config.writeEntry( "SQLiteOldVersionPath", oldPathRequester->text() );
	config.writeEntry( "SQLiteNewVersionPath", newPathRequester->text() );
}

