/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2004 Unai Garro <ugarro@gmail.com>                        *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef RECIPEFILTER_H
#define RECIPEFILTER_H

#include <QObject>

class K3ListView;
class Q3ListViewItem;

class RecipeFilter : public QObject
{
	Q_OBJECT

public:
	RecipeFilter( K3ListView * );

public slots:
	void filter( const QString & );
	void filterCategory( int categoryID );

private:
	bool isParentOf( Q3ListViewItem *parent, Q3ListViewItem *to_check );
	bool hideIfEmpty( Q3ListViewItem *parent = 0 );

	K3ListView *listview;
	Q3ListViewItem *currentCategory;
};

#endif //RECIPEFILTER_H
