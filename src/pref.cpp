/*
 * Copyright (C) 2003 Unai Garro <ugarro@users.sourceforge.net>
 */

#include "pref.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include <kapp.h>
#include <kconfig.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kfiledialog.h>


KrecipesPreferences::KrecipesPreferences(QWidget *parent)
    : KDialogBase(IconList, i18n("Krecipes Preferences"),
                  Help|Ok|Cancel, Ok, parent)
{
    // this is the base class for your preferences dialog.  it is now
    // a TreeList dialog.. but there are a number of other
    // possibilities (including Tab, Swallow, and just Plain)
    QFrame *frame;
    KIconLoader il;
    frame = addPage(i18n("Server Settings"), i18n("Database Server Options"),il.loadIcon("network_local",KIcon::NoGroup,32));
    QHBoxLayout* layout = new QHBoxLayout( frame );
    m_pageServer = new ServerPrefs(frame);
    layout->addWidget(m_pageServer);

    frame = addPage(i18n("Formatting"), i18n("Customize Formatting"),il.loadIcon("math_frac",KIcon::NoGroup,32));
    QHBoxLayout* formatting_layout = new QHBoxLayout( frame );
    m_pageNumbers = new NumbersPrefs(frame);
    formatting_layout->addWidget(m_pageNumbers);

    frame = addPage(i18n("Import"), i18n("Recipe Import Options"),il.loadIcon("down",KIcon::NoGroup,32));
    QHBoxLayout* import_layout = new QHBoxLayout( frame );
    m_pageImport = new ImportPrefs(frame);
    import_layout->addWidget(m_pageImport);

    //setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    // Signals & Slots
    connect (this, SIGNAL(okClicked()), this, SLOT(saveSettings()));

}


MySQLServerPrefs::MySQLServerPrefs( QWidget *parent ) : QWidget(parent)
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
	passwordEdit->setEchoMode(QLineEdit::Password);
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

void MySQLServerPrefs::saveOptions(void)
{
	KConfig *config=kapp->config();
	config->setGroup("Server");
	config->writeEntry("Host",serverEdit->text());
	config->writeEntry("Username",usernameEdit->text());
	config->writeEntry("Password",passwordEdit->text());
	config->writeEntry("DBName",dbNameEdit->text());
}


PostgreSQLServerPrefs::PostgreSQLServerPrefs( QWidget *parent ) : QWidget(parent)
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
	passwordEdit->setEchoMode(QLineEdit::Password);
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

void PostgreSQLServerPrefs::saveOptions(void)
{
	KConfig *config=kapp->config();
	config->setGroup("Server");
	config->writeEntry("Host",serverEdit->text());
	config->writeEntry("Username",usernameEdit->text());
	config->writeEntry("Password",passwordEdit->text());
	config->writeEntry("DBName",dbNameEdit->text());
}



SQLiteServerPrefs::SQLiteServerPrefs( QWidget *parent ) : QWidget(parent)
{
	QVBoxLayout *Form1Layout = new QVBoxLayout( this );

	QHBox *hbox = new QHBox(this);
	(void)new QLabel(i18n("Database file:"), hbox);

	fileEdit=new KLineEdit(hbox);
	hbox->setStretchFactor(fileEdit,2);

	KIconLoader il;
	QPushButton *file_select=new QPushButton(il.loadIcon("fileopen",KIcon::NoGroup,16),QString::null,hbox);
	QToolTip::add(file_select,i18n("Open file dialog"));
	file_select->setFixedWidth( 25 );

	Form1Layout->addWidget(hbox);

	connect( file_select, SIGNAL(clicked()), SLOT(selectFile()) );

	// Load & Save Current Settings
	KConfig *config=kapp->config();
	config->setGroup("Server");
	fileEdit->setText( config->readEntry( "DBFile", locateLocal("appdata","krecipes.krecdb") ) );
}

void SQLiteServerPrefs::saveOptions(void)
{
	KConfig *config=kapp->config();
	config->setGroup("Server");
	config->writeEntry("DBFile",fileEdit->text());
}

