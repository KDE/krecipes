/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003, 2004, 2006 Jason Kivlighn <jkivlighn@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "pref.h"

#include <config-krecipes.h>

#include <QLabel>


#include <q3buttongroup.h>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>

#include <qframe.h>
#include <KComboBox>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QFormLayout>

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
#include <KCmdLineArgs>
#include <KAboutData>

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
        KPageWidgetItem * page = new KPageWidgetItem( m_pageServer , i18n( "Database System" ) );
        page->setObjectName("server" );
	page->setHeader( i18n( "Database Management System Options (%1)" , config.readEntry( "Type" )));
	il->loadIcon( "network-server", KIconLoader::NoGroup, 32 );
	page->setIcon( KIcon( "server-database", il ) );
        m_helpMap.insert(page->objectName(),"configure-server-settings");
        addPage( page );

        m_pageNumbers = new NumbersPrefs( this );
	page = new KPageWidgetItem(m_pageNumbers , i18n( "Formatting" ) );
        page->setObjectName( "formating" );
	page->setHeader( i18n( "Customize Formatting" ) );
	il->loadIcon( "format-indent-more", KIconLoader::NoGroup, 32 );
	page->setIcon( KIcon( "format-indent-more", il ) );
	addPage(page);
	m_helpMap.insert(page->objectName(),"custom-formatting");

	m_pageImport = new ImportPrefs( this );
        page = new KPageWidgetItem( m_pageImport , i18n( "Import/Export" ) );
        page->setObjectName( "import" );
	page->setHeader( i18n( "Recipe Import and Export Options" ) );
	il->loadIcon( "go-down", KIconLoader::NoGroup, 32 );
	page->setIcon( KIcon( "go-down", il ) );
	addPage(page);
	m_helpMap.insert(page->objectName(),"import-export-preference");


	m_pagePerformance = new PerformancePrefs( this );
        page = new KPageWidgetItem( m_pagePerformance , i18n( "Performance" ) );
	page->setHeader( i18n( "Performance Options" ) );
        page->setObjectName( "performance" );
	il->loadIcon( "preferences-system-performance", KIconLoader::NoGroup, 32 );
	page->setIcon( KIcon( "preferences-system-performance", il ) );
	addPage(page);
	m_helpMap.insert(page->objectName(),"configure-performance");


	m_pageSpellChecking = new SpellCheckingPrefs( this );
        page = new KPageWidgetItem( m_pageSpellChecking , i18n( "Spell checking" ) );
	page->setHeader( i18n( "Spell checking Options" ) );
        page->setObjectName( "spellchecking" );
	il->loadIcon( "tools-check-spelling", KIconLoader::NoGroup, 32 );
	page->setIcon( KIcon( "tools-check-spelling", il ) );
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
	QVBoxLayout * layout = new QVBoxLayout( this );
	
	QFormLayout * dbLayout = new QFormLayout;

	serverEdit = new KLineEdit( this );
	dbLayout->addRow( i18n( "Server:" ),  serverEdit );

	usernameEdit = new KLineEdit( this );
	dbLayout->addRow( i18nc( "@label:textbox Database Username", "Username:" ), usernameEdit );

	passwordEdit = new KLineEdit( this );
	passwordEdit->setEchoMode( KLineEdit::Password );
	dbLayout->addRow( i18n( "Password:" ), passwordEdit );

	portEdit = new KIntNumInput( this );
	portEdit->setMinimum(0);
	portEdit->setSpecialValueText( i18nc("Default Port", "Default") );
	dbLayout->addRow( i18n( "Port:" ), portEdit );

	dbNameEdit = new KLineEdit( this );
	dbLayout->addRow( i18n( "Database name:" ), dbNameEdit );

	layout->addLayout( dbLayout );

	// Backup options
	QGroupBox *backupGBox = new QGroupBox( this );
	backupGBox->setTitle( i18n( "Backup" ) );
	QFormLayout *backupGBoxLayout = new QFormLayout;

	dumpPathRequester = new KUrlRequester;
	dumpPathRequester->fileDialog()->setCaption( i18n( "Select MySQL dump command" ) );
	backupGBoxLayout->addRow( i18n( "Path to '%1':" ,QString("mysqldump") ), dumpPathRequester );

	mysqlPathRequester = new KUrlRequester;
	mysqlPathRequester->fileDialog()->setCaption( i18n( "Select MySQL command" ) );
	backupGBoxLayout->addRow( i18n( "Path to '%1':" ,QString("mysql") ),mysqlPathRequester );

	backupGBox->setLayout(backupGBoxLayout);

	layout->addWidget( backupGBox );

	// Load Current Settings
	KConfigGroup config = KGlobal::config()->group( "Server" );
	serverEdit->setText( config.readEntry( "Host", "localhost" ) );
	usernameEdit->setText( config.readEntry( "Username", "" ) );
	passwordEdit->setText( config.readEntry( "Password", "" ) );
	portEdit->setValue( config.readEntry( "Port", 0 ) );
	dbNameEdit->setText( config.readEntry( "DBName", "Krecipes" ) );
	dumpPathRequester->setUrl( config.readEntry( "MySQLDumpPath", "mysqldump" ) );
	dumpPathRequester->setFilter( "mysqldump\n*|" + i18n( "All files" ) );
	mysqlPathRequester->setUrl( config.readEntry( "MySQLPath", "mysql" ) );
	mysqlPathRequester->setFilter( "mysql\n*|" + i18n( "All files" ) );
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
	config.writeEntry( "MySQLDumpPath", dumpPathRequester->text() );
	config.writeEntry( "MySQLPath", mysqlPathRequester->text() );
}


