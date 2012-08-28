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

#include <QList>

//class StdCategoryListView;
//class Q3ListViewItem;
class KAction;
class KreCategoriesListWidget;
class RecipeDB;
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
	//void changeCategoryParent(Q3ListViewItem *item,
	//	Q3ListViewItem * /*afterFirst*/, Q3ListViewItem * /*afterNow*/ );
	void remove();

private slots:
	void preparePopup();
	void saveElement( const QModelIndex & topLeft,
		const QModelIndex & bottomRight);

private:
	bool checkBounds( const QString &name );	
	//Q3ListViewItem *clipboard_item;
	//Q3ListViewItem *clipboard_parent;
	QList<QStandardItem*> m_clipboardRow;
	int m_parentRow;
	KAction * m_pasteAction;
	KAction * m_pasteAsSubAction;

};

#endif //KRECATEGORYACTIONSHANDLER_H