void SQLiteServerPrefs::selectFile()
{
	KFileDialog dialog(QString::null,"*.*|All Files",this,"dialog",true);
	if ( dialog.exec() == QDialog::Accepted ) {
		fileEdit->setText( dialog.selectedFile() );
	}
}


// Server Setttings Dialog
ServerPrefs::ServerPrefs(QWidget *parent)
    : QWidget(parent)
{
	QVBoxLayout *Form1Layout = new QVBoxLayout( this, 11, 6 );

	KConfig *config=kapp->config();
	config->setGroup("DBType");
	QString DBtype = config->readEntry("Type");
	if ( DBtype == "MySQL" )
		serverWidget = new MySQLServerPrefs(this);
	else if ( DBtype == "PostgreSQL" )
		serverWidget = new PostgreSQLServerPrefs(this);
	else
		serverWidget = new SQLiteServerPrefs(this);

	serverWidget->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Minimum);
	Form1Layout->addWidget( serverWidget );

	Form1Layout->addItem( new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

	wizard_button = new QCheckBox(i18n("Re-run wizard on next startup"),this);
	wizard_button->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Minimum);
	Form1Layout->addWidget( wizard_button );

	QLabel *note = new QLabel(i18n("Note: Krecipes must be restarted for most server preferences to take effect."),this);
	Form1Layout->addWidget( note );

	adjustSize();
}


void KrecipesPreferences::saveSettings(void)
{
m_pageServer->saveOptions();
m_pageNumbers->saveOptions();
m_pageImport->saveOptions();
}

// Save Server settings
void ServerPrefs::saveOptions(void)
{
KConfig *config=kapp->config();
config->setGroup("DBType");
QString DBtype = config->readEntry("Type");
if ( DBtype == "MySQL" )
   ((MySQLServerPrefs*)serverWidget)->saveOptions();
else if ( DBtype == "MySQL" )
   ((PostgreSQLServerPrefs*)serverWidget)->saveOptions();
else
   ((SQLiteServerPrefs*)serverWidget)->saveOptions();
   
if ( wizard_button->isChecked() ) {
   config->setGroup("Wizard");
   config->writeEntry("SystemSetup",false);
}
}

//=============Numbers Preferences Dialog================//
NumbersPrefs::NumbersPrefs(QWidget *parent)
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

    numberButtonGroup->insert( decimalRadioButton, 0 );
    numberButtonGroup->insert( fractionRadioButton, 1 );

    //ingredient display format
    QGroupBox *ingredientGrpBox = new QGroupBox(2,Qt::Vertical,i18n("Ingredient"),this);

    QHBox *ingredientBox = new QHBox(ingredientGrpBox);
    (void)new QLabel(i18n("Ingredient Format:"), ingredientBox);
    ingredientEdit=new KLineEdit(ingredientBox);
    (void)new QLabel(i18n(	"%n: Name<br>"
    				"%p: Preparation method<br>" 
				"%a: Amount<br>"
				"%u: Unit"
			),ingredientGrpBox);

    Form1Layout->addWidget(ingredientGrpBox);
    
    Form1Layout->addItem( new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding ) );
        
    adjustSize();

    languageChange();

    // Load Current Settings
    KConfig *config=kapp->config();
    config->setGroup("Formatting");

    int button = ( config->readBoolEntry( "Fraction", false ) ) ? 1 : 0;
    numberButtonGroup->setButton( button );
    
    ingredientEdit->setText( config->readEntry("Ingredient","%n%p: %a %u") );
}

void NumbersPrefs::saveOptions()
{
	KConfig *config=kapp->config();
	config->setGroup("Formatting");
	
	bool fraction = !numberButtonGroup->find( 0 )->isOn();
	config->writeEntry("Fraction",fraction);
	
	config->writeEntry("Ingredient",ingredientEdit->text());
}

void NumbersPrefs::languageChange()
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
    overwriteCheckbox->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Minimum);

    Form1Layout->addWidget(overwriteCheckbox);
    
    Form1Layout->addItem( new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

    adjustSize();
}

void ImportPrefs::saveOptions()
{
	KConfig *config=kapp->config();
	config->setGroup("Import");

	config->writeEntry( "OverwriteExisting", overwriteCheckbox->isChecked() );
}

#include "pref.moc"
