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

#include <config-krecipes.h>

#include <QLayout>
#include <QLabel>


#include <q3buttongroup.h>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
#include <QToolTip>

#include <qframe.h>
#include <KComboBox>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>

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

#include <sonnet/configwidget.h>

KrecipesPreferences::KrecipesPreferences( QWidget *parent )
		: KPageDialog( parent )
{
	setFaceType( List );
	setObjectName( "KrecipesPreferences" );
	setCaption( i18n( "Configure" ) );
	setButtons( Help | Ok | Cancel );
	setDefaultButton( Ok );

	KConfigGroup config = KGlobal::config()->group( "DBType" );

	KIconLoader *il = KIconLoader::global();

        m_pageServer = new ServerPrefs( this );
        KPageWidgetItem * page = new KPageWidgetItem( m_pageServer , i18n( "Server Settings" ) );
        page->setObjectName("server" );
	page->setHeader( i18n( "Database Server Options (%1)" , config.readEntry( "Type" )));
	il->loadIcon( "network-workgroup", KIconLoader::NoGroup, 32 );
	page->setIcon( KIcon::KIcon( "network-workgroup", il ) );
        m_helpMap.insert(page->objectName(),"configure-server-settings");
        addPage( page );

        m_pageNumbers = new NumbersPrefs( this );
	page = new KPageWidgetItem(m_pageNumbers , i18n( "Formatting" ) );
        page->setObjectName( "formating" );
	page->setHeader( i18n( "Customize Formatting" ) );
	il->loadIcon( "math_frac", KIconLoader::NoGroup, 32 );
	page->setIcon( KIcon::KIcon( "math_frac", il ) );
	addPage(page);
	m_helpMap.insert(page->objectName(),"custom-formatting");

	m_pageImport = new ImportPrefs( this );
        page = new KPageWidgetItem( m_pageImport , i18n( "Import/Export" ) );
        page->setObjectName( "import" );
	page->setHeader( i18n( "Recipe Import and Export Options" ) );
	il->loadIcon( "go-down", KIconLoader::NoGroup, 32 );
	page->setIcon( KIcon::KIcon( "go-down", il ) );
	addPage(page);
	m_helpMap.insert(page->objectName(),"import-export-preference");


	m_pagePerformance = new PerformancePrefs( this );
        page = new KPageWidgetItem( m_pagePerformance , i18n( "Performance" ) );
	page->setHeader( i18n( "Performance Options" ) );
        page->setObjectName( "performance" );
	il->loadIcon( "preferences-system-performance", KIconLoader::NoGroup, 32 );
	page->setIcon( KIcon::KIcon( "preferences-system-performance", il ) );
	addPage(page);
	m_helpMap.insert(page->objectName(),"configure-performance");


	m_pageSpellChecking = new SpellCheckingPrefs( this );
        page = new KPageWidgetItem( m_pageSpellChecking , i18n( "Spell checking" ) );
	page->setHeader( i18n( "Spell checking Options" ) );
        page->setObjectName( "spellchecking" );
	il->loadIcon( "tools-check-spelling", KIconLoader::NoGroup, 32 );
	page->setIcon( KIcon::KIcon( "tools-check-spelling", il ) );
	addPage(page);
	m_helpMap.insert(page->objectName() ,"configure-spell");


	// Signals & Slots
	connect ( this, SIGNAL( okClicked() ), this, SLOT( saveSettings() ) );
        connect ( this, SIGNAL( helpClicked() ), this, SLOT( slotHelp() ) );
}

void KrecipesPreferences::saveSettings( void )
{
	m_pageServer->saveOptions();
	m_pageNumbers->saveOptions();
	m_pageImport->saveOptions();
	m_pagePerformance->saveOptions();
        m_pageSpellChecking->saveOptions();
}


void KrecipesPreferences::slotHelp()
{
    KToolInvocation::invokeHelp( m_helpMap.value( currentPage()->name() ) );
}


