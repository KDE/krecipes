/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "selectunitdialog.h"

SelectUnitDialog::SelectUnitDialog(QWidget* parent,ElementList *unitList)
: QDialog(parent)
{
container=new QVBoxLayout(this,5,5);
box=new QGroupBox(this);
container->addWidget(box);
box->setTitle("Choose a Unit");
unitChooseView=new KListView(box);
unitChooseView->addColumn("id.");
unitChooseView->addColumn("Unit");
unitChooseView->setGeometry( QRect( 5, 30, 180, 250 ) );
okButton=new QPushButton(box);
okButton->setGeometry( QRect( 5, 300, 100, 20 ) );
okButton->setText("Ok");
okButton->setFlat(true);
cancelButton=new QPushButton(box);
cancelButton->setGeometry( QRect( 110, 300, 60, 20 ) );
cancelButton->setText("Cancel");
cancelButton->setFlat(true);
resize(QSize(200,350));
loadUnits(unitList);
connect (okButton,SIGNAL(clicked()),this,SLOT(accept()));
connect (cancelButton,SIGNAL(clicked()),this,SLOT(reject()));
}


SelectUnitDialog::~SelectUnitDialog()
{
}

int SelectUnitDialog::unitID(void)
{

QListViewItem *it;
if (it=unitChooseView->selectedItem())
{
return(it->text(0).toInt());
}
else return(-1);
}

void SelectUnitDialog::loadUnits(ElementList *unitList)
{
for ( Element *unit =unitList->getFirst(); unit; unit =unitList->getNext() )
{
QListViewItem *it= new QListViewItem(unitChooseView,QString::number(unit->id),unit->name);
}
}

