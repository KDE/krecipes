/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   Copyright (C) 2004-2006 Jason Kivlighn (jkivlighn@gmail.com)          *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "pref.h"

#include "config-krecipes.h"

#include <qlayout.h>
#include <qlabel.h>


#include <q3buttongroup.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
#include <q3frame.h>
#include <qcombobox.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3GridLayout>
#include <Q3VBoxLayout>

#include <kapplication.h>
#include <kconfiggroup.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kfiledialog.h>
#include <knuminput.h>
#include <klineedit.h>
#include <kurlrequester.h>
#include <kcombobox.h>
#include <kdebug.h>
#include <ktoolinvocation.h>
#include <kglobal.h>
#include <kvbox.h>

KrecipesPreferences::KrecipesPreferences( QWidget *parent )
		: KPageDialog( parent )
{
	setFaceType( List );
	setObjectName( i18n( "Krecipes Preferences" ) );
	setButtons( Help | Ok | Cancel );
	setDefaultButton( Ok );

	// this is the base class for your preferences dialog.  it is now
	// a TreeList dialog.. but there are a number of other
	// possibilities (including Tab, Swallow, and just Plain)
	Q3Frame * frame;

	KConfigGroup config = KGlobal::config()->group( "DBType" );

	KIconLoader *il = KIconLoader::global();
	
	KPageWidgetItem * page = new KPageWidgetItem( frame , i18n( "Server Settings" ) );
	page->setHeader( i18n( "Database Server Options (%1)" , config.readEntry( "Type" )));
	il->loadIcon( "network-workgroup", KIconLoader::NoGroup, 32 );
	page->setIcon( KIcon::KIcon( "network-workgroup", il ) );
	addPage(page);

	Q3HBoxLayout* layout = new Q3HBoxLayout( frame );
	m_pageServer = new ServerPrefs( frame );
	layout->addWidget( m_pageServer );
	m_helpMap.insert(0,"configure-server-settings");


	page = new KPageWidgetItem( frame , i18n( "Formatting" ) );
	page->setHeader( i18n( "Customize Formatting" ) );
	il->loadIcon( "math_frac", KIconLoader::NoGroup, 32 );
	page->setIcon( KIcon::KIcon( "math_frac", il ) );
	addPage(page);

	Q3HBoxLayout* formatting_layout = new Q3HBoxLayout( frame );
	m_pageNumbers = new NumbersPrefs( frame );
	formatting_layout->addWidget( m_pageNumbers );
	m_helpMap.insert(1,"custom-formatting");

	page = new KPageWidgetItem( frame , "Import/Export" );
	page->setHeader( i18n( "Recipe Import and Export Options" ) );
	il->loadIcon( "go-down", KIconLoader::NoGroup, 32 );
	page->setIcon( KIcon::KIcon( "go-down", il ) );
	addPage(page);
	
	Q3HBoxLayout* import_layout = new Q3HBoxLayout( frame );
	m_pageImport = new ImportPrefs( frame );
	import_layout->addWidget( m_pageImport );
	m_helpMap.insert(2,"import-export-preference");

	page = new KPageWidgetItem( frame , "performance" );
	page->setHeader( i18n( "Performance Options" ) );
	il->loadIcon( "launch", KIconLoader::NoGroup, 32 );
	page->setIcon( KIcon::KIcon( "launch", il ) );
	addPage(page);
	
	Q3HBoxLayout* performance_layout = new Q3HBoxLayout( frame );
	m_pagePerformance = new PerformancePrefs( frame );
	performance_layout->addWidget( m_pagePerformance );
	m_helpMap.insert(3,"configure-performance");

	// Signals & Slots
	connect ( this, SIGNAL( okClicked() ), this, SLOT( saveSettings() ) );

}

void KrecipesPreferences::slotHelp()
{
	// KDE4 port to be done
	//KToolInvocation::invokeHelp( m_helpMap[currentPage()->name()] );
}