MySQLServerPrefs::MySQLServerPrefs( QWidget *parent ) : QWidget( parent )
{
	QGridLayout * layout = new QGridLayout( this );
	layout->cellRect( 1,1 );
	layout->setSpacing( KDialog::spacingHint() );
	layout->setMargin( 0 );

	QSpacerItem* spacerTop = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerTop, 0, 1 );
	QSpacerItem* spacerLeft = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacerLeft, 1, 0 );

	QLabel* serverText = new QLabel( i18n( "Server:" ), this );
	layout->addWidget( serverText, 1, 1 );

	serverEdit = new KLineEdit( this );
	layout->addWidget( serverEdit, 1, 2 );

	QSpacerItem* spacerRow1 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerRow1, 2, 1 );

	QLabel* usernameText = new QLabel( i18n( "Username:" ), this );
	layout->addWidget( usernameText, 3, 1 );

	usernameEdit = new KLineEdit( this );
	layout->addWidget( usernameEdit, 3, 2 );

	QSpacerItem* spacerRow2 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerRow2, 4, 1 );

	QLabel* passwordText = new QLabel( i18n( "Password:" ), this );
	layout->addWidget( passwordText, 5, 1 );

	passwordEdit = new KLineEdit( this );
	passwordEdit->setEchoMode( KLineEdit::Password );
	layout->addWidget( passwordEdit, 5, 2 );

	QSpacerItem* spacerRow3 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerRow3, 6, 1 );

	QLabel* portText = new QLabel( i18n( "Port:" ), this );
	layout->addWidget( portText, 7, 1 );

	portEdit = new KIntNumInput( this );
	portEdit->setMinimum(0);
	portEdit->setSpecialValueText( i18n("Default") );
	layout->addWidget( portEdit, 7, 2 );

	QSpacerItem* spacerRow4 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerRow4, 8, 1 );

	QLabel* dbNameText = new QLabel( i18n( "Database name:" ), this );
	layout->addWidget( dbNameText, 9, 1 );

	dbNameEdit = new KLineEdit( this );
	layout->addWidget( dbNameEdit, 9, 2 );

	QSpacerItem* spacerRow5 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
	layout->addItem( spacerRow5, 10, 1 );

	// Backup options
	QGroupBox *backupGBox = new QGroupBox( this );
	backupGBox->setTitle( i18n( "Backup" ) );

	QLabel *dumpPathLabel = new QLabel;
	dumpPathLabel->setText( i18n( "Path to '%1':" ,QString("mysqldump") ));

	QLabel *mysqlPathLabel = new QLabel;
	mysqlPathLabel->setText( i18n( "Path to '%1':" ,QString("mysql") ));

	QSpacerItem* spacerRow6 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
	layout->addItem( spacerRow6, 11, 1 );
	QSpacerItem* spacerRight = new QSpacerItem( 10, 10, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
	layout->addItem( spacerRight, 1, 4 );
	
	dumpPathRequester = new KUrlRequester;
	mysqlPathRequester = new KUrlRequester;

	QGridLayout *backupGBoxLayout = new QGridLayout;
	backupGBoxLayout->addWidget( dumpPathLabel, 0, 0 );
	backupGBoxLayout->addWidget( mysqlPathLabel, 0, 1 );
	backupGBoxLayout->addWidget( dumpPathRequester, 1, 0 );
	backupGBoxLayout->addWidget( mysqlPathRequester, 1, 1 );
	backupGBox->setLayout(backupGBoxLayout);
	layout->addWidget( backupGBox, 10, 1, 1, 4 );

	// Load Current Settings
	KConfigGroup config = KGlobal::config()->group( "Server" );
	serverEdit->setText( config.readEntry( "Host", "localhost" ) );
	usernameEdit->setText( config.readEntry( "Username", "" ) );
	passwordEdit->setText( config.readEntry( "Password", "" ) );
	portEdit->setValue( config.readEntry( "Port", 0 ) );
	dbNameEdit->setText( config.readEntry( "DBName", "Krecipes" ) );
	dumpPathRequester->setUrl( config.readEntry( "MySQLDumpPath", "mysqldump" ) );
	dumpPathRequester->setFilter( "mysqldump" );
	mysqlPathRequester->setUrl( config.readEntry( "MySQLPath", "mysql" ) );
	mysqlPathRequester->setFilter( "mysql" );
}

