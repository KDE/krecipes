/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2004 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2009 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef CATEGORYACTIONSHANDLER_H
#define CATEGORYACTIONSHANDLER_H

#include "actionshandlers/actionshandlerbase.h"

class StdCategoryListView;
class Q3ListViewItem;
class KAction;

class CategoryActionsHandler : public ActionsHandlerBase
{
	Q_OBJECT

public:
	CategoryActionsHandler( StdCategoryListView *_parentListView, RecipeDB *db );
	~CategoryActionsHandler();

	void setCategoryPasteAction( KAction * action );
	void setPasteAsSubcategoryAction( KAction * action );

public slots:
	void createNew();
	void cut();
	void paste();
	void pasteAsSub();
	void changeCategoryParent(Q3ListViewItem *item,
		Q3ListViewItem * /*afterFirst*/, Q3ListViewItem * /*afterNow*/ );
	void remove();

private:
	Q3ListViewItem *clipboard_item;
	Q3ListViewItem *clipboard_parent;
	KAction *pasteAction;
	KAction *pasteAsSubAction;
	bool checkBounds( const QString &name );	

private slots:
	void preparePopup();
	void saveElement( Q3ListViewItem* i );
};

#endif //CATEGORYACTIONSHANDLER_H

