/***************************************************************************
 *   Copyright (C) 2004 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef RECIPEFILTER_H
#define RECIPEFILTER_H
 
#include <qobject.h>

class KListView;
class QListViewItem;

class RecipeFilter : public QObject
{
Q_OBJECT

public:
	RecipeFilter( KListView * );

public slots:
	void filter( const QString & );
	void filterCategory( int categoryID );

private:
	bool isParentOf(QListViewItem *parent, QListViewItem *to_check);
	bool hideIfEmpty(QListViewItem *parent=0);

	KListView *listview;
	QListViewItem *currentCategory;
};

#endif //RECIPEFILTER_H