PostgreSQLServerPrefs::PostgreSQLServerPrefs( QWidget *parent ) : QWidget( parent )
{
	QVBoxLayout * layout = new QVBoxLayout( this );

	QFormLayout * dbLayout = new QFormLayout;

	serverEdit = new KLineEdit( this );
	dbLayout->addRow( i18n( "Server:" ), serverEdit );

	usernameEdit = new KLineEdit( this );
	dbLayout->addRow ( i18nc("@label:textbox Database Username", "Username:" ), usernameEdit );

	passwordEdit = new KLineEdit( this );
	passwordEdit->setEchoMode( KLineEdit::Password );
	dbLayout->addRow( i18n( "Password:" ), passwordEdit );

	portEdit = new KIntNumInput( this );
	portEdit->setMinimum(0);
	portEdit->setSpecialValueText( i18nc("Default Port", "Default") );
	dbLayout->addRow( i18n( "Port:" ), portEdit );

	dbNameEdit = new KLineEdit( this );
	dbLayout->addRow( i18n( "Database name:" ), dbNameEdit );

	layout->addLayout( dbLayout );

	// Backup options
	QGroupBox *backupGBox = new QGroupBox( this );
	QFormLayout *backupGBoxLayout = new QFormLayout;
	backupGBox->setTitle( i18n( "Backup" ) );

	dumpPathRequester = new KUrlRequester;
	dumpPathRequester->fileDialog()->setCaption( i18n( "Select PostgreSQL dump command" ) );
	backupGBoxLayout->addRow( i18n( "Path to '%1':" ,QString("pg_dump") ), dumpPathRequester );

	psqlPathRequester = new KUrlRequester;
	dumpPathRequester->fileDialog()->setCaption( i18n( "Select PostgreSQL command" ) );
	backupGBoxLayout->addRow( i18n( "Path to '%1':" ,QString("psql") ), psqlPathRequester );

	backupGBox->setLayout( backupGBoxLayout );
	
	layout->addWidget( backupGBox );

	// Load & Save Current Settings
	KConfigGroup config = KGlobal::config()->group( "Server" );
	serverEdit->setText( config.readEntry( "Host", "localhost" ) );
	usernameEdit->setText( config.readEntry( "Username", "" ) );
	passwordEdit->setText( config.readEntry( "Password", "" ) );
	portEdit->setValue( config.readEntry( "Port", 0 ) );
	dbNameEdit->setText( config.readEntry( "DBName", "Krecipes" ) );
	dumpPathRequester->setUrl( config.readEntry( "PgDumpPath", "pg_dump" ) );
	dumpPathRequester->setFilter( "pg_dump\n*|" + i18n( "All files" ) );
	psqlPathRequester->setUrl( config.readEntry( "PsqlPath", "psql" ) );
	psqlPathRequester->setFilter( "psql\n*|" + i18n( "All files" ) );
}

