/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef UNITSDIALOG_H
#define UNITSDIALOG_H
#include <qwidget.h>
#include <klistview.h>
#include "recipedb.h"
#include "editbox.h"
#include "conversiontable.h"

/**
@author Unai Garro
*/
class UnitsDialog:public QWidget{
Q_OBJECT
public:
    UnitsDialog(QWidget *parent, RecipeDB *db);
    ~UnitsDialog();
private:
	// Widgets
	KListView *unitListView;
	ConversionTable *conversionTable;
	QPushButton *newUnitButton;
	QPushButton *removeUnitButton;
	// Internal methods
	void loadConversionTable(void);
	void loadUnitsList(void);
	void reloadData(void);
	// Internal Variables
	RecipeDB *database;
private slots:
	void createNewUnit(void);
	void removeUnit(void);
	void saveRatio(int r, int c, double value);

};

#endif
