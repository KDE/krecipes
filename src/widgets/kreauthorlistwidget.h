/***************************************************************************
*   Copyright © 2012 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef KREAUTHORLISTWIDGET_H
#define KREAUTHORLISTWIDGET_H


#include <QWidget>

#include "kregenericlistwidget.h"

class Element;
class RecipeDB;


class KreAuthorListWidget : public KreGenericListWidget
{
	Q_OBJECT

public:
	KreAuthorListWidget( QWidget *parent, RecipeDB *db );

protected slots:
	virtual void createAuthor( const Element & );
	virtual void removeAuthor( int );

};


#endif //KREAUTHORLISTWIDGET_H
