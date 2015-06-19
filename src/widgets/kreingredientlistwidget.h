/***************************************************************************
*   Copyright © 2015 José Manuel Santamaría Lema <panfaust@gmail.com>      *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#ifndef KREINGREDIENTLISTWIDGET_H
#define KREINGREDIENTLISTWIDGET_H

#include "kregenericlistwidget.h"

class RecipeDB;
class Element;


class KreIngredientListWidget : public KreGenericListWidget
{
	Q_OBJECT

public:
	KreIngredientListWidget( QWidget * parent, RecipeDB *db );

protected:
	virtual int elementCount();
	virtual void load(int limit, int offset);
	virtual void cancelLoad();

 	//This function must return the column number where the id is stored.
	virtual int idColumn();

protected slots:
	virtual void createIngredient( const Element & );
	virtual void removeIngredient( int );

};


#endif //KREINGREDIENTLISTWIDGET_H
