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
CreatePropertyDialog::CreatePropertyDialog(QWidget *parent,ElementList* list):QDialog(parent,0,true)
{

// Initialize Internal Variables
unitList=list; // Store the pointer to the unitList;

// Initialize widgets
container=new QVBoxLayout(this,5,5);

box=new QGroupBox(this);
box->setColumnLayout(0, Qt::Vertical );
box->layout()->setSpacing( 6 );
box->layout()->setMargin( 11 );
QVBoxLayout *boxLayout = new QVBoxLayout( box->layout() );
boxLayout->setAlignment( Qt::AlignTop );
box->setTitle(i18n("New Property"));

QGridLayout *gridLayout = new QGridLayout( this, 2, 2, 5, 5);

nameEditText=new QLabel(i18n("Property name:"),this);
propertyNameEdit=new KLineEdit(this);
propertyNameEdit->setMinimumWidth(150);
gridLayout->addWidget(nameEditText, 0, 0);
gridLayout->addWidget(propertyNameEdit, 0, 1);

unitsText=new QLabel(i18n("Units:"),this);
propertyUnits=new KLineEdit(this);
propertyUnits->setMinimumWidth(150);
gridLayout->addWidget(unitsText, 1, 0);
gridLayout->addWidget(propertyUnits, 1, 1);

QHBoxLayout *buttonsHBox = new QHBoxLayout( this, 5, 5 );
okButton=new QPushButton(i18n("&OK"),this);
cancelButton=new QPushButton(i18n("&Cancel"),this);
QSpacerItem* spacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
buttonsHBox->addWidget(okButton);
buttonsHBox->addWidget(cancelButton);
buttonsHBox->addItem( spacer );

boxLayout->addLayout( gridLayout );
boxLayout->addLayout( buttonsHBox );

container->addWidget(box);

adjustSize();
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


