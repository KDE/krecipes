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

#ifndef AUTHORSDIALOG_H
#define AUTHORSDIALOG_H

#include <qwidget.h>
#include <qpushbutton.h>
#include <qhbox.h>
#include <qlayout.h>
#include <kiconloader.h>
#include <klistview.h>

class RecipeDB;

/**
@author Unai Garro
*/

class AuthorsDialog:public QWidget{

Q_OBJECT

public:

	AuthorsDialog(QWidget* parent, RecipeDB *db);
	~AuthorsDialog();
	void reload(void);
private:
	// Internal data
	RecipeDB *database;
	//Widgets
	QGridLayout *layout;
	KListView *authorListView;
	QHBox *buttonBar;
	QPushButton *newAuthorButton;
	QPushButton *removeAuthorButton;
	KIconLoader *il;

private slots:
	void createNewAuthor(void);
	void removeAuthor(void);
  void modAuthor(QListViewItem*);
  void saveAuthor(QListViewItem*);

};
#endif
