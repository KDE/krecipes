/***************************************************************************
*   Copyright © 2015 José Manuel Santamaría Lema <panfaust@gmail.com>      *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
***************************************************************************/

#ifndef KREPREPMETHODACTIONSHANDLER_H
#define KREPREPMETHODACTIONSHANDLER_H

#include "actionshandlers/kregenericactionshandler.h"

class KreGenericListWidget;


class KrePrepMethodActionsHandler : public KreGenericActionsHandler
{
	Q_OBJECT

public:
	KrePrepMethodActionsHandler( KreGenericListWidget * listWidget, RecipeDB *db );
	~KrePrepMethodActionsHandler() {}

public slots:
	void createNew();
	void remove();

protected slots:
	void saveElement( const QModelIndex& topLeft, 
		const QModelIndex& topBottom );
private:
	bool checkBounds( const QString &name );

};

#endif //KREPREPMETHODACTIONSHANDLER_H

