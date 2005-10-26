/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   Copyright (C) 2004-2005 by                                            *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "pref.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qlayout.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qframe.h>
#include <qcombobox.h>

#include <kapplication.h>
#include <kconfig.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kfiledialog.h>
#include <knuminput.h>
#include <klineedit.h>
#include <kurlrequester.h>

KrecipesPreferences::KrecipesPreferences( QWidget *parent )
		: KDialogBase( IconList, i18n( "Krecipes Preferences" ),
		               Help | Ok | Cancel, Ok, parent )
{
	// this is the base class for your preferences dialog.  it is now
	// a TreeList dialog.. but there are a number of other
	// possibilities (including Tab, Swallow, and just Plain)
	QFrame * frame;

	KConfig *config = KGlobal::config();
	config->setGroup( "DBType" );

	KIconLoader il;
	frame = addPage( i18n( "Server Settings" ),
	                 QString( i18n( "Database Server Options (%1)" ) ).arg( config->readEntry( "Type" ) ),
	                 il.loadIcon( "network_local", KIcon::NoGroup, 32 ) );
	QHBoxLayout* layout = new QHBoxLayout( frame );
	m_pageServer = new ServerPrefs( frame );
	layout->addWidget( m_pageServer );

	frame = addPage( i18n( "Formatting" ), i18n( "Customize Formatting" ), il.loadIcon( "math_frac", KIcon::NoGroup, 32 ) );
	QHBoxLayout* formatting_layout = new QHBoxLayout( frame );
	m_pageNumbers = new NumbersPrefs( frame );
	formatting_layout->addWidget( m_pageNumbers );

	frame = addPage( i18n( "Import/Export" ), i18n( "Recipe Import and Export Options" ), il.loadIcon( "down", KIcon::NoGroup, 32 ) );
	QHBoxLayout* import_layout = new QHBoxLayout( frame );
	m_pageImport = new ImportPrefs( frame );
	import_layout->addWidget( m_pageImport );

	frame = addPage( i18n( "Performance" ), i18n( "Performance Options" ), il.loadIcon( "launch", KIcon::NoGroup, 32 ) );
	QHBoxLayout* performance_layout = new QHBoxLayout( frame );
	m_pagePerformance = new PerformancePrefs( frame );
	performance_layout->addWidget( m_pagePerformance );

	//setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

	// Signals & Slots
	connect ( this, SIGNAL( okClicked() ), this, SLOT( saveSettings() ) );

}


