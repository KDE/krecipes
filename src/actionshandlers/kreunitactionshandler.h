/****************************************************************************
*   Copyright © 2014-2015 José Manuel Santamaría Lema <panfaust@gmail.com>  *
*                                                                           *
*   This program is free software; you can redistribute it and/or modify    *
*   it under the terms of the GNU General Public License as published by    *
*   the Free Software Foundation; either version 2 of the License, or       *
*   (at your option) any later version.                                     *
****************************************************************************/

#ifndef KREUNITACTIONSHANDLER_H
#define KREUNITACTIONSHANDLER_H

#include "actionshandlers/kregenericactionshandler.h"

class KreUnitListWidget;
class Unit;
class RecipeDB;

class KreUnitActionsHandler : public KreGenericActionsHandler
{
	Q_OBJECT

public:
	KreUnitActionsHandler( KreUnitListWidget * listWidget, RecipeDB *db );
	~KreUnitActionsHandler() {}

public slots:
	void createNew();
	void rename();
	void remove();

private:
	bool checkBounds( const Unit &unit );
	Unit getSelectedUnit();
	void tryToSaveUnit(Unit unit);

private slots:
	void saveElement( const QModelIndex & topLeft, 
		const QModelIndex & bottomRight);

};

#endif //KREUNITACTIONSHANDLER_H

