/*****************************************************************************
*   Copyright © 2012 José Manuel Santamaría Lema <panfaust@gmail.com>        *
*                                                                            *
*   This program is free software; you can redistribute it and/or modify     *
*   it under the terms of the GNU General Public License as published by     *
*   the Free Software Foundation; either version 2 of the License, or        *
*   (at your option) any later version.                                      *
******************************************************************************/

#include "nukeconfigwidget.h"

#include "ui_nukeconfigwidget.h"

#include <KGlobal>
#include <QStringList>
#include <KStandardDirs>
#include <KConfigGroup>
#include <KMessageBox>

#include <kdebug.h>

#include "backends/recipedb.h"


NukeConfigWidget::NukeConfigWidget( QWidget * parent ) : 
	QWidget( parent ), 
	ui(new Ui::NukeConfigWidget)
{
	//Setup UI
	ui->setupUi( this );

	//Conect signals/slots.
	connect( ui->m_proceedButton, SIGNAL(clicked()), this, SLOT(proceed()) );
}

void NukeConfigWidget::proceed()
{
	QString krecipesConfigFileName = 
		KStandardDirs::locateLocal( "config", "krecipesrc" );
	
	QStringList strList;
	strList << krecipesConfigFileName;
	KGlobal::config()->addConfigSources( strList );

	KConfigGroup dbtypeGroup = KGlobal::config()->group( "DBType" );
	QString dbType = dbtypeGroup.readEntry( "Type", "" );

        KConfigGroup performanceGroup = KGlobal::config()->group( "Server" );
        QString host = performanceGroup.readEntry( "Host", "localhost" );
        QString user = performanceGroup.readEntry( "Username", QString() );
        QString pass = performanceGroup.readEntry( "Password", QString() );
        QString dbname = performanceGroup.readEntry( "DBName", "Krecipes" );
        int port = performanceGroup.readEntry( "Port", 0 );
        QString dbfile = performanceGroup.readEntry( "DBFile",
		KStandardDirs::locateLocal ( "appdata", "krecipes.krecdb" ) );

	database = RecipeDB::createDatabase( dbType, host, user, pass, dbname, port, dbfile );

	RecipeDB::Error connectError = database->connect();

	if ( !database->ok() ) {
		KMessageBox::error( this, "Error opening the Krecipes database.");
	}

	int authors_number = ui->m_authorsNumberInput->value();

	ui->m_progressBar->setValue( 0 );
	ui->m_progressBar->setMaximum( authors_number - 1 );
	database->transaction();
	int i;
	for (i = 0; i < authors_number; i++ ) {
		database->createNewAuthor( QString( "Author %1" ).arg( i ) );
		ui->m_progressBar->setValue( i );
	}
	database->commit();

}


#include "nukeconfigwidget.moc"
