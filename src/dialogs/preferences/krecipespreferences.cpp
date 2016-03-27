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

#include "krecipespreferences.h"

#include "sqliteserverprefs.h"
#include "mysqlserverprefs.h"
#include "postgresqlserverprefs.h"
#include "serverprefs.h"
#include "numberprefs.h"
#include "importprefs.h"
#include "performanceprefs.h"
#include "spellcheckingprefs.h"

#include <KConfigGroup>
#include <KIconLoader>
#include <KLocale>
#include <KToolInvocation>


KrecipesPreferences::KrecipesPreferences( QWidget *parent )
		: KPageDialog( parent )
{
	setFaceType( List );
	setObjectName( "KrecipesPreferences" );
	setCaption( i18n( "Configure" ) );
	setButtons( Help | Ok | Cancel );
	setDefaultButton( Ok );

	KConfigGroup config = KGlobal::config()->group( "DBType" );

	KIconLoader *il = KIconLoader::global();

        m_pageServer = new ServerPrefs( this );
        KPageWidgetItem * page = new KPageWidgetItem( m_pageServer , i18n( "Database System" ) );
        page->setObjectName("server" );
	page->setHeader( i18n( "Database Management System Options (%1)" , config.readEntry( "Type" )));
	il->loadIcon( "network-server", KIconLoader::NoGroup, 32 );
	page->setIcon( KIcon( "server-database", il ) );
        m_helpMap.insert(page->objectName(),"configure-server-settings");
        addPage( page );

        m_pageNumbers = new NumbersPrefs( this );
	page = new KPageWidgetItem(m_pageNumbers , i18n( "Formatting" ) );
        page->setObjectName( "formating" );
	page->setHeader( i18n( "Customize Formatting" ) );
	il->loadIcon( "format-indent-more", KIconLoader::NoGroup, 32 );
	page->setIcon( KIcon( "format-indent-more", il ) );
	addPage(page);
	m_helpMap.insert(page->objectName(),"custom-formatting");

	m_pageImport = new ImportPrefs( this );
        page = new KPageWidgetItem( m_pageImport , i18n( "Import/Export" ) );
        page->setObjectName( "import" );
	page->setHeader( i18n( "Recipe Import and Export Options" ) );
	il->loadIcon( "go-down", KIconLoader::NoGroup, 32 );
	page->setIcon( KIcon( "go-down", il ) );
	addPage(page);
	m_helpMap.insert(page->objectName(),"import-export-preference");


	m_pagePerformance = new PerformancePrefs( this );
        page = new KPageWidgetItem( m_pagePerformance , i18n( "Performance" ) );
	page->setHeader( i18n( "Performance Options" ) );
        page->setObjectName( "performance" );
	il->loadIcon( "preferences-system-performance", KIconLoader::NoGroup, 32 );
	page->setIcon( KIcon( "preferences-system-performance", il ) );
	addPage(page);
	m_helpMap.insert(page->objectName(),"configure-performance");


	m_pageSpellChecking = new SpellCheckingPrefs( this );
        page = new KPageWidgetItem( m_pageSpellChecking , i18n( "Spell checking" ) );
	page->setHeader( i18n( "Spell checking Options" ) );
        page->setObjectName( "spellchecking" );
	il->loadIcon( "tools-check-spelling", KIconLoader::NoGroup, 32 );
	page->setIcon( KIcon( "tools-check-spelling", il ) );
	addPage(page);
	m_helpMap.insert(page->objectName() ,"configure-spell");


	// Signals & Slots
	connect ( this, SIGNAL( okClicked() ), this, SLOT( saveSettings() ) );
        connect ( this, SIGNAL( helpClicked() ), this, SLOT( slotHelp() ) );
}

void KrecipesPreferences::saveSettings( void )
{
	m_pageServer->saveOptions();
	m_pageNumbers->saveOptions();
	m_pageImport->saveOptions();
	m_pagePerformance->saveOptions();
        m_pageSpellChecking->saveOptions();
}


void KrecipesPreferences::slotHelp()
{
    KToolInvocation::invokeHelp( m_helpMap.value( currentPage()->name() ) );
}

