/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef KRELISTVIEW_H
#define KRELISTVIEW_H

#include <qlabel.h>
#include <qvbox.h>
#include <klistview.h>



/**
@author Unai Garro
*/

class KreListView:public QVBox{
Q_OBJECT
public:

    KreListView(QWidget *parent,const QString &title=QString::null);
    ~KreListView();
    KListView *listView(){return list;}

private:
	QLabel *label;
	KListView *list;

};

#endif
