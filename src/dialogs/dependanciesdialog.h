/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*                                                                         *
*   Copyright (C) 2003-2005 by                                            *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef DEPENDANCIESDIALOG_H
#define DEPENDANCIESDIALOG_H

#include <qdialog.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <klistview.h>

class ElementList;

/**
@author Unai Garro
*/
class DependanciesDialog: public QDialog
{
public:
	//Methods
	DependanciesDialog( QWidget *parent = 0, const ElementList* recipeList = 0, const ElementList* propertiesList = 0, bool deps_are_deleted = true );
	~DependanciesDialog();

public slots:
	void accept();

private:
	//Widgets

	QLabel *instructionsLabel;
	QGroupBox *recipeBox;
	QGroupBox *ingredientBox;
	QGroupBox *propertiesBox;

	QGroupBox *buttonBox;
	QPushButton* okButton;
	QPushButton* cancelButton;

	KListView *recipeListView;
	KListView *ingredientListView;
	KListView *propertiesListView;

	bool m_depsAreDeleted;

	// Methods
	void loadList( KListView* listView, const ElementList *list );


};

#endif
