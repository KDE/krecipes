/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                         *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>               *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                  *
*   Copyright © 2015 José Manuel Santamaría Lema <panfaust@gmail.com>      *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#ifndef PREPMETHODSDIALOG_H
#define PREPMETHODSDIALOG_H

#include <qwidget.h>

#include <QGridLayout>
#include <kvbox.h>

#include "widgets/kregenericlistwidget.h"

class RecipeDB;
class KreGenericActionsHandler;
class KrePrepMethodActionsHandler;
class KAction;
class PrepMethodActionsHandler;
class KrePrepMethodListWidget;
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
	KreGenericActionsHandler * getActionsHandler() const;
	void addAction( KAction * action );

private:
	// Internal data
	RecipeDB *database;
	//Widgets
	QGridLayout *layout;
	KrePrepMethodListWidget * prepMethodListWidget;
	KrePrepMethodActionsHandler *prepMethodActionsHandler;
	KHBox *buttonBar;
	KPushButton *newPrepMethodButton;
	KPushButton *removePrepMethodButton;
};
#endif