MySQLServerPrefs::MySQLServerPrefs( QWidget *parent ) : QWidget( parent )
{
	Q3GridLayout * layout = new Q3GridLayout( this, 1, 1, 0, 0 );
	layout->setSpacing( KDialog::spacingHint() );
	layout->setMargin( 0 );

	QSpacerItem* spacerTop = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerTop, 0, 1 );
	QSpacerItem* spacerLeft = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacerLeft, 1, 0 );

	QLabel* serverText = new QLabel( i18n( "Server:" ), this );
	serverText->setFixedSize( QSize( 100, 20 ) );
	serverText->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( serverText, 1, 1 );

	serverEdit = new KLineEdit( this );
	serverEdit->setFixedSize( QSize( 120, 20 ) );
	serverEdit->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( serverEdit, 1, 2 );

	QSpacerItem* spacerRow1 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerRow1, 2, 1 );

	QLabel* usernameText = new QLabel( i18n( "Username:" ), this );
	usernameText->setFixedSize( QSize( 100, 20 ) );
	usernameText->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( usernameText, 3, 1 );

	usernameEdit = new KLineEdit( this );
	usernameEdit->setFixedSize( QSize( 120, 20 ) );
	usernameEdit->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( usernameEdit, 3, 2 );

	QSpacerItem* spacerRow2 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerRow2, 4, 1 );

	QLabel* passwordText = new QLabel( i18n( "Password:" ), this );
	passwordText->setFixedSize( QSize( 100, 20 ) );
	passwordText->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( passwordText, 5, 1 );

	passwordEdit = new KLineEdit( this );
	passwordEdit->setFixedSize( QSize( 120, 20 ) );
	passwordEdit->setEchoMode( QLineEdit::Password );
	passwordEdit->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( passwordEdit, 5, 2 );

	QSpacerItem* spacerRow3 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerRow3, 6, 1 );

	QLabel* portText = new QLabel( i18n( "Port:" ), this );
	portText->setFixedSize( QSize( 100, 20 ) );
	portText->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( portText, 7, 1 );

	portEdit = new KIntNumInput( this );
	portEdit->setMinimum(0);
	portEdit->setSpecialValueText( i18n("Default") );
	portEdit->setFixedSize( QSize( 120, 20 ) );
	portEdit->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( portEdit, 7, 2 );

	QSpacerItem* spacerRow4 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerRow4, 8, 1 );

	QLabel* dbNameText = new QLabel( i18n( "Database name:" ), this );
	dbNameText->setFixedSize( QSize( 100, 20 ) );
	dbNameText->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( dbNameText, 9, 1 );

	dbNameEdit = new KLineEdit( this );
	dbNameEdit->setFixedSize( QSize( 120, 20 ) );
	dbNameEdit->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( dbNameEdit, 9, 2 );

	QSpacerItem* spacerRow5 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
	layout->addItem( spacerRow5, 10, 1 );

	// Backup options
	Q3GroupBox *backupGBox = new Q3GroupBox( this, "backupGBox" );
	backupGBox->setTitle( i18n( "Backup" ) );
	backupGBox->setColumns( 2 );
	layout->addMultiCellWidget( backupGBox, 10, 10, 1, 4 );

	QLabel *dumpPathLabel = new QLabel( backupGBox );
	dumpPathLabel->setText( i18n( "Path to '%1':" ,QString("mysqldump") ));

	QLabel *mysqlPathLabel = new QLabel( backupGBox );
	mysqlPathLabel->setText( i18n( "Path to '%1':" ,QString("mysql") ));

	QSpacerItem* spacerRow6 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
	layout->addItem( spacerRow6, 11, 1 );
	QSpacerItem* spacerRight = new QSpacerItem( 10, 10, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
	layout->addItem( spacerRight, 1, 4 );

	// Load & Save Current Settings
	KConfigGroup config = KGlobal::config()->group( "Server" );
	serverEdit->setText( config.readEntry( "Host", "localhost" ) );
	usernameEdit->setText( config.readEntry( "Username", "" ) );
	passwordEdit->setText( config.readEntry( "Password", "" ) );
	portEdit->setValue( config.readEntry( "Port", 0 ) );
	dbNameEdit->setText( config.readEntry( "DBName", "Krecipes" ) );
	dumpPathRequester = new KUrlRequester( config.readEntry( "MySQLDumpPath", "mysqldump" ), backupGBox );
	dumpPathRequester->setFilter( "mysqldump" );
	mysqlPathRequester = new KUrlRequester( config.readEntry( "MySQLPath", "mysql" ), backupGBox );
	mysqlPathRequester->setFilter( "mysql" );
}

void MySQLServerPrefs::saveOptions( void )
{
	KConfigGroup config = KGlobal::config()->group( "Server" );
	config.writeEntry( "Host", serverEdit->text() );
	config.writeEntry( "Username", usernameEdit->text() );
	config.writeEntry( "Password", passwordEdit->text() );
	config.writeEntry( "Port", portEdit->value() );
	config.writeEntry( "DBName", dbNameEdit->text() );
	config.writeEntry( "MySQLDumpPath", dumpPathRequester->url() );
	config.writeEntry( "MySQLPath", mysqlPathRequester->url() );
}


