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
serverSetupPage = new ServerSetupPage(this);
addPage(serverSetupPage,"Server Settings");
savePage = new SavePage(this);
addPage(savePage,"Finish and Save Settings");
setFinishEnabled(savePage,true); // Enable finish button
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

welcomeText=new KTextEdit(this);
welcomeText->setText("Thanks very much for choosing Krecipes.\n It looks like this is the first time you are using it. This wizard will help you with the initial setup so that you can start using it quickly. \n\nWelcome and enjoy cooking! ;-) ");
layout->addWidget(welcomeText,1,2);

}


ServerSetupPage::ServerSetupPage(QWidget *parent):QWidget(parent)
{
QGridLayout *layout=new QGridLayout(this,1,1,0,0);

QPixmap networkSetupPixmap ("../pics/network.png");
logo=new QLabel(this);
logo->setPixmap(networkSetupPixmap);
layout->addMultiCellWidget(logo,1,8,1,1);

QLabel* serverText=new QLabel("Server:", this);
serverText->setFixedSize(QSize(100,20));
serverText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
layout->addWidget(serverText,1,2);

serverEdit=new KLineEdit(this);
serverEdit->setFixedSize(QSize(120,20));
serverEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
layout->addWidget(serverEdit,1,3);

QSpacerItem* spacerRow1 = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
layout->addItem( spacerRow1,2,2 );

QLabel* usernameText=new QLabel("Username:", this);
usernameText->setFixedSize(QSize(100,20));
usernameText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
layout->addWidget(usernameText,3,2);

usernameEdit=new KLineEdit(this);
usernameEdit->setFixedSize(QSize(120,20));
usernameEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
layout->addWidget(usernameEdit,3,3);

QSpacerItem* spacerRow2 = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
layout->addItem( spacerRow2,4,2 );

QLabel* passwordText=new QLabel("Password:", this);
passwordText->setFixedSize(QSize(100,20));
passwordText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
layout->addWidget(passwordText,5,2);

passwordEdit=new KLineEdit(this);
passwordEdit->setFixedSize(QSize(120,20));
passwordEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
layout->addWidget(passwordEdit,5,3);

QSpacerItem* spacerRow3 = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
layout->addItem( spacerRow3, 6,2 );

QLabel* dbNameText=new QLabel("Database Name:", this);
dbNameText->setFixedSize(QSize(100,20));
dbNameText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
layout->addWidget(dbNameText,7,2);

dbNameEdit=new KLineEdit(this);
dbNameEdit->setFixedSize(QSize(120,20));
dbNameEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
layout->addWidget(dbNameEdit,7,3);

QSpacerItem* spacerRow4 = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
layout->addItem( spacerRow4, 8,2 );
QSpacerItem* spacerRight = new QSpacerItem( 10,10, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
layout->addItem( spacerRight, 1,5 );
}

SavePage::SavePage(QWidget *parent):QWidget(parent)
{
QGridLayout *layout=new QGridLayout(this,1,1,0,0);

QPixmap logoPixmap ("../pics/save.png");
logo=new QLabel(this);
logo->setPixmap(logoPixmap);
layout->addWidget(logo,1,1);

saveText=new KTextEdit(this);
saveText->setText("Congratulations! The necessary configuration was already setup. Press 'Finish' to continue, and enjoy cooking!");
layout->addWidget(saveText,1,2);


}

