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

KreListView::KreListView(QWidget *parent,const QString &title,bool filter, int filterCol):QVBox(parent)
{

	filteredColumn=filterCol;
	QWidget *header=this;
	if (filter) header=new QHBox(this);	
	
	if (title!=QString::null)
	{
		listLabel=new QLabel(header);
		listLabel->setText(title);
	}
	
	if (filter) {
			filterBox=new QHBox(header); ((QHBox*)header)->setSpacing(30);
			filterBox->setFrameShape(QFrame::Box); filterBox->setMargin(2);
			filterLabel=new QLabel(filterBox); filterLabel->setText(i18n("Search:"));
			filterEdit=new KLineEdit(filterBox);
			}


	list=new KListView(this);
	list->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
	setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
	setSpacing(10);
	
	//Connect Signals & Slots
	if (filter) connect(filterEdit,SIGNAL(textChanged(const QString&)),this,SLOT(filter(const QString&)));
}

KreListView::~KreListView()
{
}

void KreListView::filter(const QString& s)
{
for (QListViewItem *it=list->firstChild();it;it=it->nextSibling())
	{
		if ( s.isNull() || s == "" ) // Don't filter if the filter text is empty
		{
			it->setVisible(true);
		}
		else // It's a category. Check the children
		{
			
			if (it->text(filteredColumn).contains(s,false)) it->setVisible(true);
			else it->setVisible(false);
			
		}


	}
}