/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <unistd.h>
#include <pwd.h>

#include <iostream>
#include <qhbox.h>
#include <qvbox.h>
#include <qvgroupbox.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <kconfig.h>
#include <kapp.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include "setupwizard.h"

SetupWizard::SetupWizard(QWidget *parent, const char *name, bool modal, WFlags f):KWizard(parent,name, modal,f)
{
welcomePage= new WelcomePage(this);
addPage(welcomePage,i18n("Welcome to Krecipes"));
permissionsSetupPage=new PermissionsSetupPage(this);
addPage(permissionsSetupPage,i18n("Database Permissions"));
serverSetupPage = new ServerSetupPage(this);
addPage(serverSetupPage,i18n("Server Settings"));
savePage = new SavePage(this);
addPage(savePage,i18n("Finish and Save Settings"));
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
QSpacerItem *spacer_top=new QSpacerItem(10,10,QSizePolicy::Minimum, QSizePolicy::Fixed);
layout->addItem(spacer_top,0,1);
QSpacerItem *spacer_left=new QSpacerItem(10,10,QSizePolicy::Fixed, QSizePolicy::Minimum);
layout->addItem(spacer_left,1,0);
QPixmap logoPixmap (locate("data", "krecipes/pics/wizard.png"));
logo=new QLabel(this);
logo->setPixmap(logoPixmap);
logo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
layout->addWidget(logo,1,1);

QSpacerItem *spacer_from_image=new QSpacerItem(10,10,QSizePolicy::Fixed, QSizePolicy::Minimum);
layout->addItem(spacer_from_image,1,2);

welcomeText=new QLabel(this);
welcomeText->setText(i18n("Thank you very much for choosing Krecipes.\nIt looks like this is the first time you are using it. This wizard will help you with the initial setup so that you can start using it quickly.\n\nWelcome and enjoy cooking! ;-) "));
welcomeText->setMinimumWidth(200);
welcomeText->setMaximumWidth(10000);
welcomeText->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Minimum);
welcomeText->setAlignment( int( QLabel::WordBreak | QLabel::AlignTop |QLabel::AlignJustify  ) );
layout->addWidget(welcomeText,1,3);

}

PermissionsSetupPage::PermissionsSetupPage(QWidget *parent):QWidget(parent)
{
QGridLayout *layout=new QGridLayout(this,1,1,0,0);
QSpacerItem *spacer_top=new QSpacerItem(10,10,QSizePolicy::Minimum, QSizePolicy::Fixed);
layout->addItem(spacer_top,0,1);
QSpacerItem *spacer_left=new QSpacerItem(10,10,QSizePolicy::Fixed, QSizePolicy::Minimum);
layout->addItem(spacer_left,1,0);


// Logo
QPixmap permissionsSetupPixmap (locate("data", "krecipes/pics/dbpermissions.png"));
logo=new QLabel(this);
logo->setPixmap(permissionsSetupPixmap);
logo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
layout->addMultiCellWidget(logo,1,8,1,1);

// root checkbox
rootCheckBox=new QCheckBox(i18n("I already have a MySQL root/admin account set"),this,"rootCheckBox");
layout->addWidget(rootCheckBox,1,2);

QSpacerItem *rootInfoSpacer=new QSpacerItem(10,10,QSizePolicy::Minimum,QSizePolicy::Fixed);
layout->addItem(rootInfoSpacer,2,2);

// MySQL root/admin info
QVGroupBox *rootInfoVGBox=new QVGroupBox(this,"rootInfoVGBox"); rootInfoVGBox->setTitle(i18n("MySQL root info"));
rootInfoVGBox->setEnabled(false); // Disable by default
rootInfoVGBox->setInsideSpacing(10);
layout->addWidget(rootInfoVGBox,3,2);

// Input boxes (widgets inserted below)
QHBox *userBox=new QHBox(rootInfoVGBox); userBox->setSpacing(10);
QHBox *passBox=new QHBox(rootInfoVGBox); passBox->setSpacing(10);

// User Entry
QLabel *userLabel=new QLabel(userBox); userLabel->setText(i18n("Username:"));
userEdit=new KLineEdit(userBox);

// Password Entry
QLabel *passLabel=new QLabel(passBox); passLabel->setText(i18n("Password:"));
passEdit=new KLineEdit(passBox);

// Connect Signals & slots

connect(rootCheckBox,SIGNAL(toggled(bool)),rootInfoVGBox,SLOT(setEnabled(bool)));

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

QSpacerItem *spacer_from_image=new QSpacerItem(10,10,QSizePolicy::Fixed, QSizePolicy::Minimum);
layout->addItem(spacer_from_image,1,2);

QLabel* serverText=new QLabel(i18n("Server:"), this);
serverText->setFixedSize(QSize(100,20));
serverText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
layout->addWidget(serverText,1,3);

serverEdit=new KLineEdit(this);
serverEdit->setFixedSize(QSize(120,20));
serverEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
serverEdit->setText("localhost");
layout->addWidget(serverEdit,1,4);

QSpacerItem* spacerRow1 = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
layout->addItem( spacerRow1,2,3 );

QLabel* usernameText=new QLabel(i18n("Username:"), this);
usernameText->setFixedSize(QSize(100,20));
usernameText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
layout->addWidget(usernameText,3,3);

usernameEdit=new KLineEdit(this);
usernameEdit->setFixedSize(QSize(120,20));
usernameEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	// get username
	uid_t userID; QString username;struct passwd *user;
	userID=getuid();user=getpwuid (userID); username=user->pw_name;

usernameEdit->setText(username);
layout->addWidget(usernameEdit,3,4);

QSpacerItem* spacerRow2 = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
layout->addItem( spacerRow2,4,3 );

QLabel* passwordText=new QLabel(i18n("Password:"), this);
passwordText->setFixedSize(QSize(100,20));
passwordText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
layout->addWidget(passwordText,5,3);

passwordEdit=new KLineEdit(this);
passwordEdit->setFixedSize(QSize(120,20));
passwordEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
layout->addWidget(passwordEdit,5,4);

QSpacerItem* spacerRow3 = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
layout->addItem( spacerRow3, 6,3 );

QLabel* dbNameText=new QLabel(i18n("Database Name:"), this);
dbNameText->setFixedSize(QSize(100,20));
dbNameText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
layout->addWidget(dbNameText,7,3);

dbNameEdit=new KLineEdit(this);
dbNameEdit->setFixedSize(QSize(120,20));
dbNameEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
dbNameEdit->setText("Krecipes");
layout->addWidget(dbNameEdit,7,4);

QSpacerItem* spacerRow4 = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
layout->addItem( spacerRow4, 8,3 );
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

QSpacerItem *spacer_from_image=new QSpacerItem(10,10,QSizePolicy::Fixed, QSizePolicy::Minimum);
layout->addItem(spacer_left,1,2);

saveText=new QLabel(this);
saveText->setText(i18n("Congratulations! All the necessary configuration setup are done. Press 'Finish' to continue, and enjoy cooking!"));
saveText->setMinimumWidth(200);
saveText->setMaximumWidth(10000);
saveText->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

saveText->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter | QLabel::AlignJustify) );
layout->addWidget(saveText,1,3);


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


