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

#include <qlayout.h>
#include <qlabel.h>
#include <q3hbox.h>
#include <q3buttongroup.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
#include <q3frame.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <kapplication.h>
#include <kconfig.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kfiledialog.h>
#include <knuminput.h>
#include <klineedit.h>


KrecipesPreferences::KrecipesPreferences( QWidget *parent )
		: KDialogBase( IconList, i18n( "Krecipes Preferences" ),
		               Help | Ok | Cancel, Ok, parent )
{
	// this is the base class for your preferences dialog.  it is now
	// a TreeList dialog.. but there are a number of other
	// possibilities (including Tab, Swallow, and just Plain)
	Q3Frame * frame;

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

	frame = addPage( i18n( "Import" ), i18n( "Recipe Import Options" ), il.loadIcon( "down", KIcon::NoGroup, 32 ) );
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

	QSpacerItem* spacerRow4 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
	layout->addItem( spacerRow4, 8, 1 );
	QSpacerItem* spacerRight = new QSpacerItem( 10, 10, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
	layout->addItem( spacerRight, 1, 4 );

	// Load & Save Current Settings
	KConfig *config = kapp->config();
	config->setGroup( "Server" );
	serverEdit->setText( config->readEntry( "Host", "localhost" ) );
	usernameEdit->setText( config->readEntry( "Username", "" ) );
	passwordEdit->setText( config->readEntry( "Password", "" ) );
	dbNameEdit->setText( config->readEntry( "DBName", "Krecipes" ) );
}

void MySQLServerPrefs::saveOptions( void )
{
	KConfig * config = kapp->config();
	config->setGroup( "Server" );
	config->writeEntry( "Host", serverEdit->text() );
	config->writeEntry( "Username", usernameEdit->text() );
	config->writeEntry( "Password", passwordEdit->text() );
	config->writeEntry( "DBName", dbNameEdit->text() );
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

	QSpacerItem* spacerRow4 = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
	layout->addItem( spacerRow4, 8, 1 );
	QSpacerItem* spacerRight = new QSpacerItem( 10, 10, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
	layout->addItem( spacerRight, 1, 4 );

	// Load & Save Current Settings
	KConfig *config = kapp->config();
	config->setGroup( "Server" );
	serverEdit->setText( config->readEntry( "Host", "localhost" ) );
	usernameEdit->setText( config->readEntry( "Username", "" ) );
	passwordEdit->setText( config->readEntry( "Password", "" ) );
	dbNameEdit->setText( config->readEntry( "DBName", "Krecipes" ) );
}

void PostgreSQLServerPrefs::saveOptions( void )
{
	KConfig * config = kapp->config();
	config->setGroup( "Server" );
	config->writeEntry( "Host", serverEdit->text() );
	config->writeEntry( "Username", usernameEdit->text() );
	config->writeEntry( "Password", passwordEdit->text() );
	config->writeEntry( "DBName", dbNameEdit->text() );
}



SQLiteServerPrefs::SQLiteServerPrefs( QWidget *parent ) : QWidget( parent )
{
	QVBoxLayout * Form1Layout = new QVBoxLayout( this );

	Q3HBox *hbox = new Q3HBox( this );
	( void ) new QLabel( i18n( "Database file:" ), hbox );

	fileEdit = new KLineEdit( hbox );
	hbox->setStretchFactor( fileEdit, 2 );

	KIconLoader il;
	QPushButton *file_select = new QPushButton( il.loadIcon( "fileopen", KIcon::NoGroup, 16 ), QString::null, hbox );
	QToolTip::add
		( file_select, i18n( "Open file dialog" ) );
	file_select->setFixedWidth( 25 );

	Form1Layout->addWidget( hbox );

	connect( file_select, SIGNAL( clicked() ), SLOT( selectFile() ) );

	// Load & Save Current Settings
	KConfig *config = kapp->config();
	config->setGroup( "Server" );
	fileEdit->setText( config->readEntry( "DBFile", locateLocal( "appdata", "krecipes.krecdb" ) ) );
}

void SQLiteServerPrefs::saveOptions( void )
{
	KConfig * config = kapp->config();
	config->setGroup( "Server" );
	config->writeEntry( "DBFile", fileEdit->text() );
}

void SQLiteServerPrefs::selectFile()
{
	KFileDialog dialog( QString::null, "*.*|All Files", this, "dialog", true );
	if ( dialog.exec() == QDialog::Accepted ) {
		fileEdit->setText( dialog.selectedFile() );
	}
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

	numberButtonGroup = new Q3ButtonGroup( this );
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
	Q3GroupBox *ingredientGrpBox = new Q3GroupBox( 2, Qt::Vertical, i18n( "Ingredient" ), this );

	Q3HBox *ingredientBox = new Q3HBox( ingredientGrpBox );
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

//=============Import Preferences Dialog================//
ImportPrefs::ImportPrefs( QWidget *parent )
		: QWidget( parent )
{
	// Load Current Settings
	KConfig * config = kapp->config();
	config->setGroup( "Import" );

	bool overwrite = config->readBoolEntry( "OverwriteExisting", false );
	bool direct = config->readBoolEntry( "DirectImport", false );

	Form1Layout = new QVBoxLayout( this, 11, 6 );

	overwriteCheckbox = new QCheckBox( i18n( "Overwrite recipes with same title" ), this );
	overwriteCheckbox->setChecked( overwrite );
	overwriteCheckbox->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );

	directImportCheckbox = new QCheckBox( i18n( "Ask which recipes to import when importing multiple recipes" ), this );
	directImportCheckbox->setChecked( !direct );
	directImportCheckbox->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );

	Form1Layout->addWidget( overwriteCheckbox );
	Form1Layout->addWidget( directImportCheckbox );

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
	KConfig * config = kapp->config();
	config->setGroup( "Import" );

	config->writeEntry( "OverwriteExisting", overwriteCheckbox->isChecked() );
	config->writeEntry( "DirectImport", !directImportCheckbox->isChecked() );
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

	Q3HBox *catLimitHBox = new Q3HBox( this );
	catLimitInput = new KIntNumInput(catLimitHBox);
	catLimitInput->setLabel( i18n( "Number of recipes to display at once:" ) );
	catLimitInput->setRange(0,5000,20,true);
	catLimitInput->setSpecialValueText( i18n("Unlimited") );

	if ( cat_limit > 0 )
		catLimitInput->setValue( cat_limit );

	Q3HBox *limitHBox = new Q3HBox( this );
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
