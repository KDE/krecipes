/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "createpropertydialog.h"
#include "elementlist.h"
#include <klocale.h>
CreatePropertyDialog::CreatePropertyDialog(ElementList* list):QDialog(0,0,true)
{

// Initialize Internal Variables
unitList=list; // Store the pointer to the unitList;

// Initialize widgets
container=new QVBoxLayout(this,5,5);
box=new QGroupBox(this);
container->addWidget(box);
box->setTitle(i18n("New Property"));

propertyNameEdit=new KLineEdit(box); propertyNameEdit->setGeometry( QRect( 105, 30, 210, 25 ) );
propertyUnits=new KLineEdit(box); propertyUnits->setGeometry(QRect(105,60,80,25));
okButton=new QPushButton(box); okButton->setGeometry( QRect( 5, 120, 100, 20 ) ); okButton->setText(i18n("Ok"));
cancelButton=new QPushButton(box); cancelButton->setGeometry( QRect( 110, 120, 60, 20 ) ); cancelButton->setText(i18n("Cancel"));
nameEditText=new QLabel(i18n("Property Name:"),box); nameEditText->setGeometry(QRect(5,30,100,20));
unitsText=new QLabel(i18n("Units:"),box); unitsText->setGeometry(QRect(5,60,100,20));


resize(QSize(400,200));
setFixedSize(size());

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


