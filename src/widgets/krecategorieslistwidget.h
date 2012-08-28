/***************************************************************************
*   Copyright © 2012 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef KRECATEGORIESLISTWIDGET_H
#define KRECATEGORIESLISTWIDGET_H


#include <QWidget>

#include "kregenericlistwidget.h"

class Element;
class QStandardItem;


class KreCategoriesListWidget : public KreGenericListWidget
{
	Q_OBJECT

public:
	KreCategoriesListWidget( QWidget *parent, RecipeDB *db );

protected slots:
	virtual void createCategory( const Element & category, int parent_id );
	virtual void removeCategory( int id );

protected:
	virtual int elementCount();
	virtual void load(int limit, int offset);
	virtual void cancelLoad(){}

	virtual int idColumn();

	void populate ( QStandardItem * item, int id );
};


#endif //KRECATEGORIESLISTWIDGET_H