PostgreSQLServerPrefs::PostgreSQLServerPrefs( QWidget *parent ) : QWidget( parent )
{
	Q3GridLayout * layout = new Q3GridLayout( this, 1, 1, 0, 0 );
	layout->setSpacing( KDialog::spacingHint() );
	layout->setMargin( 0 );

	QSpacerItem* spacerTop = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerTop, 0, 1 );
	QSpacerItem* spacerLeft = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacerLeft, 1, 0 );

	QLabel* serverText = new QLabel( i18n( "Server:" ), this );
	serverText->setFixedSize( QSize( 100, 20 ) );
	serverText->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( serverText, 1, 1 );

	serverEdit = new KLineEdit( this );
	serverEdit->setFixedSize( QSize( 120, 20 ) );
	serverEdit->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( serverEdit, 1, 2 );

	QSpacerItem* spacerRow1 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerRow1, 2, 1 );

	QLabel* usernameText = new QLabel( i18n( "Username:" ), this );
	usernameText->setFixedSize( QSize( 100, 20 ) );
	usernameText->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( usernameText, 3, 1 );

	usernameEdit = new KLineEdit( this );
	usernameEdit->setFixedSize( QSize( 120, 20 ) );
	usernameEdit->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( usernameEdit, 3, 2 );

	QSpacerItem* spacerRow2 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerRow2, 4, 1 );

	QLabel* passwordText = new QLabel( i18n( "Password:" ), this );
	passwordText->setFixedSize( QSize( 100, 20 ) );
	passwordText->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( passwordText, 5, 1 );

	passwordEdit = new KLineEdit( this );
	passwordEdit->setFixedSize( QSize( 120, 20 ) );
	passwordEdit->setEchoMode( QLineEdit::Password );
	passwordEdit->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( passwordEdit, 5, 2 );

	QSpacerItem* spacerRow3 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerRow3, 6, 1 );

	QLabel* portText = new QLabel( i18n( "Port:" ), this );
	portText->setFixedSize( QSize( 100, 20 ) );
	portText->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( portText, 7, 1 );

	portEdit = new KIntNumInput( this );
	portEdit->setMinimum(0);
	portEdit->setSpecialValueText( i18n("Default") );
	portEdit->setFixedSize( QSize( 120, 20 ) );
	portEdit->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( portEdit, 7, 2 );

	QSpacerItem* spacerRow4 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerRow4, 8, 1 );

	QLabel* dbNameText = new QLabel( i18n( "Database name:" ), this );
	dbNameText->setFixedSize( QSize( 100, 20 ) );
	dbNameText->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( dbNameText, 9, 1 );

	dbNameEdit = new KLineEdit( this );
	dbNameEdit->setFixedSize( QSize( 120, 20 ) );
	dbNameEdit->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( dbNameEdit, 9, 2 );

	QSpacerItem* spacerRow5 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
	layout->addItem( spacerRow5, 10, 1 );

	// Backup options
	Q3GroupBox *backupGBox = new Q3GroupBox( this, "backupGBox" );
	backupGBox->setTitle( i18n( "Backup" ) );
	backupGBox->setColumns( 2 );
	layout->addMultiCellWidget( backupGBox, 10, 10, 1, 4 );

	QLabel *dumpPathLabel = new QLabel( backupGBox );
	dumpPathLabel->setText( i18n( "Path to '%1':" ,QString("pg_dump") ));

	QLabel *psqlPathLabel = new QLabel( backupGBox );
	psqlPathLabel->setText( i18n( "Path to '%1':" ,QString("psql") ));

	QSpacerItem* spacerRow6 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
	layout->addItem( spacerRow6, 11, 1 );
	QSpacerItem* spacerRight = new QSpacerItem( 10, 10, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
	layout->addItem( spacerRight, 1, 4 );

	// Load & Save Current Settings
	KConfigGroup config = KGlobal::config()->group( "Server" );
	serverEdit->setText( config.readEntry( "Host", "localhost" ) );
	usernameEdit->setText( config.readEntry( "Username", "" ) );
	passwordEdit->setText( config.readEntry( "Password", "" ) );
	portEdit->setValue( config.readEntry( "Port", 0 ) );
	dbNameEdit->setText( config.readEntry( "DBName", "Krecipes" ) );
	dumpPathRequester = new KUrlRequester( config.readEntry( "PgDumpPath", "pg_dump" ), backupGBox );
	dumpPathRequester->setFilter( "pg_dump" );
	psqlPathRequester = new KUrlRequester( config.readEntry( "PsqlPath", "psql" ), backupGBox );
	psqlPathRequester->setFilter( "psql" );
}

