/*
 * Copyright (C) 2003 Unai Garro <ugarro@users.sourceforge.net>
 */

 #include "pref.h"
#include "krecipes.h"
#include "krecipesview.h"
#include "recipeinputdialog.h"
#include "selectrecipedialog.h"
#include "ingredientsdialog.h"
#include "propertiesdialog.h"
#include "shoppinglistdialog.h"
#include "categorieseditordialog.h"
#include "authorsdialog.h"
#include "unitsdialog.h"

#include "importers/kreimporter.h"
#include "importers/mmfimporter.h"
#include "importers/mx2importer.h"
#include "importers/mxpimporter.h"
#include "importers/nycgenericimporter.h"

#include "gui/pagesetupdialog.h"

#include "recipe.h"
#include "DBBackend/recipedb.h"

#include <qdragobject.h>
#include <kprinter.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>

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

#include <kedittoolbar.h>
#include <kstdaccel.h>
#include <kaction.h>
#include <kstdaction.h>
//Settings headers
#include <kdeversion.h>
#if defined(KDE_MAKE_VERSION)
  # if KDE_VERSION > KDE_MAKE_VERSION(3,1,4)
     #include <kautoconfigdialog.h>
  # endif
#endif

#include "serverprefs.h"
#include "unitsprefs.h"
#include "importprefs.h"

Krecipes::Krecipes()
    : KMainWindow( 0, "Krecipes" ),
      m_view(new KrecipesView(this)),
      m_printer(0)
{


    // accept dnd
    setAcceptDrops(true);

    // tell the KMainWindow that this is indeed the main widget
    setCentralWidget(m_view);

    // then, setup our actions
    setupActions();

    // and a status bar
    statusBar()->show();

    // apply the saved mainwindow settings, if any, and ask the mainwindow
    // to automatically save settings if changed: window size, toolbar
    // position, icon size, etc.
    setAutoSaveSettings();



    // Resize if the window is too small so the buttons are shown
    QSize wsize=size();
    if (wsize.width()<640)
    	{
	wsize.setWidth(640);
	resize(wsize);
	}

    // allow the view to change the statusbar and caption
    connect(m_view, SIGNAL(signalChangeStatusbar(const QString&)),
            this,   SLOT(changeStatusbar(const QString&)));
    connect(m_view, SIGNAL(signalChangeCaption(const QString&)),
            this,   SLOT(changeCaption(const QString&)));

    // Enable/Disable the Save Button (Initialize disabled, and connect signal)

    connect(this->m_view, SIGNAL(enableSaveOption(bool)), this, SLOT(enableSaveOption(bool)));
    enableSaveOption(false); // Disables saving initially

    // Enable/Disable the SaveAs Button (Initialize disabled, and connect signal)

    connect(this->m_view->selectPanel, SIGNAL(recipeSelected(bool)), saveAsAction, SLOT(setEnabled(bool)));

}

Krecipes::~Krecipes()
{
}


void Krecipes::setupActions()
{
    KStdAction::openNew(this, SLOT(fileNew()), actionCollection());
    KStdAction::open(this, SLOT(fileOpen()), actionCollection());
    saveAction=KStdAction::save(this, SLOT(fileSave()), actionCollection());
    saveAsAction=KStdAction::saveAs(this, SLOT(fileSaveAs()), actionCollection());
    saveAsAction->setEnabled(false);
    KStdAction::print(this, SLOT(filePrint()), actionCollection());
    KStdAction::quit(kapp, SLOT(quit()), actionCollection());

    m_toolbarAction = KStdAction::showToolbar(this, SLOT(optionsShowToolbar()), actionCollection());
    m_statusbarAction = KStdAction::showStatusbar(this, SLOT(optionsShowStatusbar()), actionCollection());

    KStdAction::keyBindings(this, SLOT(optionsConfigureKeys()), actionCollection());
    KStdAction::configureToolbars(this, SLOT(optionsConfigureToolbars()), actionCollection());
    KStdAction::preferences(this, SLOT(optionsPreferences()), actionCollection());

    KAction *import = new KAction(i18n("Import..."), CTRL+Key_I,
                                  this, SLOT(import()),
                                  actionCollection(), "import_action");

    KAction *pageSetup = new KAction(i18n("Page Setup..."), 0,
                                  this, SLOT(pageSetupSlot()),
                                  actionCollection(), "page_setup_action");

    createGUI();
}

