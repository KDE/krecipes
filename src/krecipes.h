/*
 * Copyright (C) 2003 Unai Garro <uga@ee.ed.ac.uk>
 */

#ifndef _KRECIPES_H_
#define _KRECIPES_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kapplication.h>
#include <kmainwindow.h>

#include "krecipesview.h"

class KPrinter;
class KToggleAction;
class KURL;

/**
 * This class serves as the main window for Krecipes.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author $AUTHOR <$EMAIL>
 * @version $APP_VERSION
 */
class Krecipes : public KMainWindow
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    Krecipes();

    /**
     * Default Destructor
     */
    virtual ~Krecipes();


protected:
    /**
     * Overridden virtuals for Qt drag 'n drop (XDND)
     */
    virtual void dragEnterEvent(QDragEnterEvent *event);

protected:
    /**
     * This function is called when it is time for the app to save its
     * properties for session management purposes.
     */
    void saveProperties(KConfig *);

    /**
     * This function is called when this app is restored.  The KConfig
     * object points to the session management config file that was saved
     * with @ref saveProperties
     */
    void readProperties(KConfig *);


private slots:
    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void filePrint();
    void optionsShowToolbar();
    void optionsShowStatusbar();
    void optionsConfigureKeys();
    void optionsConfigureToolbars();
    void optionsPreferences();
    void newToolbarConfig();

    void changeStatusbar(const QString& text);
    void changeCaption(const QString& text);

private:
    void setupAccel();
    void setupActions();

private:
    KrecipesView *m_view;

    KPrinter   *m_printer;
    KToggleAction *m_toolbarAction;
    KToggleAction *m_statusbarAction;

private:
	// Private variables
	KAction *saveAction;
private slots:
	void enableSaveOption(bool en=true);
};

#endif // _KRECIPES_H_