void PostgreSQLServerPrefs::saveOptions( void )
{
	KConfigGroup config = KGlobal::config()->group( "Server" );
	config.writeEntry( "Host", serverEdit->text() );
	config.writeEntry( "Username", usernameEdit->text() );
	config.writeEntry( "Password", passwordEdit->text() );
	config.writeEntry( "Port", portEdit->value() );
	config.writeEntry( "DBName", dbNameEdit->text() );
	config.writeEntry( "PgDumpPath", dumpPathRequester->url() );
	config.writeEntry( "PsqlPath", psqlPathRequester->url() );
}



SQLiteServerPrefs::SQLiteServerPrefs( QWidget *parent ) : QWidget( parent )
{
	Q3VBoxLayout * Form1Layout = new Q3VBoxLayout( this );

	KHBox *hbox = new KHBox( this );
	( void ) new QLabel( i18n( "Database file:" ), hbox );

	Form1Layout->addWidget( hbox );

	QSpacerItem* spacerRow5 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
	Form1Layout->addItem( spacerRow5 );

	QString sqliteBinary;
	#if HAVE_SQLITE3
	sqliteBinary = "sqlite3";
 	#elif HAVE_SQLITE
	sqliteBinary = "sqlite";
	#endif

	// Backup options
	Q3GroupBox *backupGBox = new Q3GroupBox( this, "backupGBox" );
	backupGBox->setTitle( i18n( "Backup" ) );
	backupGBox->setColumns( 2 );
	Form1Layout->addWidget( backupGBox );

	QLabel *dumpPathLabel = new QLabel( backupGBox );
	dumpPathLabel->setText( i18n( "Path to '%1':" ,sqliteBinary));

	// Load & Save Current Settings
	KConfigGroup config = KGlobal::config()->group( "Server" );
	fileRequester = new KUrlRequester( config.readEntry( "DBFile", KStandardDirs::locateLocal( "appdata", "krecipes.krecdb" ) ), hbox );
	hbox->setStretchFactor( fileRequester, 2 );
	dumpPathRequester = new KUrlRequester( config.readEntry( "SQLitePath", sqliteBinary ), backupGBox );
	dumpPathRequester->setFilter( sqliteBinary );
}

void SQLiteServerPrefs::saveOptions( void )
{
	KConfigGroup config = KGlobal::config()->group( "Server" );
	config.writeEntry( "DBFile", fileRequester->url() );
	config.writeEntry( "SQLitePath", dumpPathRequester->url() );
}


