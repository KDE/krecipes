/***************************************************************************
 *   Copyright (C) 2004 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "dbimportdialog.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qwidgetstack.h>
#include <qwidget.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kurlrequester.h>

DBImportDialog::DBImportDialog(QWidget *parent, const char *name, bool modal, WFlags f):QDialog(parent,name, modal,f)
{
	setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0,0, sizePolicy().hasHeightForWidth() ) );
	MyDialogLayout = new QHBoxLayout( this, 11, 6, "MyDialogLayout"); 
	
	dbButtonGroup = new QButtonGroup( this, "dbButtonGroup" );
	dbButtonGroup->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)4, (QSizePolicy::SizeType)5, 0, 0, dbButtonGroup->sizePolicy().hasHeightForWidth() ) );
	dbButtonGroup->setColumnLayout(0, Qt::Vertical );
	dbButtonGroup->layout()->setSpacing( 6 );
	dbButtonGroup->layout()->setMargin( 11 );
	dbButtonGroupLayout = new QVBoxLayout( dbButtonGroup->layout() );
	dbButtonGroupLayout->setAlignment( Qt::AlignTop );
	
	liteRadioButton = new QRadioButton( dbButtonGroup, "liteRadioButton" );
	liteRadioButton->setChecked( TRUE );
	dbButtonGroupLayout->addWidget( liteRadioButton );
	
	mysqlRadioButton = new QRadioButton( dbButtonGroup, "mysqlRadioButton" );
	dbButtonGroupLayout->addWidget( mysqlRadioButton );
	
	psqlRadioButton = new QRadioButton( dbButtonGroup, "psqlRadioButton" );
	dbButtonGroupLayout->addWidget( psqlRadioButton );
	MyDialogLayout->addWidget( dbButtonGroup );
	
	paramStack = new QWidgetStack( this, "paramStack" );
	paramStack->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)5, 0, 0, paramStack->sizePolicy().hasHeightForWidth() ) );
	
	sqlitePage = new QWidget( paramStack, "sqlitePage" );
	serverPageLayout_2 = new QVBoxLayout( sqlitePage, 11, 6, "serverPageLayout_2"); 
	
	QLabel *sqliteLabel = new QLabel( i18n( "Database file:" ), sqlitePage );
	serverPageLayout_2->addWidget( sqliteLabel );
	sqliteDBRequester = new KURLRequester( sqlitePage, "sqliteDBRequester" );
	sqliteDBRequester->setShowLocalProtocol(false);
	serverPageLayout_2->addWidget( sqliteDBRequester );
	paramStack->addWidget( sqlitePage, 1 );

	serverPage = new QWidget( paramStack, "serverPage" );
	serverPageLayout = new QVBoxLayout( serverPage, 11, 6, "serverPageLayout"); 

	layout5 = new QGridLayout( 0, 1, 1, 0, 6, "layout5"); 

	hostEdit = new QLineEdit( serverPage, "hostEdit" );
	layout5->addWidget( hostEdit, 0, 1 );
	hostLabel = new QLabel( serverPage, "hostLabel" );
	layout5->addWidget( hostLabel, 0, 0 );

	userEdit = new QLineEdit( serverPage, "userEdit" );
	layout5->addWidget( userEdit, 1, 1 );
	userLabel = new QLabel( serverPage, "userLabel" );
	layout5->addWidget( userLabel, 1, 0 );

	passwordEdit = new QLineEdit( serverPage, "passwordEdit" );
	passwordEdit->setEchoMode( QLineEdit::Password );
	layout5->addWidget( passwordEdit, 2, 1 );
	passwordLabel = new QLabel( serverPage, "passwordLabel" );
	layout5->addWidget( passwordLabel, 2, 0 );

	nameEdit = new QLineEdit( serverPage, "nameEdit" );
	layout5->addWidget( nameEdit, 3, 1 );
	nameLabel = new QLabel( serverPage, "nameLabel" );
	layout5->addWidget( nameLabel, 3, 0 );

	serverPageLayout->addLayout( layout5 );
	paramStack->addWidget( serverPage, 0 );
	MyDialogLayout->addWidget( paramStack );

	Layout5 = new QVBoxLayout( 0, 0, 6, "Layout5"); 

	buttonOk = new QPushButton( this, "buttonOk" );
	buttonOk->setAutoDefault( TRUE );
	buttonOk->setDefault( TRUE );
	Layout5->addWidget( buttonOk );

	buttonCancel = new QPushButton( this, "buttonCancel" );
	buttonCancel->setAutoDefault( TRUE );
	Layout5->addWidget( buttonCancel );
	Spacer1 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
	Layout5->addItem( Spacer1 );
	MyDialogLayout->addLayout( Layout5 );

	languageChange();

	adjustSize();
	setFixedHeight(height());
	clearWState( WState_Polished );

	#if (!HAVE_MYSQL)
	mysqlRadioButton->setEnabled(false);
	#endif
	
	#if (!HAVE_POSTGRESQL)
	psqlRadioButton->setEnabled(false);
	#endif
	
	#if (!(HAVE_SQLITE || HAVE_SQLITE3))
	liteRadioButton->setEnabled(false);
		#if (HAVE_MYSQL)
		dbButtonGroup->setButton(1); // Otherwise by default liteCheckBox is checked even if it's disabled
		#else
		#if (HAVE_POSTGRESQL)
		dbButtonGroup->setButton(2);
		#endif
		#endif
	#endif

	// signals and slots connections
	connect( dbButtonGroup, SIGNAL( clicked(int) ), this, SLOT( switchDBPage(int) ) );

	connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

DBImportDialog::~DBImportDialog()
{
}

void DBImportDialog::languageChange()
{
	setCaption( i18n( "Database Import" ) );
	dbButtonGroup->setTitle( i18n( "Database" ) );
	liteRadioButton->setText( "SQLite" );
	mysqlRadioButton->setText( "MySQL" );
	psqlRadioButton->setText( "PostgreSQL" );
	hostLabel->setText( i18n( "Server:" ) );
	userLabel->setText( i18n( "Username:" ) );
	passwordLabel->setText( i18n( "Password:" ) );
	nameLabel->setText( i18n( "Database name:" ) );
	buttonOk->setText( i18n( "&OK" ) );
	buttonOk->setAccel( QKeySequence( QString::null ) );
	buttonCancel->setText( i18n( "&Cancel" ) );
	buttonCancel->setAccel( QKeySequence( QString::null ) );
}

void DBImportDialog::switchDBPage(int id)
{
	switch(id)
	{
	case 0: //SQLite
		paramStack->raiseWidget( sqlitePage );
		break;
	case 1: //MySQL
	case 2: //PostgreSQL
		paramStack->raiseWidget( serverPage );
		break;
	}
}

QString DBImportDialog::dbType() const
{
	//int id=dbButtonGroup->selectedId(); //QT 3.2
	int id=dbButtonGroup->id(dbButtonGroup->selected()); //QT 3.1
	switch(id)
	{
	case 0: return "SQLite";
	case 1: return "MySQL";
	case 2: return "PostgreSQL";
	default: return QString::null;
	}
}

void DBImportDialog::serverParams(QString &host, QString &user, QString &pass, QString &table) const
{
	host = hostEdit->text();
	user = userEdit->text();
	pass = passwordEdit->text();
	table = nameEdit->text();
}

QString DBImportDialog::dbFile() const
{
	return sqliteDBRequester->url();
}

#include "dbimportdialog.moc"
