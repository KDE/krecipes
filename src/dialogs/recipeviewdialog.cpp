/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "recipeviewdialog.h"

#include <qlayout.h>
#include <qstyle.h>
#include <qfile.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <QTextDocument>

#include <kapplication.h>
#include <kcursor.h>
#include <kdebug.h>
#include <khtmlview.h>
#include <khtml_part.h>
#include <klocale.h>
#include <kmainwindow.h>
#include <kprogressdialog.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <kconfig.h>
#include <kglobal.h>

#include "datablocks/mixednumber.h"
#include "backends/recipedb.h"
#include "exporters/xsltexporter.h"
#include "recipeprintpreview.h"

RecipeViewDialog::RecipeViewDialog( QWidget *parent, RecipeDB *db, int recipeID ) : Q3VBox( parent ),
	database(db)
{
	// Initialize UI Elements
	recipeView = new KHTMLPart( this );

	connect( database, SIGNAL(recipeRemoved(int)), SLOT(recipeRemoved(int)) );

	tmp_filename = KStandardDirs::locateLocal( "tmp", "krecipes_recipe_view" );

	//----------Load  the recipe --------
	if ( recipeID != -1 )
		loadRecipe( recipeID );
}

RecipeViewDialog::~RecipeViewDialog()
{
	if ( recipe_loaded )
		removeOldFiles();
}

bool RecipeViewDialog::loadRecipe( int recipeID )
{
	Q3ValueList<int> ids;
	ids.append( recipeID );
	return loadRecipes( ids );
}

bool RecipeViewDialog::loadRecipes( const Q3ValueList<int> &ids, const QString &layoutConfig )
{
	KApplication::setOverrideCursor( Qt::WaitCursor );

	// Remove any files created by the last recipe loaded
	removeOldFiles();

	ids_loaded = ids; //need to save these ids in order to delete the html files later...make sure this comes after the call to removeOldFiles()
	recipe_loaded = ( ids.count() > 0 && ids[ 0 ] >= 0 );

	bool success = showRecipes( ids, layoutConfig );

	KApplication::restoreOverrideCursor();
	return success;
}

bool RecipeViewDialog::showRecipes( const Q3ValueList<int> &ids, const QString &layoutConfig )
{
	KProgressDialog * progress_dialog = 0;

	if ( ids.count() > 1 )  //we don't want a progress bar coming up when there is only one recipe... it may show up during the splash screen
	{
		progress_dialog = new KProgressDialog( this, QString::null, i18n( "Opening recipes, please wait..." ) );
		progress_dialog->setObjectName( "open_progress_dialog" );
		progress_dialog->setModal( true );
		progress_dialog->resize( 240, 80 );
	}

	XSLTExporter html_generator( tmp_filename + ".html", "html" );
	if ( layoutConfig != QString::null ) {
            KConfigGroup config(KGlobal::config(), "Page Setup" );
		QString styleFile = config.readEntry( layoutConfig+"Layout", KStandardDirs::locate( "appdata", "layouts/None.klo" ) );
		if ( !styleFile.isEmpty() && QFile::exists( styleFile ) )
			html_generator.setStyle( styleFile );

		QString templateFile = config.readEntry( layoutConfig+"Template", KStandardDirs::locate( "appdata", "layouts/Default.xsl" ) );
		if ( !templateFile.isEmpty() && QFile::exists( templateFile ) )
			html_generator.setTemplate( templateFile );
	}

	html_generator.exporter( ids, database, progress_dialog ); //writes the generated HTML to 'tmp_filename+".html"'
	if ( progress_dialog && progress_dialog->wasCancelled() ) {
		delete progress_dialog;
		return false;
	}

	delete recipeView;              // Temporary workaround
	recipeView = new KHTMLPart( this ); // to avoid the problem of caching images of KHTMLPart

	//KDE4 port
	KParts::OpenUrlArguments argsUrl (recipeView->arguments());
	argsUrl.setReload( true ); // Don't use the cache
	recipeView->setArguments( argsUrl );
	
	KParts::BrowserArguments args;
	recipeView->browserExtension()->setBrowserArguments(args);

	KUrl url;
	url.setPath( tmp_filename + ".html" );
	recipeView->openUrl( url );
	recipeView->show();
	kDebug() << "Opening URL: " << Qt::escape(url.prettyUrl()) << endl;

	delete progress_dialog;
	return true;
}

void RecipeViewDialog::print()
{
	if ( recipe_loaded ) {
		RecipePrintPreview preview( this, database, ids_loaded );
		preview.exec();
	}
}

void RecipeViewDialog::printNoPreview( void )
{
	if ( recipe_loaded ) {
		recipeView->view() ->print();
	}
}

void RecipeViewDialog::reload( const QString &layoutConfig )
{
	loadRecipes( ids_loaded, layoutConfig );
}

void RecipeViewDialog::removeOldFiles()
{
	if ( ids_loaded.count() > 0 ) {
		RecipeList recipe_list;
		database->loadRecipes( &recipe_list, RecipeDB::Title, ids_loaded );

		Q3ValueList<int> recipe_ids;
		for ( RecipeList::const_iterator it = recipe_list.begin(); it != recipe_list.end(); ++it )
			recipe_ids << ( *it ).recipeID;

		XSLTExporter::removeHTMLFiles( tmp_filename, recipe_ids );
	}
}

void RecipeViewDialog::recipeRemoved( int id )
{
	//if the deleted recipe is loaded, clean the view up
	if ( ids_loaded.find(id) != ids_loaded.end() ) {
		Recipe recipe; database->loadRecipe( &recipe, RecipeDB::Title, id );
		XSLTExporter::removeHTMLFiles( tmp_filename, recipe.recipeID );
		ids_loaded.remove(id);
	}
}

#include "recipeviewdialog.moc"