MySQLServerPrefs::MySQLServerPrefs( QWidget *parent ) : QWidget( parent )
{
	QGridLayout * layout = new QGridLayout( this, 1, 1, 0, 0 );
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

	QLabel* dbNameText = new QLabel( i18n( "Database name:" ), this );
	dbNameText->setFixedSize( QSize( 100, 20 ) );
	dbNameText->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( dbNameText, 7, 1 );

	dbNameEdit = new KLineEdit( this );
	dbNameEdit->setFixedSize( QSize( 120, 20 ) );
	dbNameEdit->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( dbNameEdit, 7, 2 );

	QSpacerItem* spacerRow5 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
	layout->addItem( spacerRow5, 8, 1 );

	// Backup options
	QGroupBox *backupGBox = new QGroupBox( this, "backupGBox" );
	backupGBox->setTitle( i18n( "Backup" ) );
	backupGBox->setColumns( 2 );
	layout->addMultiCellWidget( backupGBox, 9, 9, 1, 4 );

	QLabel *dumpPathLabel = new QLabel( backupGBox );
	dumpPathLabel->setText( QString(i18n( "Path to '%1':" )).arg("mysqldump") );
	dumpPathRequester = new KURLRequester( backupGBox );
	dumpPathRequester->setFilter( "mysqldump" );

	QLabel *mysqlPathLabel = new QLabel( backupGBox );
	mysqlPathLabel->setText( QString(i18n( "Path to '%1':" )).arg("mysql") );
	mysqlPathRequester = new KURLRequester( backupGBox );
	mysqlPathRequester->setFilter( "mysql" );


	QSpacerItem* spacerRow4 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
	layout->addItem( spacerRow4, 10, 1 );
	QSpacerItem* spacerRight = new QSpacerItem( 10, 10, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
	layout->addItem( spacerRight, 1, 4 );

	// Load & Save Current Settings
	KConfig *config = kapp->config();
	config->setGroup( "Server" );
	serverEdit->setText( config->readEntry( "Host", "localhost" ) );
	usernameEdit->setText( config->readEntry( "Username", "" ) );
	passwordEdit->setText( config->readEntry( "Password", "" ) );
	dbNameEdit->setText( config->readEntry( "DBName", "Krecipes" ) );
	dumpPathRequester->setURL( config->readEntry( "MySQLDumpPath", "mysqldump" ) );
	mysqlPathRequester->setURL( config->readEntry( "MySQLPath", "mysql" ) );
}

void MySQLServerPrefs::saveOptions( void )
{
	KConfig * config = kapp->config();
	config->setGroup( "Server" );
	config->writeEntry( "Host", serverEdit->text() );
	config->writeEntry( "Username", usernameEdit->text() );
	config->writeEntry( "Password", passwordEdit->text() );
	config->writeEntry( "DBName", dbNameEdit->text() );
	config->writeEntry( "MySQLDumpPath", dumpPathRequester->url() );
	config->writeEntry( "MySQLPath", mysqlPathRequester->url() );
}


PostgreSQLServerPrefs::PostgreSQLServerPrefs( QWidget *parent ) : QWidget( parent )
{
	QGridLayout * layout = new QGridLayout( this, 1, 1, 0, 0 );
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

	QLabel* dbNameText = new QLabel( i18n( "Database name:" ), this );
	dbNameText->setFixedSize( QSize( 100, 20 ) );
	dbNameText->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( dbNameText, 7, 1 );

	dbNameEdit = new KLineEdit( this );
	dbNameEdit->setFixedSize( QSize( 120, 20 ) );
	dbNameEdit->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( dbNameEdit, 7, 2 );


	QSpacerItem* spacerRow5 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
	layout->addItem( spacerRow5, 8, 1 );

	// Backup options
	QGroupBox *backupGBox = new QGroupBox( this, "backupGBox" );
	backupGBox->setTitle( i18n( "Backup" ) );
	backupGBox->setColumns( 2 );
	layout->addMultiCellWidget( backupGBox, 9, 9, 1, 4 );

	QLabel *dumpPathLabel = new QLabel( backupGBox );
	dumpPathLabel->setText( QString(i18n( "Path to '%1':" )).arg("pg_dump") );
	dumpPathRequester = new KURLRequester( backupGBox );
	dumpPathRequester->setFilter( "pg_dump" );

	QLabel *psqlPathLabel = new QLabel( backupGBox );
	psqlPathLabel->setText( QString(i18n( "Path to '%1':" )).arg("psql") );
	psqlPathRequester = new KURLRequester( backupGBox );
	psqlPathRequester->setFilter( "psql" );


	QSpacerItem* spacerRow4 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
	layout->addItem( spacerRow4, 10, 1 );
	QSpacerItem* spacerRight = new QSpacerItem( 10, 10, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
	layout->addItem( spacerRight, 1, 4 );

	// Load & Save Current Settings
	KConfig *config = kapp->config();
	config->setGroup( "Server" );
	serverEdit->setText( config->readEntry( "Host", "localhost" ) );
	usernameEdit->setText( config->readEntry( "Username", "" ) );
	passwordEdit->setText( config->readEntry( "Password", "" ) );
	dbNameEdit->setText( config->readEntry( "DBName", "Krecipes" ) );
	dumpPathRequester->setURL( config->readEntry( "PgDumpPath", "pg_dump" ) );
	psqlPathRequester->setURL( config->readEntry( "PsqlPath", "psql" ) );
}

void PostgreSQLServerPrefs::saveOptions( void )
{
	KConfig * config = kapp->config();
	config->setGroup( "Server" );
	config->writeEntry( "Host", serverEdit->text() );
	config->writeEntry( "Username", usernameEdit->text() );
	config->writeEntry( "Password", passwordEdit->text() );
	config->writeEntry( "DBName", dbNameEdit->text() );
	config->writeEntry( "PgDumpPath", dumpPathRequester->url() );
	config->writeEntry( "PsqlPath", psqlPathRequester->url() );
}



SQLiteServerPrefs::SQLiteServerPrefs( QWidget *parent ) : QWidget( parent )
{
	QVBoxLayout * Form1Layout = new QVBoxLayout( this );

	QHBox *hbox = new QHBox( this );
	( void ) new QLabel( i18n( "Database file:" ), hbox );

	fileRequester = new KURLRequester( hbox );
	hbox->setStretchFactor( fileRequester, 2 );

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
	QGroupBox *backupGBox = new QGroupBox( this, "backupGBox" );
	backupGBox->setTitle( i18n( "Backup" ) );
	backupGBox->setColumns( 2 );
	Form1Layout->addWidget( backupGBox );

	QLabel *dumpPathLabel = new QLabel( backupGBox );
	dumpPathLabel->setText( QString(i18n( "Path to '%1':" )).arg(sqliteBinary) );
	dumpPathRequester = new KURLRequester( backupGBox );
	dumpPathRequester->setFilter( sqliteBinary );

	// Load & Save Current Settings
	KConfig *config = kapp->config();
	config->setGroup( "Server" );
	fileRequester->setURL( config->readEntry( "DBFile", locateLocal( "appdata", "krecipes.krecdb" ) ) );
	dumpPathRequester->setURL( config->readEntry( "SQLitePath", sqliteBinary ) );
}

void SQLiteServerPrefs::saveOptions( void )
{
	KConfig * config = kapp->config();
	config->setGroup( "Server" );
	config->writeEntry( "DBFile", fileRequester->url() );
	config->writeEntry( "SQLitePath", dumpPathRequester->url() );
}


// Server Setttings Dialog
ServerPrefs::ServerPrefs( QWidget *parent )
		: QWidget( parent )
{
	QVBoxLayout * Form1Layout = new QVBoxLayout( this, 11, 6 );

	KConfig *config = kapp->config();
	config->setGroup( "DBType" );
	QString DBtype = config->readEntry( "Type" );
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
	KConfig * config = kapp->config();
	config->setGroup( "DBType" );
	QString DBtype = config->readEntry( "Type" );
	if ( DBtype == "MySQL" )
		( ( MySQLServerPrefs* ) serverWidget ) ->saveOptions();
	else if ( DBtype == "PostgreSQL" )
		( ( PostgreSQLServerPrefs* ) serverWidget ) ->saveOptions();
	else
		( ( SQLiteServerPrefs* ) serverWidget ) ->saveOptions();

	if ( wizard_button->isChecked() ) {
		config->setGroup( "Wizard" );
		config->writeEntry( "SystemSetup", false );
	}
}

//=============Numbers Preferences Dialog================//
NumbersPrefs::NumbersPrefs( QWidget *parent )
		: QWidget( parent )
{
	Form1Layout = new QVBoxLayout( this, 11, 6 );

	numberButtonGroup = new QButtonGroup( this );
	numberButtonGroup->setColumnLayout( 0, Qt::Vertical );
	numberButtonGroup->layout() ->setSpacing( 6 );
	numberButtonGroup->layout() ->setMargin( 11 );
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
	QGroupBox *ingredientGrpBox = new QGroupBox( 2, Qt::Vertical, i18n( "Ingredient" ), this );

	QHBox *ingredientBox = new QHBox( ingredientGrpBox );
	( void ) new QLabel( i18n( "Ingredient Format:" ), ingredientBox );
	ingredientEdit = new KLineEdit( ingredientBox );
	( void ) new QLabel( i18n( "%n: Name<br>"
	                           "%p: Preparation method<br>"
	                           "%a: Amount<br>"
	                           "%u: Unit"
	                         ), ingredientGrpBox );

	Form1Layout->addWidget( ingredientGrpBox );

	Form1Layout->addItem( new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

	adjustSize();

	languageChange();

	// Load Current Settings
	KConfig *config = kapp->config();
	config->setGroup( "Formatting" );

	int button = ( config->readBoolEntry( "Fraction", false ) ) ? 1 : 0;
	numberButtonGroup->setButton( button );

	ingredientEdit->setText( config->readEntry( "Ingredient", "%n%p: %a %u" ) );
}

void NumbersPrefs::saveOptions()
{
	KConfig * config = kapp->config();
	config->setGroup( "Formatting" );

	bool fraction = !numberButtonGroup->find( 0 ) ->isOn();
	config->writeEntry( "Fraction", fraction );

	config->writeEntry( "Ingredient", ingredientEdit->text() );
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
	KConfig * config = kapp->config();
	config->setGroup( "Import" );

	bool overwrite = config->readBoolEntry( "OverwriteExisting", false );
	bool direct = config->readBoolEntry( "DirectImport", false );

	Form1Layout = new QVBoxLayout( this, 11, 6 );

	QGroupBox *importGroup = new QGroupBox(2,Qt::Vertical,i18n("Import"), this);

	overwriteCheckbox = new QCheckBox( i18n( "Overwrite recipes with same title" ), importGroup );
	overwriteCheckbox->setChecked( overwrite );
	overwriteCheckbox->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );

	directImportCheckbox = new QCheckBox( i18n( "Ask which recipes to import" ), importGroup );
	directImportCheckbox->setChecked( !direct );
	directImportCheckbox->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );

	Form1Layout->addWidget(importGroup);

	QGroupBox *exportGroup = new QGroupBox(1,Qt::Vertical,i18n("Export"), this);

	QHBox *clipboardHBox = new QHBox(exportGroup);
	clipboardHBox->setSpacing(6);
	QLabel *clipboardLabel = new QLabel(i18n("'Copy to Clipboard' format:"),clipboardHBox);
	clipBoardFormatComboBox = new QComboBox( clipboardHBox );
	clipBoardFormatComboBox->insertItem(QString("%3 (*.txt)").arg(i18n("Plain Text")));
	clipBoardFormatComboBox->insertItem("Krecipes (*.kreml)");
	clipBoardFormatComboBox->insertItem("Meal-Master (*.mmf)");
	clipBoardFormatComboBox->insertItem("Rezkonv (*.rk)");
	clipBoardFormatComboBox->insertItem("RecipeML (*.xml)");
	//clipBoardFormatComboBox->insertItem("CookML (*.cml)");
	clipboardHBox->setStretchFactor(clipBoardFormatComboBox,1);

	config->setGroup( "Export" );
	QString clipboardFormat = config->readEntry("ClipboardFormat");
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

	QWhatsThis::add( directImportCheckbox, 
		i18n("When this is enabled, the importer will show every recipe in the file(s) and allow you to select which recipes you want imported.\n \
							\
		Disable this to always import every recipe, which allows for faster and less memory-intensive imports.")
	);

	adjustSize();
}

void ImportPrefs::saveOptions()
{
	KConfig * config = kapp->config();
	config->setGroup( "Import" );

	config->writeEntry( "OverwriteExisting", overwriteCheckbox->isChecked() );
	config->writeEntry( "DirectImport", !directImportCheckbox->isChecked() );

	config->setGroup( "Export" );
	QString ext = clipBoardFormatComboBox->currentText().mid(clipBoardFormatComboBox->currentText().find("(")+1,clipBoardFormatComboBox->currentText().length()-clipBoardFormatComboBox->currentText().find("(")-2);
	config->writeEntry( "ClipboardFormat", ext );
}


//=============Performance Options Dialog================//
PerformancePrefs::PerformancePrefs( QWidget *parent )
		: QWidget( parent )
{
	// Load Current Settings
	KConfig * config = kapp->config();
	config->setGroup( "Performance" );

	int cat_limit = config->readNumEntry( "CategoryLimit", -1 );
	int limit = config->readNumEntry( "Limit", -1 );

	Form1Layout = new QVBoxLayout( this, 11, 6 );

	QLabel *explainationLabel = new QLabel( i18n("In most instances these options do not need to be changed.  However, limiting the amount of items displayed at once will <b>allow Krecipes to better perform when the database is loaded with many thousands of recipes</b>."), this );
	explainationLabel->setTextFormat( Qt::RichText );

	QHBox *catLimitHBox = new QHBox( this );
	catLimitInput = new KIntNumInput(catLimitHBox);
	catLimitInput->setLabel( i18n( "Number of categories to display at once:" ) );
	catLimitInput->setRange(0,5000,20,true);
	catLimitInput->setSpecialValueText( i18n("Unlimited") );

	if ( cat_limit > 0 )
		catLimitInput->setValue( cat_limit );

	QHBox *limitHBox = new QHBox( this );
	limitInput = new KIntNumInput(limitHBox);
	limitInput->setLabel( i18n( "Number of elements to display at once:" ) );
	limitInput->setRange(0,100000,1000,true);
	limitInput->setSpecialValueText( i18n("Unlimited") );

	if ( limit > 0 )
		limitInput->setValue( limit );

	Form1Layout->addWidget( explainationLabel );
	Form1Layout->addWidget( catLimitHBox );
	Form1Layout->addWidget( limitHBox );

	Form1Layout->addItem( new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

	adjustSize();
}

void PerformancePrefs::saveOptions()
{
	KConfig * config = kapp->config();
	config->setGroup( "Performance" );

	int catLimit = ( catLimitInput->value() == 0 ) ? -1 : catLimitInput->value();
	config->writeEntry( "CategoryLimit", catLimit );

	int limit = ( limitInput->value() == 0 ) ? -1 : limitInput->value();
	config->writeEntry( "Limit", limit );
}

#include "pref.moc"
