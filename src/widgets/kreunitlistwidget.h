/*************************************************************************** 
*   Copyright © 2014-2015 José Manuel Santamaría Lema <panfaust@gmail.com> *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#ifndef KREUNITLISTWIDGET_H
#define KREUNITLISTWIDGET_H

#include "kregenericlistwidget.h"

#include <QObject>

class Unit;


class KreUnitListWidget : public KreGenericListWidget
{
	Q_OBJECT

public:
	KreUnitListWidget( QWidget *parent, RecipeDB *db );

public slots:
	virtual void createUnit( const Unit & );
	virtual void removeUnit( int );

protected:
	virtual int elementCount();
	virtual void load(int limit, int offset);
	virtual void cancelLoad();
	virtual int idColumn();

};

#endif //KREUNITLISTWIDGET_H
