/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (jkvlighn@gmail.com)                                   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "restoredialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <kurlrequester.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include <klocale.h>

/*
 *  Constructs a RestoreDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
RestoreDialog::RestoreDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
	if ( !name )
		setName( "RestoreDialog" );
	setSizeGripEnabled( TRUE );
	RestoreDialogLayout = new QVBoxLayout( this, 11, 6, "RestoreDialogLayout"); 
	
	textLabel2 = new QLabel( this, "textLabel2" );
	textLabel2->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
	RestoreDialogLayout->addWidget( textLabel2 );
	
	layout3 = new QHBoxLayout( 0, 0, 6, "layout3"); 
	
	textLabel1 = new QLabel( this, "textLabel1" );
	layout3->addWidget( textLabel1 );
	
	kURLRequester1 = new KURLRequester( this, "kURLRequester1" );
	layout3->addWidget( kURLRequester1 );
	RestoreDialogLayout->addLayout( layout3 );
	
	Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 
	
	#if 0
	buttonHelp = new QPushButton( this, "buttonHelp" );
	buttonHelp->setAutoDefault( TRUE );
	Layout1->addWidget( buttonHelp );
	Horizontal_Spacing2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	Layout1->addItem( Horizontal_Spacing2 );
	#endif

	buttonOk = new QPushButton( this, "buttonOk" );
	buttonOk->setAutoDefault( TRUE );
	buttonOk->setDefault( TRUE );
	Layout1->addWidget( buttonOk );
	
	buttonCancel = new QPushButton( this, "buttonCancel" );
	buttonCancel->setAutoDefault( TRUE );
	Layout1->addWidget( buttonCancel );
	RestoreDialogLayout->addLayout( Layout1 );
	languageChange();
	resize( QSize(443, 156).expandedTo(minimumSizeHint()) );
	clearWState( WState_Polished );
	
	// signals and slots connections
	connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
RestoreDialog::~RestoreDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void RestoreDialog::languageChange()
{
    setCaption( i18n( "Restore Backup" ) );
    textLabel2->setText( i18n( "Please a select a backup file.<br /><br />"
"<b>Important: A backup file created from one database backend can only be restored into the same database backend.</b>" ) );
    textLabel1->setText( i18n( "Backup file:" ) );
    //buttonHelp->setText( i18n( "&Help" ) );
    //buttonHelp->setAccel( QKeySequence( i18n( "F1" ) ) );
    buttonOk->setText( i18n( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    buttonCancel->setText( i18n( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
}

QString RestoreDialog::backupFile() const
{
	return kURLRequester1->url();
}

#include "restoredialog.moc.cpp"
