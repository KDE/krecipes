/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <iostream>
#include <qlayout.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <kconfig.h>
#include <kapp.h>
#include <kstandarddirs.h>
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
setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
connect(finishButton(),SIGNAL(clicked()),this,SLOT(save()));
}


SetupWizard::~SetupWizard()
{

}

WelcomePage::WelcomePage(QWidget *parent):QWidget(parent)
{
QGridLayout *layout=new QGridLayout(this,1,1,0,0);
QSpacerItem *spacer_top=new QSpacerItem(5,5,QSizePolicy::Minimum, QSizePolicy::Fixed);
layout->addItem(spacer_top,0,1);
QSpacerItem *spacer_left=new QSpacerItem(5,5,QSizePolicy::Fixed, QSizePolicy::Minimum);
layout->addItem(spacer_left,1,0);
QPixmap logoPixmap (locate("data", "krecipes/pics/wizard.png"));
logo=new QLabel(this);
logo->setPixmap(logoPixmap);
logo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
layout->addWidget(logo,1,1);

welcomeText=new QLabel(this);
welcomeText->setText("Thanks very much for choosing Krecipes.\nIt looks like this is the first time you are using it. This wizard will help you with the initial setup so that you can start using it quickly.\n\nWelcome and enjoy cooking! ;-) ");
welcomeText->setMinimumWidth(200);
welcomeText->setMaximumWidth(10000);
welcomeText->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
welcomeText->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
layout->addWidget(welcomeText,1,2);

}


ServerSetupPage::ServerSetupPage(QWidget *parent):QWidget(parent)
{
QGridLayout *layout=new QGridLayout(this,1,1,0,0);
QSpacerItem *spacer_top=new QSpacerItem(10,10,QSizePolicy::Minimum, QSizePolicy::Fixed);
layout->addItem(spacer_top,0,1);
QSpacerItem *spacer_left=new QSpacerItem(10,10,QSizePolicy::Fixed, QSizePolicy::Minimum);
layout->addItem(spacer_left,1,0);


QPixmap networkSetupPixmap (locate("data", "krecipes/pics/network.png"));
logo=new QLabel(this);
logo->setPixmap(networkSetupPixmap);
logo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
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

QString ServerSetupPage::server(void)
{
return(this->serverEdit->text());
}

QString ServerSetupPage::user(void)
{
return(this->usernameEdit->text());
}

QString ServerSetupPage::password(void)
{
return(this->passwordEdit->text());
}

QString ServerSetupPage::dbName(void)
{
return(this->dbNameEdit->text());;
}

SavePage::SavePage(QWidget *parent):QWidget(parent)
{
QGridLayout *layout=new QGridLayout(this,1,1,0,0);
QSpacerItem *spacer_top=new QSpacerItem(10,10,QSizePolicy::Minimum, QSizePolicy::Fixed);
layout->addItem(spacer_top,0,1);
QSpacerItem *spacer_left=new QSpacerItem(10,10,QSizePolicy::Fixed, QSizePolicy::Minimum);
layout->addItem(spacer_left,1,0);

QPixmap logoPixmap (locate("data", "krecipes/pics/save.png"));
logo=new QLabel(this);
logo->setPixmap(logoPixmap);
logo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
layout->addWidget(logo,1,1);

saveText=new QLabel(this);
saveText->setText("Congratulations! The necessary configuration was already setup. Press 'Finish' to continue, and enjoy cooking!");
saveText->setMinimumWidth(200);
saveText->setMaximumWidth(10000);
saveText->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

saveText->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
layout->addWidget(saveText,1,2);


}

void SetupWizard::save(void)
{
KConfig *config=kapp->config();

// Save the server data
config->setGroup("Server");
config->writeEntry("Host",serverSetupPage->server());
config->writeEntry("Username",serverSetupPage->user());
config->writeEntry("Password",serverSetupPage->password());
config->writeEntry("DBName",serverSetupPage->dbName());

// Indicate that settings were already made

config->setGroup("Wizard");
config->writeEntry( "SystemSetup",true);

}


