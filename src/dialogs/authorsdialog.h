/*****************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                           *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>                 *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                    *
*   Copyright © 2009-2012 José Manuel Santamaría Lema <panfaust@gmail.com>   *
*                                                                            *
*   This program is free software; you can redistribute it and/or modify     *
*   it under the terms of the GNU General Public License as published by     *
*   the Free Software Foundation; either version 2 of the License, or        *
*   (at your option) any later version.                                      *
******************************************************************************/

#ifndef AUTHORSDIALOG_H
#define AUTHORSDIALOG_H

#include <qwidget.h>

#include "widgets/dblistviewbase.h"


class RecipeDB;
class KreAuthorListWidget;
class KreGenericActionsHandler;
class KreAuthorActionsHandler;
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

	KreGenericActionsHandler* getActionsHandler() const;
	void addAction( KAction * action );

private:
	// Internal data
	RecipeDB *database;
	//Widgets
	KreAuthorListWidget *authorListWidget;
	KreAuthorActionsHandler *authorActionsHandler;
	KPushButton *newAuthorButton;
	KPushButton *removeAuthorButton;
};
#endif
