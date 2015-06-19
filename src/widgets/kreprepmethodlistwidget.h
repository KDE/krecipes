/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef KREPREPMETHODLISTWIDGET_H
#define KREPREPMETHODLISTEIDGET_H


#include "widgets/kregenericlistwidget.h"


class KrePrepMethodListWidget : public KreGenericListWidget
{
	Q_OBJECT

public:
	KrePrepMethodListWidget( QWidget *parent, RecipeDB *db );

protected:
	virtual int elementCount();
	virtual void load(int limit, int offset);
	virtual void cancelLoad();

 	//This function must return the column number where the id is stored.
	virtual int idColumn();

protected slots:
	void createPrepMethod( const Element & );
	void removePrepMethod( int );

};


#endif //KREPREPMETHODLISTWIDGET_H
