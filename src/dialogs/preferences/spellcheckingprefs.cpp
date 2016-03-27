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

#include "spellcheckingprefs.h"

#include <sonnet/configwidget.h>
#include <KConfigGroup>
#include <KConfig>
#include <KGlobal>
#include <KStandardDirs>
#include <KCmdLineArgs>
#include <KAboutData>

#include <QHBoxLayout>


SpellCheckingPrefs::SpellCheckingPrefs( QWidget *parent )
	: QWidget( parent )
{
	QHBoxLayout *lay = new QHBoxLayout( this );

	QString spellCheckingConfigFileName = KStandardDirs::locateLocal( "config",
		KCmdLineArgs::aboutData()->appName() + "rc" );

	KConfig localConfig( spellCheckingConfigFileName, KConfig::SimpleConfig );

	//If we don't have our local configuration for spell checking, fall back to
	//user's global configuration.
	if ( !localConfig.hasGroup( "Spelling" ) ) {
		KConfigGroup localGroup( &localConfig, "Spelling" );
		KConfig globalSonnetConfig( KStandardDirs::locateLocal( "config", "sonnetrc" ) );
		KConfigGroup globalGroup( &globalSonnetConfig, "Spelling" );
		globalGroup.copyTo( &localGroup );
		localConfig.sync();
		KConfigGroup group( KGlobal::config(), "Spelling" );
		globalGroup.copyTo( &group );
	}

	m_confPage = new Sonnet::ConfigWidget(&( *KGlobal::config() ), this );
	lay->addWidget( m_confPage );
	setLayout( lay );
}

void SpellCheckingPrefs::saveOptions()
{
	m_confPage->save();
}

