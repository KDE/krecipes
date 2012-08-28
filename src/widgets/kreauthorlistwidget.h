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
class ElementList;
class RecipeDB;
class LoadingAuthorsThread;


class KreAuthorListWidget : public KreGenericListWidget
{
	Q_OBJECT

public:
	KreAuthorListWidget( QWidget *parent, RecipeDB *db );
	~KreAuthorListWidget();

protected slots:
	virtual void createAuthor( const Element & );
	virtual void removeAuthor( int );

protected:
	virtual int elementCount();
	virtual void load(int limit, int offset);
	virtual void cancelLoad();
	virtual int idColumn();

private slots:
	void queryFinished( const ElementList & authorList, int authorsLoaded );

private:
	LoadingAuthorsThread * m_thread;
};


#endif //KREAUTHORLISTWIDGET_H
