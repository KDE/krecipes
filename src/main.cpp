
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

static const char *version = "1.1-svn";

static KCmdLineOptions options[] =
    {
        { "convert-sqlite3", I18N_NOOP("Convert the current SQLite 2.x database to SQLite 3 and exit") , 0 },
        { 0, 0, 0 }
    };

int main( int argc, char **argv )
{
	KAboutData about( "krecipes", I18N_NOOP( "Krecipes" ), version, description,
	                  KAboutData::License_GPL, I18N_NOOP( "(C) 2003 Unai Garro\n(C) 2004-2008 Jason Kivlighn\n\n___________\n\n\nThis product is RecipeML compatible.\n You can get more information about this file format in:\n http://www.formatdata.com/recipeml" ), 0, 0, "jkivlighn@gmail.com" );
	about.addAuthor( "Unai Garro", 0, "ugarro@users.sourceforge.net" );
	about.addAuthor( "Jason Kivlighn", 0, "jkivlighn@gmail.com" );
	about.addAuthor( "Cyril Bosselut", 0, "bosselut@b1project.com" );

	about.addCredit( "Colleen Beamer", I18N_NOOP("Testing, bug reports, suggestions"), "colleen.beamer@gmail.com" );
	about.addCredit( "Robert Wadley", I18N_NOOP("Icons and artwork"), "rob@robntina.fastmail.us" );

	about.setTranslator( I18N_NOOP( "INSERT YOUR NAME HERE" ), I18N_NOOP( "INSERT YOUR EMAIL ADDRESS" ) );
	KCmdLineArgs::init( argc, argv, &about );
	KCmdLineArgs::addCmdLineOptions( options );
	KUniqueApplication::addCmdLineOptions();

	if ( !KUniqueApplication::start() ) {
		std::cout << "Krecipes is already running!" << std::endl;
		return 0;
	}

	KUniqueApplication app;

	// see if we are starting with session management
	if ( app.isRestored() ) {
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

