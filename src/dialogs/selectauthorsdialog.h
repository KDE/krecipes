/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef SELECTAUTHORSDIALOG_H
#define SELECTAUTHORSDIALOG_H

#include <qdialog.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <kiconloader.h>
#include <klistview.h>
#include <kcombobox.h>

class RecipeDB;
class ElementList;

/**
@author Unai Garro
*/
class SelectAuthorsDialog:public QDialog{

Q_OBJECT

public:

    SelectAuthorsDialog(QWidget *parent, ElementList *currentAuthors, RecipeDB *db);
    ~SelectAuthorsDialog();
    void getSelectedAuthors(ElementList *newAuthors);

private:

	//Widgets
	KComboBox *authorsCombo;
	KListView *authorListView;
	QGridLayout *layout;
	QPushButton *okButton;
	QPushButton *cancelButton;
	QPushButton *addAuthorButton;
	QPushButton *removeAuthorButton;
	KIconLoader *il;

	//Variables
	ElementList *authorList; //Copy of pointer to authorList
	RecipeDB *database;

	//Private methods
	void loadAuthors(ElementList *authorList);
	void createNewAuthorIfNecessary(void);
	void reloadAuthorsCombo(void);
private slots:
	void addAuthor(void);
	void removeAuthor(void);
};

#endif