void MySQLServerPrefs::saveOptions( void )
{
	// Save Current Settings
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
	QGridLayout * layout = new QGridLayout( this );
	layout->cellRect( 1,1 );
	layout->setSpacing( KDialog::spacingHint() );
	layout->setMargin( 0 );

	QSpacerItem* spacerTop = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerTop, 0, 1 );
	QSpacerItem* spacerLeft = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacerLeft, 1, 0 );

	QLabel* serverText = new QLabel( i18n( "Server:" ), this );
	layout->addWidget( serverText, 1, 1 );

	serverEdit = new KLineEdit( this );
	layout->addWidget( serverEdit, 1, 2 );

	QSpacerItem* spacerRow1 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerRow1, 2, 1 );

	QLabel* usernameText = new QLabel( i18n( "Username:" ), this );
	layout->addWidget( usernameText, 3, 1 );

	usernameEdit = new KLineEdit( this );
	layout->addWidget( usernameEdit, 3, 2 );

	QSpacerItem* spacerRow2 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerRow2, 4, 1 );

	QLabel* passwordText = new QLabel( i18n( "Password:" ), this );
	layout->addWidget( passwordText, 5, 1 );

	passwordEdit = new KLineEdit( this );
	passwordEdit->setEchoMode( KLineEdit::Password );
	layout->addWidget( passwordEdit, 5, 2 );

	QSpacerItem* spacerRow3 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerRow3, 6, 1 );

	QLabel* portText = new QLabel( i18n( "Port:" ), this );
	layout->addWidget( portText, 7, 1 );

	portEdit = new KIntNumInput( this );
	portEdit->setMinimum(0);
	portEdit->setSpecialValueText( i18n("Default") );
	layout->addWidget( portEdit, 7, 2 );

	QSpacerItem* spacerRow4 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerRow4, 8, 1 );

	QLabel* dbNameText = new QLabel( i18n( "Database name:" ), this );
	layout->addWidget( dbNameText, 9, 1 );

	dbNameEdit = new KLineEdit( this );
	layout->addWidget( dbNameEdit, 9, 2 );

	QSpacerItem* spacerRow5 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
	layout->addItem( spacerRow5, 10, 1 );

	// Backup options
	QGroupBox *backupGBox = new QGroupBox( this );
	QGridLayout *backupGBoxLayout = new QGridLayout;
	backupGBox->setTitle( i18n( "Backup" ) );

	QLabel *dumpPathLabel = new QLabel( backupGBox );
	dumpPathLabel->setText( i18n( "Path to '%1':" ,QString("pg_dump") ));

	QLabel *psqlPathLabel = new QLabel( backupGBox );
	psqlPathLabel->setText( i18n( "Path to '%1':" ,QString("psql") ));

	dumpPathRequester = new KUrlRequester;
	psqlPathRequester = new KUrlRequester;
	
	backupGBox->setLayout( backupGBoxLayout );
	backupGBoxLayout->addWidget( dumpPathLabel, 0, 0 );
	backupGBoxLayout->addWidget( psqlPathLabel, 0, 1 ); 
	backupGBoxLayout->addWidget( dumpPathRequester, 1, 0 );
	backupGBoxLayout->addWidget( psqlPathRequester, 1, 1 );
	layout->addWidget( backupGBox, 10, 1, 1, 4 );

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
	dumpPathRequester->setUrl( config.readEntry( "PgDumpPath", "pg_dump" ) );
	dumpPathRequester->setFilter( "pg_dump" );
	psqlPathRequester->setUrl( config.readEntry( "PsqlPath", "psql" ) );
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
	QVBoxLayout * Form1Layout = new QVBoxLayout( this );

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
	QGroupBox *backupGBox = new QGroupBox( this );
	backupGBox->setTitle( i18n( "Backup" ) );

	QLabel *dumpPathLabel = new QLabel( backupGBox );
	dumpPathLabel->setText( i18n( "Path to '%1':" ,sqliteBinary));
	
	dumpPathRequester = new KUrlRequester;
	
	QGridLayout *backupGBoxLayout = new QGridLayout;
	backupGBox->setLayout( backupGBoxLayout );
	backupGBoxLayout->addWidget( dumpPathLabel, 0, 0 );
	backupGBoxLayout->addWidget( dumpPathRequester, 0, 1 );
	Form1Layout->addWidget( backupGBox );

	// Load & Save Current Settings
	KConfigGroup config = KGlobal::config()->group( "Server" );
	fileRequester = new KUrlRequester( config.readEntry( "DBFile", KStandardDirs::locateLocal( "appdata", "krecipes.krecdb" ) ), hbox );
	fileRequester->fileDialog()->setCaption( i18n( "Select SQLite database file" ) );
	hbox->setStretchFactor( fileRequester, 2 );
	dumpPathRequester->setUrl( config.readEntry( "SQLitePath", sqliteBinary ) );
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
	QVBoxLayout * Form1Layout = new QVBoxLayout( this );
	Form1Layout->setMargin( 11 );
	Form1Layout->setSpacing( 6 );

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
	Form1Layout = new QVBoxLayout( this );
	Form1Layout->setMargin( 11 );
	Form1Layout->setSpacing( 6 );

	numberButtonGroup = new QGroupBox( this );
	numberButtonGroupLayout = new QVBoxLayout;
	numberButtonGroup->setLayout( numberButtonGroupLayout );

	fractionRadioButton = new QRadioButton( numberButtonGroup );
	numberButtonGroupLayout->addWidget( fractionRadioButton );

	decimalRadioButton = new QRadioButton( numberButtonGroup );
	numberButtonGroupLayout->addWidget( decimalRadioButton );
	Form1Layout->addWidget( numberButtonGroup );

	//unit display format
	QGroupBox *abbrevGrpBox = new QGroupBox;
	abbrevGrpBox->setTitle( i18n( "Units" ) );
	abbrevButton = new QCheckBox( i18n( "Use abbreviations" ) );
	QGridLayout *abbrevLayout = new QGridLayout;
	abbrevLayout->addWidget( abbrevButton );
	abbrevGrpBox->setLayout( abbrevLayout );
	Form1Layout->addWidget( abbrevGrpBox );


	Form1Layout->addItem( new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

	adjustSize();

	languageChange();

	// Load Current Settings
	KConfigGroup config = KGlobal::config()->group( "Formatting" );

	if ( config.readEntry( "Fraction", true ) )
		fractionRadioButton->click();
	else
		decimalRadioButton->click();

	abbrevButton->setChecked( config.readEntry( "AbbreviateUnits", false ) );
}

