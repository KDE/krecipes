/***************************************************************************
*   Copyright (C) 2003-2004 by                                            *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "pref.h"
#include "krecipes.h"
#include "krecipesview.h"
#include "dialogs/recipeviewdialog.h"
#include "dialogs/recipeinputdialog.h"
#include "dialogs/selectrecipedialog.h"
#include "dialogs/ingredientsdialog.h"
#include "dialogs/propertiesdialog.h"
#include "dialogs/shoppinglistdialog.h"
#include "dialogs/categorieseditordialog.h"
#include "dialogs/authorsdialog.h"
#include "dialogs/unitsdialog.h"
#include "dialogs/ingredientmatcherdialog.h"
#include "dialogs/dbimportdialog.h"

#include "gui/pagesetupdialog.h"

#include "importers/kreimporter.h"
#include "importers/mmfimporter.h"
#include "importers/mx2importer.h"
#include "importers/mxpimporter.h"
#include "importers/nycgenericimporter.h"
#include "importers/recipemlimporter.h"
#include "importers/rezkonvimporter.h"
#include "importers/kredbimporter.h"

#include "recipe.h"
#include "DBBackend/recipedb.h"

#include <qdragobject.h>
#include <kprinter.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qmessagebox.h>

#include <kprogress.h>
#include <kmessagebox.h>
#include <kglobal.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include <kkeydialog.h>
#include <kaccel.h>
#include <kio/netaccess.h>
#include <kfiledialog.h>
#include <kconfig.h>
#include <kcursor.h>

#include <kedittoolbar.h>
#include <kstdaccel.h>
#include <kaction.h>
#include <kstdaction.h> 
//Settings headers
#include <kdeversion.h>

Krecipes::Krecipes()
		: KMainWindow( 0, "Krecipes" ),
		m_view( new KrecipesView( this ) ),
		m_printer( 0 )
{


	// accept dnd
	setAcceptDrops( true );

	// tell the KMainWindow that this is indeed the main widget
	setCentralWidget( m_view );

	// then, setup our actions
	setupActions();

	// and a status bar
	statusBar() ->show();

	// apply the saved mainwindow settings, if any, and ask the mainwindow
	// to automatically save settings if changed: window size, toolbar
	// position, icon size, etc.
	setAutoSaveSettings();


	// allow the view to change the statusbar and caption
	connect( m_view, SIGNAL( signalChangeStatusbar( const QString& ) ),
	         this, SLOT( changeStatusbar( const QString& ) ) );
	connect( m_view, SIGNAL( signalChangeCaption( const QString& ) ),
	         this, SLOT( changeCaption( const QString& ) ) );

	connect( m_view, SIGNAL( panelShown( KrePanel, bool ) ), SLOT( updateActions( KrePanel, bool ) ) );


	// Enable/Disable the Save Button (Initialize disabled, and connect signal)

	connect( m_view, SIGNAL( enableSaveOption( bool ) ), this, SLOT( enableSaveOption( bool ) ) );
	enableSaveOption( false ); // Disables saving initially

	parsing_file_dlg = new KDialog( this, "parsing_file_dlg", true, Qt::WX11BypassWM );
	QLabel *parsing_file_dlg_label = new QLabel( i18n( "Gathering recipe data from file.\nPlease wait..." ), parsing_file_dlg );
	parsing_file_dlg_label->setFrameStyle( QFrame::Box | QFrame::Raised );
	( new QVBoxLayout( parsing_file_dlg ) ) ->addWidget( parsing_file_dlg_label );
	parsing_file_dlg->adjustSize();
	//parsing_file_dlg->setFixedSize(parsing_file_dlg->size());
}

Krecipes::~Krecipes()
{}

void Krecipes::updateActions( KrePanel panel, bool show )
{
	switch ( panel ) {
	case RecipeView: {
			exportAction->setEnabled( show );
			printAction->setEnabled( show );
			reloadAction->setEnabled( show );

			//can't edit when there are multiple recipes loaded
			if ( show && m_view->viewPanel->recipesLoaded() == 1 ) {
				editAction->setEnabled( true );
			}
			else
				editAction->setEnabled( false );

			break;
		}
	case SelectP: {
			exportAction->setEnabled( show );
			editAction->setEnabled( show );
			break;
		}
	default:
		break;
	}
}

void Krecipes::setupActions()
{
	KIconLoader il;

	printAction = KStdAction::print( this, SLOT( filePrint() ), actionCollection() );
	reloadAction = new KAction( i18n( "Reloa&d" ), "reload", Key_F5, m_view, SLOT( reloadDisplay() ), actionCollection(), "reload_action" );

	editAction = new KAction( i18n( "&Edit Recipe" ), "edit", CTRL + Key_E,
	                          m_view, SLOT( editRecipe() ),
	                          actionCollection(), "edit_action" );


	KAction *action = KStdAction::openNew( this, SLOT( fileNew() ), actionCollection() );
	action->setText( i18n( "&New Recipe" ) );

	saveAction = KStdAction::save( this, SLOT( fileSave() ), actionCollection() );

	KStdAction::quit( kapp, SLOT( quit() ), actionCollection() );

	m_toolbarAction = KStdAction::showToolbar( this, SLOT( optionsShowToolbar() ), actionCollection() );
	m_statusbarAction = KStdAction::showStatusbar( this, SLOT( optionsShowStatusbar() ), actionCollection() );

	KStdAction::keyBindings( this, SLOT( optionsConfigureKeys() ), actionCollection() );
	KStdAction::configureToolbars( this, SLOT( optionsConfigureToolbars() ), actionCollection() );
	KStdAction::preferences( this, SLOT( optionsPreferences() ), actionCollection() );

	( void ) new KAction( i18n( "Import from File..." ), CTRL + Key_I,
	                      this, SLOT( import() ),
	                      actionCollection(), "import_action" );

	( void ) new KAction( i18n( "Import from Database..." ), 0,
	                      this, SLOT( kreDBImport() ),
	                      actionCollection(), "import_db_action" );

	exportAction = new KAction( i18n( "Export..." ), 0,
	                            this, SLOT( fileExport() ),
	                            actionCollection(), "export_action" );

	( void ) new KAction( i18n( "Page Setup..." ), 0,
	                      this, SLOT( pageSetupSlot() ),
	                      actionCollection(), "page_setup_action" );

	updateActions( SelectP, true );
	updateActions( RecipeView, false );

	createGUI();
}

void Krecipes::saveProperties( KConfig * )
{
	// the 'config' object points to the session managed
	// config file.  anything you write here will be available
	// later when this app is restored

	//if (!m_view->currentURL().isNull())
	//  config->writeEntry("lastURL", m_view->currentURL());
}

void Krecipes::readProperties( KConfig * )
{
	// the 'config' object points to the session managed
	// config file.  this function is automatically called whenever
	// the app is being restored.  read in here whatever you wrote
	// in 'saveProperties'

	//QString url = config->readEntry("lastURL");

	//if (!url.isNull())
	//  m_view->openURL(KURL(url));
}

void Krecipes::dragEnterEvent( QDragEnterEvent *event )
{
	// accept uri drops only
	event->accept( QUriDrag::canDecode( event ) );
}


void Krecipes::fileNew()
{

	// Create a new element (Element depends on active panel. New recipe by default)
	m_view->createNewElement();
}

void Krecipes::fileOpen()
{
	// this slot is called whenever the File->Open menu is selected,
	// the Open shortcut is pressed (usually CTRL+O) or the Open toolbar
	// button is clicked
	/*
	    // this brings up the generic open dialog
	    KURL url = KURLRequesterDlg::getURL(QString::null, this, i18n("Open Location") );
	*/ 
	// standard filedialog
	/*KURL url = KFileDialog::getOpenURL(QString::null, QString::null, this, i18n("Open Location"));
	if (!url.isEmpty())
	    m_view->openURL(url);*/
}

