/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PREPMETHODSDIALOG_H
#define PREPMETHODSDIALOG_H

#include <qwidget.h>
#include <qpushbutton.h>
#include <qhbox.h>
#include <qlayout.h>
#include <kiconloader.h>
#include <klistview.h>

class RecipeDB;
class StdPrepMethodListView;

/**
@prepMethod Unai Garro
*/

class PrepMethodsDialog:public QWidget{

Q_OBJECT

public:

	PrepMethodsDialog(QWidget* parent, RecipeDB *db);
	~PrepMethodsDialog();
	void reload(void);
private:
	// Internal data
	RecipeDB *database;
	//Widgets
	QGridLayout *layout;
	StdPrepMethodListView *prepMethodListView;
	QHBox *buttonBar;
	QPushButton *newPrepMethodButton;
	QPushButton *removePrepMethodButton;
	KIconLoader *il;
};
#endif
