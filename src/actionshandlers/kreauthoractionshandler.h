/*****************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                    *
*   Copyright © 2009-2012 José Manuel Santamaría Lema <panfaust@gmail.com>   *
*                                                                            *
*   This program is free software; you can redistribute it and/or modify     *
*   it under the terms of the GNU General Public License as published by     *
*   the Free Software Foundation; either version 2 of the License, or        *
*   (at your option) any later version.                                      *
******************************************************************************/

#ifndef KREAUTHORACTIONSHANDLER_H
#define KREAUTHORACTIONSHANDLER_H

#include "actionshandlers/kregenericactionshandler.h"


class KreAuthorActionsHandler : public KreGenericActionsHandler
{
	Q_OBJECT

public:
	KreAuthorActionsHandler( KreGenericListWidget * listWidget, RecipeDB * db );
	~KreAuthorActionsHandler() {}

public slots:
	void createNew();
	void remove();

private:
	bool checkBounds( const QString &name );

private slots:
	void saveElement( const QModelIndex & topLeft, 
		const QModelIndex & bottomRight);
};

#endif //KREAUTHORACTIONSHANDLER_H

