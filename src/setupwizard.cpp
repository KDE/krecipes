/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <iostream>
#include <qlayout.h>
#include <qpixmap.h>
#include "setupwizard.h"

SetupWizard::SetupWizard(QWidget *parent, const char *name, bool modal, WFlags f):KWizard(parent,name, modal,f)
{
welcomePage= new WelcomePage(this);
addPage(welcomePage,"Welcome to Krecipes");


}


SetupWizard::~SetupWizard()
{

}

WelcomePage::WelcomePage(QWidget *parent):QWidget(parent)
{
QGridLayout *layout=new QGridLayout(this,1,1,0,0);

QPixmap logoPixmap ("../pics/wizard.png");
logo=new QLabel(this);
logo->setPixmap(logoPixmap);
layout->addWidget(logo,1,1);

welcomeText=new QLabel(this);
welcomeText->setText("Thanks very much for choosing Krecipes.\n It looks like this is the first time you are using it. This wizard will help you with the initial setup so that you can start using it quickly. \n\nWelcome and enjoy cooking! ;-) ");
layout->addWidget(welcomeText,1,2);

}