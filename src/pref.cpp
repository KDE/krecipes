/*
 * Copyright (C) 2003 Unai Garro <uga@ee.ed.ac.uk>
 */

#include "pref.h"

#include <klocale.h>
#include <kapp.h>
#include <kconfig.h>
#include <qlayout.h>
#include <qlabel.h>

KrecipesPreferences::KrecipesPreferences()
    : KDialogBase(IconList, i18n("Krecipes Preferences"),
                  Help|Default|Ok|Apply|Cancel, Ok)
{
    // this is the base class for your preferences dialog.  it is now
    // a TreeList dialog.. but there are a number of other
    // possibilities (including Tab, Swallow, and just Plain)
    QFrame *frame;

    frame = addPage(i18n("Server Settings"), i18n("Database Server Options"));
    QHBoxLayout* layout = new QHBoxLayout( frame );
    layout->setSpacing(0);
    layout->setMargin(0);
    m_pageServer = new ServerPrefs(frame);
    layout->addWidget(m_pageServer);


    frame = addPage(i18n("Appearance"), i18n("Customize Krecipes Appearance"));
    m_pageTwo = new KrecipesPrefPageTwo(frame);

    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    // Signals & Slots
    connect (this, SIGNAL(okClicked()), this, SLOT(saveSettings()));

}



// Server Setttings Dialog
ServerPrefs::ServerPrefs(QWidget *parent)
    : QWidget(parent)
{

    QGridLayout *layout = new QGridLayout(this,1,1,0,0);
    layout->setSpacing( KDialog::spacingHint() );
    layout->setMargin(0);

    QSpacerItem* spacerTop = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout->addItem( spacerTop, 0,1);
    QSpacerItem* spacerLeft = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum);
    layout->addItem( spacerLeft,1,0 );

    QLabel* serverText=new QLabel(i18n("Server:"), this);
    serverText->setFixedSize(QSize(100,20));
    serverText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(serverText,1,1);

    serverEdit=new KLineEdit(this);
    serverEdit->setFixedSize(QSize(120,20));
    serverEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(serverEdit,1,2);

    QSpacerItem* spacerRow1 = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout->addItem( spacerRow1,2,1 );

    QLabel* usernameText=new QLabel(i18n("Username:"), this);
    usernameText->setFixedSize(QSize(100,20));
    usernameText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(usernameText,3,1);

    usernameEdit=new KLineEdit(this);
    usernameEdit->setFixedSize(QSize(120,20));
    usernameEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(usernameEdit,3,2);

    QSpacerItem* spacerRow2 = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout->addItem( spacerRow2,4,1 );

    QLabel* passwordText=new QLabel(i18n("Password:"), this);
    passwordText->setFixedSize(QSize(100,20));
    passwordText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(passwordText,5,1);

    passwordEdit=new KLineEdit(this);
    passwordEdit->setFixedSize(QSize(120,20));
    passwordEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(passwordEdit,5,2);

    QSpacerItem* spacerRow3 = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout->addItem( spacerRow3, 6,1 );

    QLabel* dbNameText=new QLabel(i18n("Database Name:"), this);
    dbNameText->setFixedSize(QSize(100,20));
    dbNameText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(dbNameText,7,1);

    dbNameEdit=new KLineEdit(this);
    dbNameEdit->setFixedSize(QSize(120,20));
    dbNameEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(dbNameEdit,7,2);

    QSpacerItem* spacerRow4 = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    layout->addItem( spacerRow4, 8,1 );
    QSpacerItem* spacerRight = new QSpacerItem( 10,10, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
    layout->addItem( spacerRight, 1,4 );


    // Load & Save Current Settings
    KConfig *config=kapp->config();
    config->setGroup("Server");
    serverEdit->setText( config->readEntry( "Host","localhost"));
    usernameEdit->setText( config->readEntry( "Username",""));
    passwordEdit->setText( config->readEntry("Password",""));
    dbNameEdit->setText( config->readEntry( "DBName", "Krecipes") );


}

KrecipesPrefPageTwo::KrecipesPrefPageTwo(QWidget *parent)
    : QFrame(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setAutoAdd(true);

    new QLabel(i18n("Add something here"), this);
}


void KrecipesPreferences::saveSettings(void)
{
m_pageServer->saveOptions();

}

// Save Server settings
void ServerPrefs::saveOptions(void)
{
KConfig *config=kapp->config();
config->setGroup("Server");
config->writeEntry("Host",serverEdit->text());
config->writeEntry("Username",usernameEdit->text());
config->writeEntry("Password",passwordEdit->text());
config->writeEntry("DBName",dbNameEdit->text());
}



#include "pref.moc"
