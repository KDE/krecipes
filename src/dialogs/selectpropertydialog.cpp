/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "selectpropertydialog.h"

SelectPropertyDialog::SelectPropertyDialog(QWidget* parent,IngredientPropertyList *propertyList): QDialog(parent)
{
container=new QVBoxLayout(this,5,5);
box=new QGroupBox(this);
container->addWidget(box);
box->setTitle("Choose a Property");
propertyChooseView=new KListView(box);
propertyChooseView->addColumn("id.");
propertyChooseView->addColumn("Property");
propertyChooseView->setGeometry( QRect( 5, 30, 180, 250 ) );
okButton=new QPushButton(box);
okButton->setGeometry( QRect( 5, 300, 100, 20 ) );
okButton->setText("Ok");
okButton->setFlat(true);
cancelButton=new QPushButton(box);
cancelButton->setGeometry( QRect( 110, 300, 60, 20 ) );
cancelButton->setText("Cancel");
cancelButton->setFlat(true);
resize(QSize(200,350));
loadProperties(propertyList);
connect (okButton,SIGNAL(clicked()),this,SLOT(accept()));
connect (cancelButton,SIGNAL(clicked()),this,SLOT(reject()));
}


SelectPropertyDialog::~SelectPropertyDialog()
{
}


int SelectPropertyDialog::propertyID(void)
{

QListViewItem *it;
if (it=propertyChooseView->selectedItem())
{
return(it->text(0).toInt());
}
else return(-1);
}

void SelectPropertyDialog::loadProperties(IngredientPropertyList *propertyList)
{
for ( IngredientProperty *property =propertyList->getFirst(); property; property =propertyList->getNext() )
{
QListViewItem *it= new QListViewItem(propertyChooseView,QString::number(property->id),property->name);
}
}
