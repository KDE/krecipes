/*
 * Copyright (C) 2003 Unai Garro <ugarro@users.sourceforge.net>
 */

#include "pref.h"

#include <klocale.h>
#include <kapp.h>
#include <kconfig.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qradiobutton.h>

KrecipesPreferences::KrecipesPreferences(QWidget *parent)
    : KDialogBase(IconList, i18n("Krecipes Preferences"),
                  Help|Default|Ok|Apply|Cancel, Ok, parent)
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

    frame = addPage(i18n("Units"), i18n("Customize Units"));
    m_pageUnits = new UnitsPrefs(frame);
    layout->addWidget(m_pageUnits);

    frame = addPage(i18n("Import"), i18n("Recipe import options"));
    m_pageImport = new ImportPrefs(frame);
    layout->addWidget(m_pageImport);

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

    QLabel* dbNameText=new QLabel(i18n("Database name:"), this);
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
    : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setAutoAdd(true);

    new QLabel(i18n("Add something here"), this);
}

void KrecipesPreferences::saveSettings(void)
{
m_pageServer->saveOptions();
m_pageUnits->saveOptions();
m_pageImport->saveOptions();
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

//=============Units Preferences Dialog================//
UnitsPrefs::UnitsPrefs(QWidget *parent)
    : QWidget(parent)
{
    Form1Layout = new QVBoxLayout( this, 11, 6 );

    numberButtonGroup = new QButtonGroup( this );
    numberButtonGroup->setColumnLayout(0, Qt::Vertical );
    numberButtonGroup->layout()->setSpacing( 6 );
    numberButtonGroup->layout()->setMargin( 11 );
    numberButtonGroup->resize( QSize() );
    numberButtonGroupLayout = new QVBoxLayout( numberButtonGroup->layout() );
    numberButtonGroupLayout->setAlignment( Qt::AlignTop );

    fractionRadioButton = new QRadioButton( numberButtonGroup );
    numberButtonGroupLayout->addWidget( fractionRadioButton );

    decimalRadioButton = new QRadioButton( numberButtonGroup );
    numberButtonGroupLayout->addWidget( decimalRadioButton );
    Form1Layout->addWidget( numberButtonGroup );
    QSpacerItem* spacer = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Form1Layout->addItem( spacer );

    numberButtonGroup->insert( decimalRadioButton, 0 );
    numberButtonGroup->insert( fractionRadioButton, 1 );

    adjustSize();
    resize(300,height());

    languageChange();

    // Load Current Settings
    KConfig *config=kapp->config();
    config->setGroup("Units");

    int button = ( config->readBoolEntry( "Fraction", false ) ) ? 1 : 0;
    numberButtonGroup->setButton( button );
}

void UnitsPrefs::saveOptions()
{
	KConfig *config=kapp->config();
	config->setGroup("Units");

	bool fraction = !numberButtonGroup->find( 0 )->isOn();
	config->writeEntry("Fraction",fraction);
}

void UnitsPrefs::languageChange()
{
    numberButtonGroup->setTitle( i18n( "Number Format" ) );
    fractionRadioButton->setText( i18n( "Fraction" ) );
    decimalRadioButton->setText( i18n( "Decimal" ) );
}

//=============Import Preferences Dialog================//
ImportPrefs::ImportPrefs(QWidget *parent)
    : QWidget(parent)
{
    // Load Current Settings
    KConfig *config=kapp->config();
    config->setGroup("Import");

    bool overwrite = config->readBoolEntry( "OverwriteExisting", false );

    Form1Layout = new QVBoxLayout( this, 11, 6 );

    overwriteCheckbox = new QCheckBox( i18n("Overwrite recipes with same title"), this );
    overwriteCheckbox->setChecked( overwrite );

    Form1Layout->addWidget(overwriteCheckbox);

    adjustSize();
    resize(300,height());
}

void ImportPrefs::saveOptions()
{
	KConfig *config=kapp->config();
	config->setGroup("Import");

	config->writeEntry( "OverwriteExisting", overwriteCheckbox->isChecked() );
}

#include "pref.moc"
