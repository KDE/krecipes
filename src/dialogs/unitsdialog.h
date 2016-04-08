/***************************************************************************
*   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2004 Cyril Bosselut <bosselut@b1project.com>         *
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef UNITSDIALOG_H
#define UNITSDIALOG_H

#include <QWidget>

#include "datablocks/unitratiolist.h"
#include "datablocks/unit.h"
#include "widgets/dblistviewbase.h"

class RecipeDB;
class KreConversionTable;
class KreUnitListWidget;
class KreUnitActionsHandler;
class KAction;
class KPushButton;
class KTabWidget;

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
	KreUnitActionsHandler* getActionsHandler() const;
	void addAction( KAction * action );

public slots:
	void reload( ReloadFlags flag = Load );

private:
	// Widgets
	KreUnitListWidget *unitListWidget;
	KreUnitActionsHandler *unitActionsHandler;
	KreConversionTable *massConversionTable;
	KreConversionTable *volumeConversionTable;
	KPushButton *newUnitButton;
	KPushButton *removeUnitButton;
	QWidget *unitTab;

	// Internal methods
	void saveAllRatios( UnitRatioList &ratioList );
	bool checkBounds( const QString &name );

	// Internal Variables
	RecipeDB *database;
	KTabWidget *tabWidget;

private slots:
	void loadConversionTables();
	void loadConversionTable( KreConversionTable*, Unit::Type );
	void saveRatio( int r, int c, double value );
	void removeRatio( int r, int c );
};

#endif
