/*****************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                    *
*   Copyright © 2004 Unai Garro <ugarro@gmail.com>                           *
*   Copyright © 2009-2012 José Manuel Santamaría Lema <panfaust@gmail.com>   *
*                                                                            *
*   This program is free software; you can redistribute it and/or modify     *
*   it under the terms of the GNU General Public License as published by     *
*   the Free Software Foundation; either version 2 of the License, or        *
*   (at your option) any later version.                                      *
******************************************************************************/

#ifndef KRECATEGORYACTIONSHANDLER_H
#define KRECATEGORYACTIONSHANDLER_H

#include "actionshandlers/kregenericactionshandler.h"

#include "recipedb.h"
#include <QList>

class KAction;
class KreCategoriesListWidget;
class QStandardItem;


class KreCategoryActionsHandler : public KreGenericActionsHandler
{
	Q_OBJECT

public:
	KreCategoryActionsHandler( KreCategoriesListWidget * listWidget, RecipeDB *db );
	~KreCategoryActionsHandler();

	void setCategoryPasteAction( KAction * action );
	void setPasteAsSubcategoryAction( KAction * action );

public slots:
	void createNew();
	void cut();
	void paste();
	void pasteAsSub();
	void remove();

private slots:
	void preparePopup();
	void saveElement( const QModelIndex & topLeft,
		const QModelIndex & bottomRight);

private:
	bool checkBounds( const QString &name );
	RecipeDB::IdType m_clipboard_id;
	KAction * m_pasteAction;
	KAction * m_pasteAsSubAction;

};

#endif //KRECATEGORYACTIONSHANDLER_H