void Krecipes::saveProperties(KConfig *)
{
    // the 'config' object points to the session managed
    // config file.  anything you write here will be available
    // later when this app is restored

    //if (!m_view->currentURL().isNull())
      //  config->writeEntry("lastURL", m_view->currentURL());
}

void Krecipes::readProperties(KConfig *)
{
    // the 'config' object points to the session managed
    // config file.  this function is automatically called whenever
    // the app is being restored.  read in here whatever you wrote
    // in 'saveProperties'

    //QString url = config->readEntry("lastURL");

    //if (!url.isNull())
      //  m_view->openURL(KURL(url));
}

void Krecipes::dragEnterEvent(QDragEnterEvent *event)
{
    // accept uri drops only
    event->accept(QUriDrag::canDecode(event));
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

void Krecipes::fileSaveAs()
{
    // this slot is called whenever the File->Save As menu is selected,
m_view->exportRecipe();
}

void Krecipes::filePrint()
{
    // this slot is called whenever the File->Print menu is selected,
    // the Print shortcut is pressed (usually CTRL+P) or the Print toolbar
    // button is clicked
    if (!m_printer) m_printer = new KPrinter;
    if (m_printer->setup(this))
    {
        // setup the printer.  with Qt, you always "print" to a
        // QPainter.. whether the output medium is a pixmap, a screen,
        // or paper
        QPainter p;
        p.begin(m_printer);

        // we let our view do the actual printing
        QPaintDeviceMetrics metrics(m_printer);
        m_view->print(&p, metrics.width(), metrics.height());

        // and send the result to the printer
        p.end();
    }
}

void Krecipes::import()
{
	KFileDialog file_dialog( i18n("Import Recipes"),
	  "*.kre *.kreml|Krecipes (*.kre, *.kreml)\n"
	  "*.mx2|MasterCook (*.mx2)\n"
	  "*.mxp *.txt|MasterCook Export (*.mxp, *.txt)\n"
	  "*.mmf *.txt|Meal-Master Format (*.mmf, *.txt)\n"
	  "*.txt|\"Now You're Cooking\" Generic Export (*.txt)",
	  this,
	  "file_dialog",
	  true
	);
	file_dialog.setMode( KFile::Files );

	if ( file_dialog.exec() == KFileDialog::Accepted )
	{
		QStringList warnings_list;

		QString selected_filter = file_dialog.currentFilter();
		QStringList files = file_dialog.selectedFiles();

		for ( QStringList::const_iterator it = files.begin(); it != files.end(); ++it )
		{
			BaseImporter *importer;
			if ( selected_filter == "*.mxp *.txt" )
				importer = new MXPImporter( *it );
			else if ( selected_filter == "*.mmf *.txt" )
				importer = new MMFImporter( *it );
			else if ( selected_filter == "*.txt" )
				importer = new NYCGenericImporter( *it );
			else if ( selected_filter == "*.mx2" )
				importer = new MX2Importer( *it );
			else if ( selected_filter == "*.kre *.kreml" )
				importer = new KreImporter( *it );
			else
			{
				KMessageBox::sorry( this,
				  QString(i18n("Filter \"%1\" not recognized.\n"
				    "Please select one of the provided filters.")).arg(selected_filter),
				  i18n("Unrecognized Filter")
				);
				import(); //let's try again :)
				return;
			}

			QString error = importer->getErrorMsg();
			if ( error != QString::null )
				KMessageBox::error( this, QString(i18n("Error importing file %1\n%2")).arg(*it).arg(error) );

			if ( importer->getWarningMsgs().count() > 0 )
			{
				warnings_list += QString(i18n("The file \"%1\" generated the following warnings:")).arg(*it);
				warnings_list += importer->getWarningMsgs();
			}

			m_view->import( *importer );
			delete importer;
		}

		if ( warnings_list.count() > 0 )
		{
			warnings_list.prepend(i18n("NOTE: We recommend that all recipes generating warnings be checked to ensure that they were properly imported, and no loss of recipe data has occured."));

			KTextEdit *warningEdit = new KTextEdit( this );
			warningEdit->setText( warnings_list.join("\n\n") );
			warningEdit->setReadOnly(true);

			KDialogBase showWarningsDlg( KDialogBase::Swallow, i18n("Import warnings"), KDialogBase::Ok, KDialogBase::Default, this );
			showWarningsDlg.setMainWidget( warningEdit ); //KDialogBase will delete warningEdit for us
			showWarningsDlg.resize( QSize(550,250) );
			showWarningsDlg.exec();
		}

		//TODO: to just reload the active panel would be preferable
		m_view->selectPanel->reload();
		m_view->ingredientsPanel->reload();
		m_view->propertiesPanel->reload();
		m_view->unitsPanel->reload();
		m_view->shoppingListPanel->reload();
		m_view->categoriesPanel->reload();
		m_view->authorsPanel->reload();
	}
}

void Krecipes::pageSetupSlot()
{
	Recipe recipe;
	m_view->selectPanel->getCurrentRecipe(&recipe);

	PageSetupDialog *page_setup = new PageSetupDialog(this,recipe);
	page_setup->exec();

	delete page_setup;
}

//return true to close app
bool Krecipes::queryClose()
{
	if ( !m_view->inputPanel->everythingSaved() )
	{
		switch( KMessageBox::questionYesNoCancel( this,
		  i18n("A recipe contains unsaved changes.\n"
		  "Do you want to save the changes before exiting?"),
		  i18n("Unsaved Changes") ) )
		{
		case KMessageBox::Yes: m_view->save();
		case KMessageBox::No: return true;
		case KMessageBox::Cancel: return false;
		default: return true;
		}
	}
	else
		return true;
}

void Krecipes::optionsShowToolbar()
{
    // this is all very cut and paste code for showing/hiding the
    // toolbar
    if (m_toolbarAction->isChecked())
        toolBar()->show();
    else
        toolBar()->hide();
}

void Krecipes::optionsShowStatusbar()
{
    // this is all very cut and paste code for showing/hiding the
    // statusbar
    if (m_statusbarAction->isChecked())
        statusBar()->show();
    else
        statusBar()->hide();
}

void Krecipes::optionsConfigureKeys()
{
#if defined(KDE_MAKE_VERSION)
# if KDE_VERSION > KDE_MAKE_VERSION(3,1,4)
   // for KDE 3.2: KKeyDialog::configureKeys is deprecated
   KKeyDialog::configure(actionCollection(), this, true);
 #else
    KKeyDialog::configureKeys(actionCollection(), "krecipesui.rc");
#endif
 #else
  KKeyDialog::configureKeys(actionCollection(), "krecipesui.rc");
#endif
}

void Krecipes::optionsConfigureToolbars()
{
    // use the standard toolbar editor
#if defined(KDE_MAKE_VERSION)
# if KDE_VERSION >= KDE_MAKE_VERSION(3,1,0)
    saveMainWindowSettings(KGlobal::config(), autoSaveGroup());
# else
    saveMainWindowSettings(KGlobal::config());
# endif
#else
    saveMainWindowSettings(KGlobal::config());
#endif
}

void Krecipes::newToolbarConfig()
{
    // this slot is called when user clicks "Ok" or "Apply" in the toolbar editor.
    // recreate our GUI, and re-apply the settings (e.g. "text under icons", etc.)
    createGUI();

#if defined(KDE_MAKE_VERSION)
# if KDE_VERSION >= KDE_MAKE_VERSION(3,1,0)
    applyMainWindowSettings(KGlobal::config(), autoSaveGroup());
# else
    applyMainWindowSettings(KGlobal::config());
# endif
#else
    applyMainWindowSettings(KGlobal::config());
#endif
}

void Krecipes::optionsPreferences()
{
#if defined(KDE_MAKE_VERSION)
# if KDE_VERSION > KDE_MAKE_VERSION(3,1,4)
     if(KAutoConfigDialog::showDialog("settings"))
		return;

    KAutoConfigDialog *dialog = new KAutoConfigDialog(this, "settings");
    dialog->addPage(new serverprefs(0, "serverprefs"), i18n("Server Settings"), "Server", "identity", i18n("Database Server Options"));
    dialog->addPage(new unitsprefs(0, "NumberFormat"), i18n("Units"), "Units", "frac", i18n("Customize Units"));
    dialog->addPage(new importprefs(0, "Import"), i18n("Units"), "Import", "redo", i18n("Recipe Import Options"));
# else
   // popup some sort of preference dialog, here
    KrecipesPreferences dlg(this);
    if (dlg.exec())
    {}
# endif
#else
      // popup some sort of preference dialog, here
    KrecipesPreferences dlg(this);
    if (dlg.exec())
    {}
#endif
}

void Krecipes::changeStatusbar(const QString& text)
{
    // display the text on the statusbar
    statusBar()->message(text);
}

void Krecipes::changeCaption(const QString& text)
{
    // display the text on the caption
    setCaption(text);
}
void Krecipes::enableSaveOption(bool en)
{
saveAction->setEnabled(en);
}

#include "krecipes.moc"
