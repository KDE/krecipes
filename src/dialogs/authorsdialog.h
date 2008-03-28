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

#ifndef AUTHORSDIALOG_H
#define AUTHORSDIALOG_H

#include <qwidget.h>
#include <qpushbutton.h>

#include <qlayout.h>
#include <kiconloader.h>
#include <k3listview.h>
#include <kvbox.h>

#include "widgets/dblistviewbase.h"

class RecipeDB;
class StdAuthorListView;

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
private:
	// Internal data
	RecipeDB *database;
	//Widgets
	StdAuthorListView *authorListView;
	QPushButton *newAuthorButton;
	QPushButton *removeAuthorButton;
};
#endif