void NumbersPrefs::saveOptions()
{
	KConfigGroup config = KGlobal::config()->group( "Formatting" );

	bool fraction = fractionRadioButton->isChecked();
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

	Form1Layout = new QVBoxLayout( this );
	Form1Layout->setMargin( 11 );
	Form1Layout->setSpacing( 6 );

	QGroupBox *importGroup = new QGroupBox;
	QVBoxLayout *importGroupLayout = new QVBoxLayout;
	importGroup->setLayout( importGroupLayout );
	importGroup->setTitle( i18n( "Import" ) );

	overwriteCheckbox = new QCheckBox( i18n( "Overwrite recipes with same title" ), importGroup );
	overwriteCheckbox->setChecked( overwrite );
	overwriteCheckbox->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	importGroupLayout->addWidget( overwriteCheckbox );

	directImportCheckbox = new QCheckBox( i18n( "Ask which recipes to import" ), importGroup );
	directImportCheckbox->setChecked( !direct );
	directImportCheckbox->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	importGroupLayout->addWidget( directImportCheckbox );

	Form1Layout->addWidget(importGroup);

	QGroupBox *exportGroup = new QGroupBox;
	QGridLayout *exportGroupLayout = new QGridLayout;
	exportGroup->setLayout( exportGroupLayout );
	exportGroup->setTitle( i18n( "Export" ) );

	QLabel *clipboardLabel = new QLabel(i18n("'Copy to Clipboard' format:") );
	exportGroupLayout->addWidget( clipboardLabel, 0, 0 );
	clipBoardFormatComboBox = new KComboBox;
	clipBoardFormatComboBox->insertItem( clipBoardFormatComboBox->count(), QString("%3 (*.txt)").arg(i18n("Plain Text")));
	clipBoardFormatComboBox->insertItem( clipBoardFormatComboBox->count(), "Krecipes (*.kreml)");
	clipBoardFormatComboBox->insertItem( clipBoardFormatComboBox->count(), "Meal-Master (*.mmf)");
	clipBoardFormatComboBox->insertItem( clipBoardFormatComboBox->count(), "Rezkonv (*.rk)");
	clipBoardFormatComboBox->insertItem( clipBoardFormatComboBox->count(), "RecipeML (*.xml)");
	//clipBoardFormatComboBox->insertItem("CookML (*.cml)");
	exportGroupLayout->addWidget( clipBoardFormatComboBox, 0, 1 ); 

	config = KGlobal::config()->group( "Export" );
	QString clipboardFormat = config.readEntry("ClipboardFormat");
	if ( clipboardFormat == "*.kreml" )
		clipBoardFormatComboBox->setCurrentIndex(1);
	else if ( clipboardFormat == "*.mmf" )
		clipBoardFormatComboBox->setCurrentIndex(2);
	else if ( clipboardFormat == "*.xml" )
		clipBoardFormatComboBox->setCurrentIndex(3);
	else
		clipBoardFormatComboBox->setCurrentIndex(0);

	Form1Layout->addWidget(exportGroup);

	Form1Layout->addItem( new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

	directImportCheckbox->setWhatsThis(
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
	QString ext = clipBoardFormatComboBox->currentText().mid(clipBoardFormatComboBox->currentText().indexOf("(")+1,clipBoardFormatComboBox->currentText().length()-clipBoardFormatComboBox->currentText().indexOf("(")-2);
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

	Form1Layout = new QVBoxLayout( this );
	Form1Layout->setMargin( 11 );
	Form1Layout->setSpacing( 6 );

	searchAsYouTypeBox = new QCheckBox( i18n( "Search as you type" ), this );
	searchAsYouTypeBox->setChecked( config.readEntry( "SearchAsYouType", true ) );

	QLabel *explainationLabel = new QLabel( i18n("In most instances these options do not need to be changed.  However, limiting the amount of items displayed at once will <b>allow Krecipes to better perform when the database is loaded with many thousands of recipes</b>."), this );
	explainationLabel->setTextFormat( Qt::RichText );
        explainationLabel->setWordWrap( true );

	KHBox *catLimitHBox = new KHBox( this );
	catLimitInput = new KIntNumInput(catLimitHBox);
	catLimitInput->setLabel( i18n( "Number of categories to display at once:" ) );
	catLimitInput->setRange(0,5000,20);
	catLimitInput->setSliderEnabled( true );
	catLimitInput->setSpecialValueText( i18n("Unlimited") );

	if ( cat_limit > 0 )
		catLimitInput->setValue( cat_limit );

	KHBox *limitHBox = new KHBox( this );
	limitInput = new KIntNumInput(limitHBox);
	limitInput->setLabel( i18n( "Number of elements to display at once:" ) );
	limitInput->setRange(0,100000,1000);
	limitInput->setSliderEnabled( true );
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

//=============Sonnet Options Dialog================//
SpellCheckingPrefs::SpellCheckingPrefs( QWidget *parent )
    : QWidget( parent )
{
    QHBoxLayout *lay = new QHBoxLayout( this );
    m_confPage = new Sonnet::ConfigWidget(&( *KGlobal::config() ), this );
    lay->addWidget( m_confPage );
    setLayout( lay );
}

void SpellCheckingPrefs::saveOptions()
{
    m_confPage->save();
}

#include "pref.moc"
