
/***************************************************************************
*   Copyright (C) 2003-2005 by                                            *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "krecipes.h"

#include <iostream>

#include <kuniqueapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

#include "convert_sqlite3.h"

static const char *description =
    I18N_NOOP( "The KDE Cookbook" );

static const char *version = "1.0";

static KCmdLineOptions options;

int main( int argc, char **argv )
{
	options.add("(const QByteArray&)convert-sqlite3", (const KLocalizedString&)I18N_NOOP("Convert the current SQLite 2.x database to SQLite 3 and exit") , (const QByteArray&)0 );
	options.add( (const QByteArray&)0, (const KLocalizedString&)"", (const QByteArray&)0 );    


	KAboutData about( (const QByteArray&)"krecipes", (const QByteArray&)0, (const KLocalizedString&)I18N_NOOP( "Krecipes" ), (const QByteArray&)version, (const KLocalizedString&)description, KAboutData::License_GPL, (const KLocalizedString&)I18N_NOOP( "(C) 2003 Unai Garro\n(C) 2004-2006 Jason Kivlighn\n\n___________\n\n\nThis product is RecipeML compatible.\n You can get more information about this file format in:\n http://www.formatdata.com/recipeml" ), (const KLocalizedString&)"", (const QByteArray&)"http://krecipes.sourceforge.net/", (const QByteArray&)"jkivlighn@gmail.com" );
	about.addAuthor( (const KLocalizedString&)"Unai Garro", (const KLocalizedString&)"", (const QByteArray&)"ugarro@users.sourceforge.net", (const QByteArray&)0 );
	about.addAuthor( (const KLocalizedString&)"Jason Kivlighn", (const KLocalizedString&)"", (const QByteArray&)"jkivlighn@gmail.com", (const QByteArray&)0 );
	about.addAuthor( (const KLocalizedString&)"Cyril Bosselut", (const KLocalizedString&)"", (const QByteArray&)"bosselut@b1project.com", (const QByteArray&)0 );

	about.addCredit( (const KLocalizedString&)"Colleen Beamer", (const KLocalizedString&)I18N_NOOP("Testing, bug reports, suggestions"), (const QByteArray&)"colleen.beamer@gmail.com", (const QByteArray&)0 );
	about.addCredit( (const KLocalizedString&)"Robert Wadley", (const KLocalizedString&)I18N_NOOP("Icons and artwork"), (const QByteArray&)"rob@robntina.fastmail.us", (const QByteArray&)0 );
	about.addCredit( (const KLocalizedString&)"Daniel Sauvé", (const KLocalizedString&)I18N_NOOP("Porting to KDE4"), (const QByteArray&)"megametres@gmail.com", (const QByteArray&)"http://metres.homelinux.com" );

	about.setTranslator( (const KLocalizedString&)I18N_NOOP( "INSERT YOUR NAME HERE" ), (const KLocalizedString&)I18N_NOOP( "INSERT YOUR EMAIL ADDRESS" ) );
	KCmdLineArgs::init( argc, argv, &about );
	KCmdLineArgs::addCmdLineOptions( options );
	KUniqueApplication::addCmdLineOptions();

	if ( !KUniqueApplication::start() ) {
		std::cout << "Krecipes is already running!" << std::endl;
		return 0;
	}

	KUniqueApplication app;

	// see if we are starting with session management
	if ( app.isSessionRestored() ) {
		RESTORE( Krecipes );
	}
	else {
		// no session.. just start up normally
		KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

		QApplication::flushX();

		if ( args->isSet("convert-sqlite3") ) {
			ConvertSQLite3();
			return 0;
		}

		Krecipes * widget = new Krecipes;
		app.setMainWidget( widget );
		widget->show();

		args->clear();
	}

	return app.exec();
}

