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

#include "serverprefs.h"

#include "sqliteserverprefs.h"
#include "mysqlserverprefs.h"
#include "postgresqlserverprefs.h"

#include <KConfigGroup>
#include <KGlobal>
#include <KLocale>

#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>


ServerPrefs::ServerPrefs( QWidget *parent )
		: QWidget( parent )
{
	QVBoxLayout * Form1Layout = new QVBoxLayout( this );
	Form1Layout->setMargin( 11 );
	Form1Layout->setSpacing( 6 );

	KConfigGroup config = KGlobal::config()->group( "DBType" );
	QString DBtype = config.readEntry( "Type" );
	if ( DBtype == "MySQL" )
		serverWidget = new MySQLServerPrefs( this );
	else if ( DBtype == "PostgreSQL" )
		serverWidget = new PostgreSQLServerPrefs( this );
	else
		serverWidget = new SQLiteServerPrefs( this );

	serverWidget->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	serverWidget->setMinimumSize( serverWidget->sizeHint() );
	Form1Layout->addWidget( serverWidget );

	Form1Layout->addItem( new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

	wizard_button = new QCheckBox( i18n( "Re-run wizard on next startup" ), this );
	wizard_button->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	config = KGlobal::config()->group( "Wizard" );
	wizard_button->setChecked( !config.readEntry( "SystemSetup", false ) );
	Form1Layout->addWidget( wizard_button );

	QLabel *note = new QLabel( i18n( "Note: Krecipes must be restarted for most server preferences to take effect." ), this );
	note->setWordWrap( true );
	Form1Layout->addWidget( note );

	adjustSize();
}

// Save Server settings
void ServerPrefs::saveOptions( void )
{
	KConfigGroup config = KGlobal::config()->group( "DBType" );
	QString DBtype = config.readEntry( "Type" );
	if ( DBtype == "MySQL" )
		( ( MySQLServerPrefs* ) serverWidget ) ->saveOptions();
	else if ( DBtype == "PostgreSQL" )
		( ( PostgreSQLServerPrefs* ) serverWidget ) ->saveOptions();
	else
		( ( SQLiteServerPrefs* ) serverWidget ) ->saveOptions();

	config = KGlobal::config()->group( "Wizard" );
	config.writeEntry( "SystemSetup", !(wizard_button->isChecked()) );
}