// Server Setttings Dialog
ServerPrefs::ServerPrefs( QWidget *parent )
		: QWidget( parent )
{
	Q3VBoxLayout * Form1Layout = new Q3VBoxLayout( this, 11, 6 );

	KConfigGroup config = KGlobal::config()->group( "DBType" );
	QString DBtype = config.readEntry( "Type" );
	if ( DBtype == "MySQL" )
		serverWidget = new MySQLServerPrefs( this );
	else if ( DBtype == "PostgreSQL" )
		serverWidget = new PostgreSQLServerPrefs( this );
	else
		serverWidget = new SQLiteServerPrefs( this );

	serverWidget->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	Form1Layout->addWidget( serverWidget );

	Form1Layout->addItem( new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

	wizard_button = new QCheckBox( i18n( "Re-run wizard on next startup" ), this );
	wizard_button->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	Form1Layout->addWidget( wizard_button );

	QLabel *note = new QLabel( i18n( "Note: Krecipes must be restarted for most server preferences to take effect." ), this );
	Form1Layout->addWidget( note );

	adjustSize();
}


void KrecipesPreferences::saveSettings( void )
{
	m_pageServer->saveOptions();
	m_pageNumbers->saveOptions();
	m_pageImport->saveOptions();
	m_pagePerformance->saveOptions();
}

// Save Server settings
void ServerPrefs::saveOptions( void )
{
	KConfigGroup config = KGlobal::config()->group( "DBType" );
	QString DBtype = config.readEntry( "Type" );
	if ( DBtype == "MySQL" )
		( ( MySQLServerPrefs* ) serverWidget ) ->saveOptions();
	else if ( DBtype == "PostgreSQL" )
		( ( PostgreSQLServerPrefs* ) serverWidget ) ->saveOptions();
	else
		( ( SQLiteServerPrefs* ) serverWidget ) ->saveOptions();

	if ( wizard_button->isChecked() ) {
		config = KGlobal::config()->group( "Wizard" );	
		config.writeEntry( "SystemSetup", false );
	}
}

//=============Numbers Preferences Dialog================//
NumbersPrefs::NumbersPrefs( QWidget *parent )
		: QWidget( parent )
{
	Form1Layout = new Q3VBoxLayout( this, 11, 6 );

	numberButtonGroup = new Q3ButtonGroup( this );
	numberButtonGroup->setColumnLayout( 0, Qt::Vertical );
	numberButtonGroup->layout() ->setSpacing( 6 );
	numberButtonGroup->layout() ->setMargin( 11 );
	numberButtonGroup->resize( QSize() );
	numberButtonGroupLayout = new Q3VBoxLayout( numberButtonGroup->layout() );
	numberButtonGroupLayout->setAlignment( Qt::AlignTop );

	fractionRadioButton = new QRadioButton( numberButtonGroup );
	numberButtonGroupLayout->addWidget( fractionRadioButton );

	decimalRadioButton = new QRadioButton( numberButtonGroup );
	numberButtonGroupLayout->addWidget( decimalRadioButton );
	Form1Layout->addWidget( numberButtonGroup );

	numberButtonGroup->insert( decimalRadioButton, 0 );
	numberButtonGroup->insert( fractionRadioButton, 1 );

	//unit display format
	Q3GroupBox *abbrevGrpBox = new Q3GroupBox( 1, Qt::Vertical, i18n( "Units" ), this );
	KHBox *abbrevBox = new KHBox( abbrevGrpBox );
	abbrevButton = new QCheckBox( i18n( "Use abbreviations" ), abbrevBox );
	Form1Layout->addWidget( abbrevGrpBox );


	Form1Layout->addItem( new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

	adjustSize();

	languageChange();

	// Load Current Settings
	KConfigGroup config = KGlobal::config()->group( "Formatting" );

	int button = ( config.readEntry( "Fraction", false ) ) ? 1 : 0;
	numberButtonGroup->setButton( button );

	abbrevButton->setChecked( config.readEntry( "AbbreviateUnits", false ) );
}

void NumbersPrefs::saveOptions()
{
	KConfigGroup config = KGlobal::config()->group( "Formatting" );

	bool fraction = !numberButtonGroup->find( 0 ) ->isOn();
	config.writeEntry( "Fraction", fraction );

	config.writeEntry( "AbbreviateUnits", abbrevButton->isChecked() );
}

void NumbersPrefs::languageChange()
{
	numberButtonGroup->setTitle( i18n( "Number Format" ) );
	fractionRadioButton->setText( i18n( "Fraction" ) );
	decimalRadioButton->setText( i18n( "Decimal" ) );
}

//=============Import/Export Preferences Dialog================//
ImportPrefs::ImportPrefs( QWidget *parent )
		: QWidget( parent )
{
	// Load Current Settings
	KConfigGroup config = KGlobal::config()->group( "Import" );

	bool overwrite = config.readEntry( "OverwriteExisting", false );
	bool direct = config.readEntry( "DirectImport", false );

	Form1Layout = new Q3VBoxLayout( this, 11, 6 );

	Q3GroupBox *importGroup = new Q3GroupBox(2,Qt::Vertical,i18n("Import"), this);

	overwriteCheckbox = new QCheckBox( i18n( "Overwrite recipes with same title" ), importGroup );
	overwriteCheckbox->setChecked( overwrite );
	overwriteCheckbox->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );

	directImportCheckbox = new QCheckBox( i18n( "Ask which recipes to import" ), importGroup );
	directImportCheckbox->setChecked( !direct );
	directImportCheckbox->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );

	Form1Layout->addWidget(importGroup);

	Q3GroupBox *exportGroup = new Q3GroupBox(1,Qt::Vertical,i18n("Export"), this);

	KHBox *clipboardHBox = new KHBox(exportGroup);
	clipboardHBox->setSpacing(6);
	/*QLabel *clipboardLabel = */new QLabel(i18n("'Copy to Clipboard' format:"),clipboardHBox);
	clipBoardFormatComboBox = new QComboBox( clipboardHBox );
	clipBoardFormatComboBox->insertItem(QString("%3 (*.txt)").arg(i18n("Plain Text")));
	clipBoardFormatComboBox->insertItem("Krecipes (*.kreml)");
	clipBoardFormatComboBox->insertItem("Meal-Master (*.mmf)");
	clipBoardFormatComboBox->insertItem("Rezkonv (*.rk)");
	clipBoardFormatComboBox->insertItem("RecipeML (*.xml)");
	//clipBoardFormatComboBox->insertItem("CookML (*.cml)");
	clipboardHBox->setStretchFactor(clipBoardFormatComboBox,1);

	config = KGlobal::config()->group( "Export" );
	QString clipboardFormat = config.readEntry("ClipboardFormat");
	if ( clipboardFormat == "*.kreml" )
		clipBoardFormatComboBox->setCurrentItem(1);
	else if ( clipboardFormat == "*.mmf" )
		clipBoardFormatComboBox->setCurrentItem(2);
	else if ( clipboardFormat == "*.xml" )
		clipBoardFormatComboBox->setCurrentItem(3);
	else
		clipBoardFormatComboBox->setCurrentItem(0);

	Form1Layout->addWidget(exportGroup);

	Form1Layout->addItem( new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

	Q3WhatsThis::add( directImportCheckbox, 
		i18n("When this is enabled, the importer will show every recipe in the file(s) and allow you to select which recipes you want imported.\n \
							\
		Disable this to always import every recipe, which allows for faster and less memory-intensive imports.")
	);

	adjustSize();
}

void ImportPrefs::saveOptions()
{
	KConfigGroup config = KGlobal::config()->group( "Import" );

	config.writeEntry( "OverwriteExisting", overwriteCheckbox->isChecked() );
	config.writeEntry( "DirectImport", !directImportCheckbox->isChecked() );

	config = KGlobal::config()->group( "Export" );
	QString ext = clipBoardFormatComboBox->currentText().mid(clipBoardFormatComboBox->currentText().find("(")+1,clipBoardFormatComboBox->currentText().length()-clipBoardFormatComboBox->currentText().find("(")-2);
	config.writeEntry( "ClipboardFormat", ext );
}


//=============Performance Options Dialog================//
PerformancePrefs::PerformancePrefs( QWidget *parent )
		: QWidget( parent )
{
	// Load Current Settings
	KConfigGroup config = KGlobal::config()->group( "Performance" );

	int cat_limit = config.readEntry( "CategoryLimit", -1 );
	int limit = config.readEntry( "Limit", -1 );

	Form1Layout = new Q3VBoxLayout( this, 11, 6 );

	searchAsYouTypeBox = new QCheckBox( i18n( "Search as you type" ), this );
	searchAsYouTypeBox->setChecked( config.readEntry( "SearchAsYouType", true ) );

	QLabel *explainationLabel = new QLabel( i18n("In most instances these options do not need to be changed.  However, limiting the amount of items displayed at once will <b>allow Krecipes to better perform when the database is loaded with many thousands of recipes</b>."), this );
	explainationLabel->setTextFormat( Qt::RichText );

	KHBox *catLimitHBox = new KHBox( this );
	catLimitInput = new KIntNumInput(catLimitHBox);
	catLimitInput->setLabel( i18n( "Number of categories to display at once:" ) );
	catLimitInput->setRange(0,5000,20,true);
	catLimitInput->setSpecialValueText( i18n("Unlimited") );

	if ( cat_limit > 0 )
		catLimitInput->setValue( cat_limit );

	KHBox *limitHBox = new KHBox( this );
	limitInput = new KIntNumInput(limitHBox);
	limitInput->setLabel( i18n( "Number of elements to display at once:" ) );
	limitInput->setRange(0,100000,1000,true);
	limitInput->setSpecialValueText( i18n("Unlimited") );

	if ( limit > 0 )
		limitInput->setValue( limit );

	Form1Layout->addWidget( searchAsYouTypeBox );
	Form1Layout->addWidget( explainationLabel );
	Form1Layout->addWidget( catLimitHBox );
	Form1Layout->addWidget( limitHBox );

	Form1Layout->addItem( new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

	adjustSize();
}

void PerformancePrefs::saveOptions()
{
	KConfigGroup config = KGlobal::config()->group( "Performance" );

	int catLimit = ( catLimitInput->value() == 0 ) ? -1 : catLimitInput->value();
	config.writeEntry( "CategoryLimit", catLimit );

	int limit = ( limitInput->value() == 0 ) ? -1 : limitInput->value();
	config.writeEntry( "Limit", limit );

	config.writeEntry( "SearchAsYouType", searchAsYouTypeBox->isChecked() );
}

#include "pref.moc"
