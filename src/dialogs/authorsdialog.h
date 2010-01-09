/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef AUTHORSDIALOG_H
#define AUTHORSDIALOG_H

#include <qwidget.h>

#include <kiconloader.h>
#include <k3listview.h>
#include <kvbox.h>

#include "widgets/dblistviewbase.h"

class RecipeDB;
class StdAuthorListView;
class ActionsHandlerBase;
class AuthorActionsHandler;
class KAction;
class KPushButton;

/**
@author Unai Garro
*/

class AuthorsDialog: public QWidget
{

	Q_OBJECT

public:

	AuthorsDialog( QWidget* parent, RecipeDB *db );
	~AuthorsDialog();
	void reload( ReloadFlags flag = Load );

	ActionsHandlerBase* getActionsHandler() const;
	void addAction( KAction * action );

private:
	// Internal data
	RecipeDB *database;
	//Widgets
	StdAuthorListView *authorListView;
	AuthorActionsHandler *authorActionsHandler;
	KPushButton *newAuthorButton;
	KPushButton *removeAuthorButton;
};
#endif