void Krecipes::fileSave()
{
	// this slot is called whenever the File->Save menu is selected,
	// the Save shortcut is pressed (usually CTRL+S) or the Save toolbar
	// button is clicked
	m_view->save();
}

void Krecipes::fileExport()
{
	// this slot is called whenever the File->Export menu is selected,
	m_view->exportRecipe();
}

void Krecipes::filePrint()
{
	m_view->print();
}

void Krecipes::import()
{
	KFileDialog file_dialog( QString::null,
	                         "*.kre *.kreml|Krecipes (*.kre, *.kreml)\n"
	                         "*.mx2|MasterCook (*.mx2)\n"
	                         "*.mxp *.txt|MasterCook Export (*.mxp, *.txt)\n"
	                         "*.mmf *.txt|Meal-Master (*.mmf, *.txt)\n"
	                         "*.txt|\"Now You're Cooking\" Generic Export (*.txt)\n"
	                         "*.xml *.recipeml|RecipeML (*.xml, *.recipeml)\n"
	                         "*.rk *.txt|Rezkonv (*.rk, *.txt)",
	                         this,
	                         "file_dialog",
	                         true
	                       );
	file_dialog.setMode( KFile::Files );

	if ( file_dialog.exec() == KFileDialog::Accepted ) {
		QStringList warnings_list;

		QString selected_filter = file_dialog.currentFilter();

		BaseImporter *importer;
		if ( selected_filter == "*.mxp *.txt" )
			importer = new MXPImporter();
		else if ( selected_filter == "*.mmf *.txt" )
			importer = new MMFImporter();
		else if ( selected_filter == "*.txt" )
			importer = new NYCGenericImporter();
		else if ( selected_filter == "*.mx2" )
			importer = new MX2Importer();
		else if ( selected_filter == "*.kre *.kreml" )
			importer = new KreImporter();
		else if ( selected_filter == "*.xml *.recipeml" )
			importer = new RecipeMLImporter();
		else if ( selected_filter == "*.rk *.txt" )
			importer = new RezkonvImporter();
		else {
			KMessageBox::sorry( this,
			                    QString( i18n( "Filter \"%1\" not recognized.\n"
			                                   "Please select one of the provided filters." ) ).arg( selected_filter ),
			                    i18n( "Unrecognized Filter" )
			                  );
			import(); //let's try again :)
			return ;
		}

		parsing_file_dlg->show();
		KApplication::setOverrideCursor( KCursor::waitCursor() );
		importer->parseFiles( file_dialog.selectedFiles() );
		parsing_file_dlg->hide();
		KApplication::restoreOverrideCursor();

		m_view->import( *importer );

		if ( !importer->getMessages().isEmpty() ) {
			KTextEdit * warningEdit = new KTextEdit( this );
			warningEdit->setTextFormat( Qt::RichText );
			warningEdit->setText( QString( i18n( "NOTE: We recommend that all recipes generating warnings be checked to ensure that they were properly imported, and no loss of recipe data has occurred.<br><br>" ) ) + importer->getMessages() );
			warningEdit->setReadOnly( true );

			KDialogBase showWarningsDlg( KDialogBase::Swallow, i18n( "Import Warnings" ), KDialogBase::Ok, KDialogBase::Default, this );
			showWarningsDlg.setMainWidget( warningEdit ); //KDialogBase will delete warningEdit for us
			showWarningsDlg.resize( QSize( 550, 250 ) );
			showWarningsDlg.exec();
		}

		delete importer;
	}
}

