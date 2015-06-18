/***************************************************************************
*   Copyright © 2015 José Manuel Santamaría Lema <panfaust@gmail.com>      *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#ifndef KREPROPERTYLISTWIDGET_H
#define KREPROPERTYLISTWIDGET_H

#include <QObject>

#include "widgets/kregenericlistwidget.h"

#include "datablocks/ingredientproperty.h"


class KrePropertyListWidget : public KreGenericListWidget
{
	Q_OBJECT

public:
	KrePropertyListWidget( QWidget * parent, RecipeDB * db );

protected:
	virtual int elementCount();
	virtual void load(int limit, int offset);
	virtual void cancelLoad();

 	//This function must return the column number where the id is stored.
	virtual int idColumn();

protected slots:
	virtual void createProperty( const IngredientProperty &property );
	virtual void removeProperty( int id );
};


#endif //KREPROPERTYLISTWIDGET_H
