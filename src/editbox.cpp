/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "editbox.h"
#include <qlayout.h>
#include <iostream>
#include <klocale.h>
EditBox::EditBox(QWidget* parent):QWidget(parent)
{
QHBoxLayout *layout= new QHBoxLayout(this);
layout->setMargin(0);
editBox=new RatioInput(this);
editBox->setMinimumWidth(10);
layout->addWidget(editBox);
okButton=new QPushButton(i18n("&OK"),this);
okButton->setFixedWidth(okButton->fontMetrics().width(i18n("&OK")));
okButton->setFlat(true);
layout->addWidget(okButton);
accepted=false;
connect(this->okButton,SIGNAL(clicked()),this,SLOT(acceptValue()));
connect(this->editBox,SIGNAL(valueAccepted()),this,SLOT(acceptValue()));
}

void EditBox::acceptValue(void)
{
accepted=true;
emit valueChanged(editBox->value());
}

EditBox::~EditBox()
{
}

double EditBox::value(void) const
{
return editBox->value();
}

void EditBox::setValue(double newValue)
{
editBox->setValue(newValue);
}

void EditBox::setRange(double min,double max,double step,bool slider)
{
editBox->setRange(min,max,step,slider);
}

void EditBox::setPrecision(int dec)
{
editBox->setPrecision(dec);
}

void RatioInput::keyPressEvent ( QKeyEvent * e )
{
	if (e->key()==Key_Enter || e->key()==Key_Return)
	{
	emit valueAccepted();
	}
	else
	{
	e->ignore();
	}
}

RatioInput::RatioInput(QWidget *parent):KDoubleNumInput(parent)
{
}

#include "editbox.moc"
