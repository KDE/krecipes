/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "editbox.h"
#include <qlayout.h>

EditBox::EditBox(QWidget* parent):QWidget(parent)
{
QHBoxLayout *layout= new QHBoxLayout(this);
layout->setMargin(0);
editBox=new KDoubleNumInput(this);
editBox->setMinimumWidth(10);
layout->addWidget(editBox);
okButton=new QPushButton("ok",this);
okButton->setFixedWidth(okButton->fontMetrics().width("ok"));
okButton->setFlat(true);
layout->addWidget(okButton);
accepted=false;
connect(this->okButton,SIGNAL(clicked()),this,SLOT(acceptValue()));
}

void EditBox::acceptValue(void)
{
accepted=true;
emit valueChanged(editBox->value());
}



EditBox::~EditBox()
{
}

double EditBox::value(void)
{
return editBox->value();
}

void EditBox::setValue(double newValue)
{
editBox->setValue(newValue);
}


