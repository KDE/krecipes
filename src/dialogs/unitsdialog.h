/***************************************************************************
 *   Copyright (C) 2003-2004 by                                            *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
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
#include <qpushbutton.h>

#include "unitratiolist.h"

class RecipeDB;
class ConversionTable;
class StdUnitListView;

/**
@author Unai Garro
*/
class UnitsDialog:public QWidget{
Q_OBJECT
public:
    UnitsDialog(QWidget *parent, RecipeDB *db);
    ~UnitsDialog();
    virtual QSize sizeHint () const{return QSize(300,200);}

public slots:
    void reload (void);

private:
	// Widgets
	StdUnitListView *unitListView;
	ConversionTable *conversionTable;
	QPushButton *newUnitButton;
	QPushButton *removeUnitButton;

	// Internal methods
	void loadUnitsList(void);
	void reloadData(void);
	void saveAllRatios( UnitRatioList &ratioList );
	bool checkBounds( const QString &name );

	// Internal Variables
	RecipeDB *database;
private slots:
	void loadConversionTable(void);
	void saveRatio(int r, int c, double value);
};

#endif
