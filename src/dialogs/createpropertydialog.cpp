/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "createpropertydialog.h"

CreatePropertyDialog::CreatePropertyDialog(ElementList* list):QDialog(0,0,true)
{

// Initialize Internal Variables
unitList=list; // Store the pointer to the unitList;

// Initialize widgets
container=new QVBoxLayout(this,5,5);
box=new QGroupBox(this);
container->addWidget(box);
box->setTitle("New Property");

propertyNameEdit=new KLineEdit(box); propertyNameEdit->setGeometry( QRect( 105, 30, 210, 25 ) );
propertyUnits=new KLineEdit(box); propertyUnits->setGeometry(QRect(105,60,80,25));
propertyPerUnits=new QComboBox(box); propertyPerUnits->setGeometry(QRect(235,60,80,25));
okButton=new QPushButton(box); okButton->setGeometry( QRect( 5, 120, 100, 20 ) ); okButton->setText("Ok");
cancelButton=new QPushButton(box); cancelButton->setGeometry( QRect( 110, 120, 60, 20 ) ); cancelButton->setText("Cancel");
nameEditText=new QLabel("Property Name:",box); nameEditText->setGeometry(QRect(5,30,100,20));
unitsText=new QLabel("Units:",box); unitsText->setGeometry(QRect(5,60,100,20));
perUnitsText=new QLabel("per",box); perUnitsText->setGeometry(QRect(195,60,30,20));

loadUnits();

resize(QSize(400,200));

// Signals & Slots
connect (okButton,SIGNAL(clicked()),this,SLOT(accept()));
connect (cancelButton,SIGNAL(clicked()),this,SLOT(reject()));
}


CreatePropertyDialog::~CreatePropertyDialog()
{
}


QString CreatePropertyDialog::newPropertyName(void)
{
return(propertyNameEdit->text());
}

QString CreatePropertyDialog::newUnitsName(void)
{
return(propertyUnits->text());
}

int CreatePropertyDialog::perUnits(void)
{
if (propertyPerUnits->count()>0) // Make sure that the unit list is not empty
  return(unitList->getElement(propertyPerUnits->currentItem())->id);
else return(-1);
}

void CreatePropertyDialog::loadUnits()
{
for ( Element *unit =unitList->getFirst(); unit; unit =unitList->getNext() )
{
  propertyPerUnits->insertItem(unit->name);
}
}
