/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "krelistview.h"
#include <klocale.h>

KreListView::KreListView(QWidget *parent,const QString &title):QVBox(parent)
{
if (title!=QString::null)
	{
	label=new QLabel(this);
	label->setText(title);
	}
list=new KListView(this);
list->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
setSpacing(10);
}

KreListView::~KreListView()
{
}