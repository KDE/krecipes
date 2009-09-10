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

#ifndef PREPMETHODSDIALOG_H
#define PREPMETHODSDIALOG_H

#include <qwidget.h>

#include <QGridLayout>
#include <k3listview.h>
#include <kvbox.h>

#include "widgets/dblistviewbase.h"

class RecipeDB;
class ActionsHandlerBase;
class KAction;
class PrepMethodActionsHandler;
class StdPrepMethodListView;
class KPushButton;

/**
@author Unai Garro
*/

class PrepMethodsDialog: public QWidget
{

	Q_OBJECT

public:

	PrepMethodsDialog( QWidget* parent, RecipeDB *db );
	~PrepMethodsDialog();
	void reload( ReloadFlags flag = Load );
	ActionsHandlerBase *getActionsHandler() const;
	void addAction( KAction * action );

private:
	// Internal data
	RecipeDB *database;
	//Widgets
	QGridLayout *layout;
	StdPrepMethodListView *prepMethodListView;
	PrepMethodActionsHandler *prepMethodActionsHandler;
	KHBox *buttonBar;
	KPushButton *newPrepMethodButton;
	KPushButton *removePrepMethodButton;
};
#endif
