/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "selectcategoriesdialog.h"

#include <klocale.h>
#include <kdebug.h>

#include "DBBackend/recipedb.h"

class CategoryListItem:public QCheckListItem{
public:
	CategoryListItem(QListView* qlv, const Element &el ):QCheckListItem(qlv,QString::null,QCheckListItem::CheckBox),elStored(el){}
private:
	const Element elStored;

public:
	virtual QString text(int column) const
		{
		if (column==1) return(elStored.name);
		else return(QString::null);
		}
};

SelectCategoriesDialog::SelectCategoriesDialog(QWidget *parent, const ElementList &categoryList,QPtrList <bool> *selected):QDialog(parent,0,true)
{

// Store pointer

categoryListPC=categoryList;
//Design UI

layout = new QGridLayout( this, 1, 1, 0, 0);

	// Border Spacers
	QSpacerItem* spacer_left = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );	layout->addItem( spacer_left, 1,0 );
	QSpacerItem* spacer_top = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem(spacer_top,0,1);

//Category List
categoryListView=new KListView(this);
categoryListView->addColumn("*");
categoryListView->addColumn(i18n("Category"));
categoryListView->setSorting(-1);
categoryListView->setAllColumnsShowFocus(true);
layout->addMultiCellWidget(categoryListView,1,1,1,3);

//Ok/Cancel buttons
QSpacerItem* buttonsSpacer=new QSpacerItem(10,10, QSizePolicy::Minimum, QSizePolicy::Fixed);
layout->addItem(buttonsSpacer,2,1);

okButton=new QPushButton(this);
okButton->setText(i18n("&OK"));
okButton->setFlat(true);
layout->addWidget(okButton,3,1);

QSpacerItem* spacerBetweenButtons=new QSpacerItem(10,10, QSizePolicy::Fixed, QSizePolicy::Minimum);
layout->addItem(spacerBetweenButtons,3,2);

cancelButton=new QPushButton(this);
cancelButton->setText(i18n("&Cancel"));
cancelButton->setFlat(true);
layout->addWidget(cancelButton,3,3);

// Load the list
loadCategories(categoryList,selected);

// Connect signals & Slots
connect (okButton,SIGNAL(clicked()),this,SLOT(accept()));
connect (cancelButton,SIGNAL(clicked()),this,SLOT(reject()));

}

SelectCategoriesDialog::~SelectCategoriesDialog()
{
}

void SelectCategoriesDialog::getSelectedCategories(ElementList *newSelected)
{
ElementList::const_iterator element_it = categoryListPC.begin(); // Initialize to first element

for (CategoryListItem *it=(CategoryListItem *) categoryListView->firstChild();it; it=(CategoryListItem *) it->nextSibling())
	{
	/*bool *newValue=new bool;*/
	if (it->isOn()) newSelected->append(*element_it); // If checked, add

	element_it++;
	}

}

void SelectCategoriesDialog::loadCategories(const ElementList &categoryList, QPtrList <bool> *selected)
{
bool *checked=selected->last();

ElementList::const_iterator cat_it = categoryList.end();
--cat_it;
for ( unsigned int i = 0; i < categoryList.count(); i++ )
{
	CategoryListItem *it=new CategoryListItem(categoryListView,*cat_it);
	if (*checked) it->setOn(true);
	checked=selected->prev();
	--cat_it;
}
}
