/*
 * Copyright (C) 2003 Unai Garro <uga@ee.ed.ac.uk>
 */

#include "krecipes.h"
#include "pref.h"

#include <qdragobject.h>
#include <kprinter.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>

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

#include "setupwizard.h"

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

    // allow the view to change the statusbar and caption
    connect(m_view, SIGNAL(signalChangeStatusbar(const QString&)),
            this,   SLOT(changeStatusbar(const QString&)));
    connect(m_view, SIGNAL(signalChangeCaption(const QString&)),
            this,   SLOT(changeCaption(const QString&)));

    // Enable/Disable the Save Button (Initialize disabled, and connect signal)

    connect(this->m_view, SIGNAL(enableSaveOption(bool)), this, SLOT(enableSaveOption(bool)));
    enableSaveOption(false); // Disables saving initially

    // Init the setup wizard if necessary
    wizard();
}

Krecipes::~Krecipes()
{
}


void Krecipes::setupActions()
{
    KStdAction::openNew(this, SLOT(fileNew()), actionCollection());
    KStdAction::open(this, SLOT(fileOpen()), actionCollection());
    saveAction=KStdAction::save(this, SLOT(fileSave()), actionCollection());
    KStdAction::saveAs(this, SLOT(fileSaveAs()), actionCollection());
    KStdAction::print(this, SLOT(filePrint()), actionCollection());
    KStdAction::quit(kapp, SLOT(quit()), actionCollection());

    m_toolbarAction = KStdAction::showToolbar(this, SLOT(optionsShowToolbar()), actionCollection());
    m_statusbarAction = KStdAction::showStatusbar(this, SLOT(optionsShowStatusbar()), actionCollection());

    KStdAction::keyBindings(this, SLOT(optionsConfigureKeys()), actionCollection());
    KStdAction::configureToolbars(this, SLOT(optionsConfigureToolbars()), actionCollection());
    KStdAction::preferences(this, SLOT(optionsPreferences()), actionCollection());

    // this doesn't do anything useful.  it's just here to illustrate
    // how to insert a custom menu and menu item
    KAction *custom = new KAction(i18n("Cus&tom Menuitem"), 0,
                                  this, SLOT(optionsPreferences()),
                                  actionCollection(), "custom_action");
    createGUI();
}

void Krecipes::saveProperties(KConfig *config)
{
    // the 'config' object points to the session managed
    // config file.  anything you write here will be available
    // later when this app is restored

    //if (!m_view->currentURL().isNull())
      //  config->writeEntry("lastURL", m_view->currentURL());
}

void Krecipes::readProperties(KConfig *config)
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
    /*KURL file_url = KFileDialog::getSaveURL();
    if (!file_url.isEmpty() && !file_url.isMalformed())
    {
        // save your info, here
    }*/
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
        m_view->print(&p, metrics.height(), metrics.width());

        // and send the result to the printer
        p.end();
    }
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
    KKeyDialog::configureKeys(actionCollection(), "krecipesui.rc");
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
    // popup some sort of preference dialog, here
    KrecipesPreferences dlg;
    if (dlg.exec())
    {
        // redo your settings
    }
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

void Krecipes::wizard(void)
{
KConfig *config=kapp->config();
config->setGroup("Wizard");


bool setupDone=config->readBoolEntry( "SystemSetup",false);
if (!setupDone)
{
SetupWizard* setupWizard=new SetupWizard();
setupWizard->exec();
}

}

#include "krecipes.moc"