void Krecipes::kreDBImport()
{
	DBImportDialog importOptions;
	if ( importOptions.exec() == QDialog::Accepted ) {
		//Get all params, even if we don't use them
		QString host, user, pass, table;
		importOptions.serverParams( host, user, pass, table );

		KreDBImporter importer( importOptions.dbType(), host, user, pass ); //last 3 params may or may not be even used (depends on dbType)

		parsing_file_dlg->show();
		KApplication::setOverrideCursor( KCursor::waitCursor() );
		QStringList tables;
		if ( importOptions.dbType() == "SQLite" )
			tables << importOptions.dbFile();
		else //MySQL or PostgreSQL
			tables << table;
		importer.parseFiles( tables );
		parsing_file_dlg->hide();
		KApplication::restoreOverrideCursor();

		QString error = importer.getErrorMsg();
		if ( !error.isEmpty() ) {
			KMessageBox::sorry( this, error );
		}
		else
			m_view->import( importer );
	}
}

void Krecipes::pageSetupSlot()
{
	Recipe recipe;
	m_view->selectPanel->getCurrentRecipe( &recipe );

	PageSetupDialog *page_setup = new PageSetupDialog( this, recipe );
	page_setup->exec();

	delete page_setup;
}

//return true to close app
bool Krecipes::queryClose()
{
	if ( !m_view->inputPanel->everythingSaved() ) {
		switch ( KMessageBox::questionYesNoCancel( this,
		         i18n( "A recipe contains unsaved changes.\n"
		               "Do you want to save the changes before exiting?" ),
		         i18n( "Unsaved Changes" ) ) ) {
		case KMessageBox::Yes:
			return m_view->save();
		case KMessageBox::No:
			return true;
		case KMessageBox::Cancel:
			return false;
		default:
			return true;
		}
	}
	else
		return true;
}

