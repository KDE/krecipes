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

#ifndef _KRECIPES_H_
#define _KRECIPES_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kapplication.h>
#include <kmainwindow.h>

#include "krecipesview.h" //for KrePanel

class KrecipesView;

class KPrinter;
class KToggleAction;
class KURL;
class KDialog;

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

    virtual bool queryClose();


private slots:
    void fileNew();
    void fileOpen();
    void fileSave();
    void fileExport();
    void filePrint();
    void optionsShowToolbar();
    void optionsShowStatusbar();
    void optionsConfigureKeys();
    void optionsConfigureToolbars();
    void optionsPreferences();
    void newToolbarConfig();
    void import();
    void kreDBImport();
    void pageSetupSlot();
    
    /**  This function is called whenever a panel is shown or hidden and then sets
      *  actions as enabled as appropriate.
      */
    void updateActions( KrePanel panel, bool show );

    void changeStatusbar(const QString& text);
    void changeCaption(const QString& text);

private:
    // Private methods
    void setupAccel();
    void setupActions();

private:
    KrecipesView *m_view;

    KPrinter   *m_printer;
    KToggleAction *m_toolbarAction;
    KToggleAction *m_statusbarAction;

private:
	// Internal variables
	KAction *saveAction;
	KAction *exportAction;
	KAction *editAction;
	KAction *printAction;
	KAction *reloadAction;

	KDialog *parsing_file_dlg;

	QValueList<KAction*> recipe_actions;

private slots:
	void enableSaveOption(bool en=true);
};

#endif // _KRECIPES_H_