void PostgreSQLServerPrefs::saveOptions( void )
{
	KConfigGroup config = KGlobal::config()->group( "Server" );
	config.writeEntry( "Host", serverEdit->text() );
	config.writeEntry( "Username", usernameEdit->text() );
	config.writeEntry( "Password", passwordEdit->text() );
	config.writeEntry( "Port", portEdit->value() );
	config.writeEntry( "DBName", dbNameEdit->text() );
	config.writeEntry( "PgDumpPath", dumpPathRequester->text() );
	config.writeEntry( "PsqlPath", psqlPathRequester->text() );
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
	
	QFormLayout *backupGBoxLayout = new QFormLayout;
	backupGBox->setLayout( backupGBoxLayout );

	dumpPathRequester = new KUrlRequester;
	dumpPathRequester->fileDialog()->setCaption( i18n( "Select SQLite command" ) );
	backupGBoxLayout->addRow( i18n( "Path to '%1':", sqliteBinary ), dumpPathRequester );
	
	Form1Layout->addWidget( backupGBox );

	// SQLite converter options
	QGroupBox *converterGBox = new QGroupBox( this );
	converterGBox->setTitle( i18n( "SQLite converter" ) );

	QFormLayout *converterGBoxLayout = new QFormLayout;
	converterGBox->setLayout( converterGBoxLayout );

	oldPathRequester = new KUrlRequester;
	oldPathRequester->fileDialog()->setCaption( i18n( "Select old SQLite version command" ) );
	converterGBoxLayout->addRow( i18n( "Path to SQLite old version command:" ), oldPathRequester );
	
	newPathRequester = new KUrlRequester;
	newPathRequester->fileDialog()->setCaption( i18n( "Select new SQLite version command" ) );
	converterGBoxLayout->addRow( i18n( "Path to SQLite new version command:" ), newPathRequester );
	
	Form1Layout->addWidget( converterGBox );


	// Load Current Settings
	KConfigGroup config = KGlobal::config()->group( "Server" );
	fileRequester = new KUrlRequester( config.readEntry( "DBFile", KStandardDirs::locateLocal( "appdata", "krecipes.krecdb" ) ), hbox );
	fileRequester->fileDialog()->setCaption( i18n( "Select SQLite database file" ) );
	hbox->setStretchFactor( fileRequester, 2 );
	dumpPathRequester->setUrl( config.readEntry( "SQLitePath", sqliteBinary ) );
	dumpPathRequester->setFilter( sqliteBinary + "\n*|" + i18n( "All files" ) );
	oldPathRequester->setUrl( config.readEntry( "SQLiteOldVersionPath", "sqlite" ) );
	oldPathRequester->setFilter( "sqlite\n*|" + i18n( "All files" ) );
	newPathRequester->setUrl( config.readEntry( "SQLiteNewVersionPath", "sqlite3" ) );
	newPathRequester->setFilter( "sqlite3\n*|" + i18n( "All files" ) );
}

void SQLiteServerPrefs::saveOptions( void )
{
	KConfigGroup config = KGlobal::config()->group( "Server" );
	config.writeEntry( "DBFile", fileRequester->text() );
	config.writeEntry( "SQLitePath", dumpPathRequester->text() );
	config.writeEntry( "SQLiteOldVersionPath", oldPathRequester->text() );
	config.writeEntry( "SQLiteNewVersionPath", newPathRequester->text() );
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
	serverWidget->setMinimumSize( serverWidget->sizeHint() );
	Form1Layout->addWidget( serverWidget );

	Form1Layout->addItem( new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

	wizard_button = new QCheckBox( i18n( "Re-run wizard on next startup" ), this );
	wizard_button->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	config = KGlobal::config()->group( "Wizard" );
	wizard_button->setChecked( !config.readEntry( "SystemSetup", false ) );
	Form1Layout->addWidget( wizard_button );

	QLabel *note = new QLabel( i18n( "Note: Krecipes must be restarted for most server preferences to take effect." ), this );
	note->setWordWrap( true );
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

	config = KGlobal::config()->group( "Wizard" );
	config.writeEntry( "SystemSetup", !(wizard_button->isChecked()) );
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
	fractionRadioButton->setText( i18nc("@option:radio Display fraction", "Fraction" ) );
	decimalRadioButton->setText( i18nc("@option:radio Display decimal", "Decimal" ) );
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
	clipBoardFormatComboBox->insertItem( clipBoardFormatComboBox->count(), "MasterCook (*.mx2)");
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
	else if ( clipboardFormat == "*.mx2" )
		clipBoardFormatComboBox->setCurrentIndex(4);
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
	catLimitInput->setSpecialValueText( i18nc("Unlimited categories", "Unlimited") );

	if ( cat_limit > 0 )
		catLimitInput->setValue( cat_limit );

	KHBox *limitHBox = new KHBox( this );
	limitInput = new KIntNumInput(limitHBox);
	limitInput->setLabel( i18n( "Number of elements to display at once:" ) );
	limitInput->setRange(0,100000,1000);
	limitInput->setSliderEnabled( true );
	limitInput->setSpecialValueText( i18nc("Unlimited elements", "Unlimited") );

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

	QString spellCheckingConfigFileName = KStandardDirs::locateLocal( "config",
		KCmdLineArgs::aboutData()->appName() + "rc" );

	KConfig localConfig( spellCheckingConfigFileName, KConfig::SimpleConfig );

	//If we don't have our local configuration for spell checking, fall back to
	//user's global configuration.
	if ( !localConfig.hasGroup( "Spelling" ) ) {
		KConfigGroup localGroup( &localConfig, "Spelling" );
		KConfig globalSonnetConfig( KStandardDirs::locateLocal( "config", "sonnetrc" ) );
		KConfigGroup globalGroup( &globalSonnetConfig, "Spelling" );
		globalGroup.copyTo( &localGroup );
		localConfig.sync();
		KConfigGroup group( KGlobal::config(), "Spelling" );
		globalGroup.copyTo( &group );
	}

	m_confPage = new Sonnet::ConfigWidget(&( *KGlobal::config() ), this );
	lay->addWidget( m_confPage );
	setLayout( lay );
}

void SpellCheckingPrefs::saveOptions()
{
	m_confPage->save();
}

#include "pref.moc"
