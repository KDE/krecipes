/***************************************************************************
*   Copyright (C) 2003-2004 by                                            *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
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

#include "datablocks/unitratiolist.h"
#include "datablocks/unit.h"
#include "widgets/dblistviewbase.h"

class RecipeDB;
class ConversionTable;
class StdUnitListView;

/**
@author Unai Garro
*/
class UnitsDialog: public QWidget
{
	Q_OBJECT
public:
	UnitsDialog( QWidget *parent, RecipeDB *db );
	~UnitsDialog();
	virtual QSize sizeHint () const
	{
		return QSize( 300, 200 );
	}

public slots:
	void reload( ReloadFlags flag = Load );

private:
	// Widgets
	StdUnitListView *unitListView;
	ConversionTable *massConversionTable;
	ConversionTable *volumeConversionTable;
	QPushButton *newUnitButton;
	QPushButton *removeUnitButton;

	// Internal methods
	void saveAllRatios( UnitRatioList &ratioList );
	bool checkBounds( const QString &name );

	// Internal Variables
	RecipeDB *database;
private slots:
	void loadConversionTables();
	void loadConversionTable( ConversionTable*, Unit::Type );
	void saveRatio( int r, int c, double value );
	void removeRatio( int r, int c );
};

#endif
