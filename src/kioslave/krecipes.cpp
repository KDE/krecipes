/***************************************************************************
*   Copyright (C)  by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include <kinstance.h>
#include <kdebug.h>
#include <kurl.h>
#include <klocale.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kconfig.h>

#include <kio/global.h>

#include <qfile.h>
#include <qfileinfo.h>
#include <qstring.h>
#include <qdir.h>
#include <qregexp.h>

#include <qsqldatabase.h>
#include <qsqlquery.h>

extern "C"
{
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
}

#include "krecipes.h"

#include "recipedb.h"
#include "datablocks/categorytree.h"


using namespace KIO;

kio_krecipesProtocol::kio_krecipesProtocol(const QCString &pool_socket,
                                                 const QCString &app_socket)
    : SlaveBase("kio_krecipes", pool_socket, app_socket)
{
	KConfig config( locate("config", "krecipesrc" ) );
	config.setGroup( "Server" );
	QString host = config.readEntry( "Host", "localhost" );
	QString user = config.readEntry( "Username", QString::null );
	QString pass = config.readEntry( "Password", QString::null );
	QString dbname = config.readEntry( "DBName", "Krecipes" );
	QString dbFile = config.readEntry( "DBFile", QString::null );

	config.setGroup( "DBType" );
	QString dbType = config.readEntry( "Type", "SQLite" );

	m_db = RecipeDB::createDatabase( dbType, host, user, pass, dbname, dbFile );
	if ( m_db )
		m_db->connect(false,false);
}

kio_krecipesProtocol::~kio_krecipesProtocol()
{
	delete m_db;
}

void kio_krecipesProtocol::listDir( const KURL & url )
{
	kdDebug() << "========= KRECIPES LIST " << url.url() << " =========" << endl;

	Q_ASSERT( m_db && m_db->ok() );

	UDSEntry entry;

	int catID;
	if ( url.path() == "/" )
		catID = -1;
	else {
		QStringList parts = QStringList::split("/",url.path());
		kdDebug()<<"in category: "<<parts[parts.count()-1]<<endl;
		catID = m_db->findExistingCategoryByName( parts[parts.count()-1] );
		if ( catID == -1 ) {
			kdDebug()<<"Category does not exist: "<<parts[parts.count()-1]<<endl;
			finished();
			return;
		}
	}

	CategoryTree categoryTree;
	m_db->loadCategories( &categoryTree, -1, 0, catID, false );

	for ( CategoryTree * child_it = categoryTree.firstChild(); child_it; child_it = child_it->nextSibling() ) {
		entry.clear();
		createCategoryEntry(entry, child_it->category );
		listEntry( entry, false );
	}

	if ( catID != -1 ) {
		ElementList recipeList;
		m_db->loadRecipeList( &recipeList, catID, false );
		for ( ElementList::const_iterator it = recipeList.begin(); it != recipeList.end(); ++it ) {
			entry.clear();
	
			Recipe r;
			r.title = (*it).name;
			r.recipeID = (*it).id;
	
			createRecipeEntry(entry, r );
			listEntry( entry, false );
		}
	}

	listEntry( entry, true ); // ready
	
	kdDebug() << "============= COMPLETED LIST ============" << endl;
	
	finished();
	
	kdDebug() << "kio_krecipesProtocol::listDir done" << endl;
}

void kio_krecipesProtocol::stat( const KURL & url )
{
	kdDebug() << "========= KRECIPES STAT " << url.url() << " =========" << endl;

	UDSEntry entry;

	UDSAtom atom;
	atom.m_uds = KIO::UDS_NAME;
	atom.m_str = "test";
	entry.append( atom );

	atom.m_uds = KIO::UDS_FILE_TYPE;
	atom.m_long = S_IFREG;
	entry.append( atom );

	atom.m_uds = KIO::UDS_SIZE;
	atom.m_long = 0L;
	entry.append( atom );

	statEntry( entry ); // ready
	finished();
}

void kio_krecipesProtocol::get( const KURL & url )
{
	kdDebug() << "========= KRECIPES GET " << url.url() << " =========" << endl;

	SlaveBase::get(url);
}

void kio_krecipesProtocol::createRecipeEntry( UDSEntry &entry, const Recipe &recipe )
{
	UDSAtom atom;
	atom.m_uds = KIO::UDS_NAME;
	atom.m_str = recipe.title;
	entry.append( atom );
}

void kio_krecipesProtocol::createCategoryEntry( UDSEntry &entry, const Element &category )
{
	UDSAtom atom;
	atom.m_uds = KIO::UDS_NAME;
	atom.m_str = category.name;
	entry.append( atom );

	atom.m_uds = KIO::UDS_FILE_TYPE;
	atom.m_long = S_IFDIR;
	entry.append( atom );
}

/* KIO slave registration */

extern "C" { int KDE_EXPORT kdemain(int argc, char **argv); }

static const KCmdLineOptions options[] =
{
    { "+protocol", I18N_NOOP( "Protocol name" ), 0 },
    { "+pool", I18N_NOOP( "Socket name" ), 0 },
    { "+app", I18N_NOOP( "Socket name" ), 0 },
    KCmdLineLastOption
};

int kdemain( int argc, char **argv )
{
	putenv(strdup("SESSION_MANAGER="));
	KApplication::disableAutoDcopRegistration();
	KCmdLineArgs::init(argc, argv, "kio_krecipes", 0, 0, 0, 0);
	KCmdLineArgs::addCmdLineOptions( options );
	KApplication app( false, false ); // note: GUI disabled

	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	if ( args->count() == 3 ) {
		kio_krecipesProtocol slave( args->arg(1), args->arg(2) );
		slave.dispatchLoop();
	}
	return 0;
}