void Krecipes::optionsShowToolbar()
{
	// this is all very cut and paste code for showing/hiding the
	// toolbar
	if ( m_toolbarAction->isChecked() )
		toolBar() ->show();
	else
		toolBar() ->hide();
}

void Krecipes::optionsShowStatusbar()
{
	// this is all very cut and paste code for showing/hiding the
	// statusbar
	if ( m_statusbarAction->isChecked() )
		statusBar() ->show();
	else
		statusBar() ->hide();
}

void Krecipes::optionsConfigureKeys()
{
#if KDE_IS_VERSION(3,1,92 )
	// for KDE 3.2: KKeyDialog::configureKeys is deprecated
	KKeyDialog::configure( actionCollection(), this, true );
#else

	KKeyDialog::configureKeys( actionCollection(), "krecipesui.rc" );
#endif

}

void Krecipes::optionsConfigureToolbars()
{
	// use the standard toolbar editor
#if defined(KDE_MAKE_VERSION)
# if KDE_VERSION >= KDE_MAKE_VERSION(3,1,0)
	saveMainWindowSettings( KGlobal::config(), autoSaveGroup() );
# else

	saveMainWindowSettings( KGlobal::config() );
# endif
#else

	saveMainWindowSettings( KGlobal::config() );
#endif

	KEditToolbar dlg( actionCollection() );
	connect( &dlg, SIGNAL( newToolbarConfig() ), this, SLOT( newToolbarConfig() ) );
	dlg.exec();
}

void Krecipes::newToolbarConfig()
{
	// this slot is called when user clicks "Ok" or "Apply" in the toolbar editor.
	// recreate our GUI, and re-apply the settings (e.g. "text under icons", etc.)
	createGUI();

#if defined(KDE_MAKE_VERSION)
# if KDE_VERSION >= KDE_MAKE_VERSION(3,1,0)

	applyMainWindowSettings( KGlobal::config(), autoSaveGroup() );
# else

	applyMainWindowSettings( KGlobal::config() );
# endif
#else

	applyMainWindowSettings( KGlobal::config() );
#endif
}

void Krecipes::optionsPreferences()
{

	// popup some sort of preference dialog, here
	KrecipesPreferences dlg( this );
	if ( dlg.exec() ) {}

}

void Krecipes::changeStatusbar( const QString& text )
{
	// display the text on the statusbar
	statusBar() ->message( text );
}

void Krecipes::changeCaption( const QString& text )
{
	// display the text on the caption
	setCaption( text );
}
void Krecipes::enableSaveOption( bool en )
{
	saveAction->setEnabled( en );
}

#include "krecipes.moc"
