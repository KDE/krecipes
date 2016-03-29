/***************************************************************************
*   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                    *
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>             *
*   Copyright © 2009-2016 José Manuel Santamaría Lema <panfaust@gmail.com> *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#include "sqlitesetuppage.h"

#include <KLocale>
#include <KStandardDirs>
#include <KLineEdit>
#include <KHBox>
#include <KFileDialog>

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>


SQLiteSetupPage::SQLiteSetupPage( QWidget *parent ) : QWidget( parent )
{
	QGridLayout * layout = new QGridLayout( this );
	layout->cellRect( 1, 1 );
	layout->setMargin( 0 );
	layout->setSpacing( 0 );
	QSpacerItem *spacer_top = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacer_top, 0, 1 );
	QSpacerItem *spacer_left = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_left, 1, 0 );


	// Image

	QPixmap serverSetupPixmap ( KStandardDirs::locate( "data", "krecipes/pics/network.png" ) );
	logo = new QLabel( this );
	logo->setPixmap( serverSetupPixmap );
	logo->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout->addWidget( logo, 1, 1, 4, 1, Qt::AlignTop );

	QSpacerItem *spacer_from_image = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_from_image, 1, 2 );


	// Explanation text
	serverSetupText = new QLabel( this );
	serverSetupText->setText( i18n( "In this dialog you can adjust SQLite settings." ) );
	serverSetupText->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	serverSetupText->setAlignment( Qt::AlignTop | Qt::AlignJustify );
	layout->addWidget( serverSetupText, 1, 3 );

	// Text spacer

	QSpacerItem* textSpacer = new QSpacerItem( 10, 30, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( textSpacer, 2, 3 );

	// Input Boxes

	KHBox *hbox = new KHBox( this );

	( void ) new QLabel( i18n( "Database file:" ), hbox );

	fileEdit = new KLineEdit( hbox );
	fileEdit->setText( KStandardDirs::locateLocal ( "appdata", "krecipes.krecdb" ) );
	hbox->setStretchFactor( fileEdit, 2 );

	KIconLoader *il = KIconLoader::global();
	QPushButton *file_select = new QPushButton( il->loadIcon( "document-open", KIconLoader::NoGroup, 16 ), QString(), hbox );
	file_select->setToolTip( i18n( "Open file dialog" ) );
	file_select->setFixedWidth( 25 );

	layout->addWidget( hbox, 3, 3 );

	// Bottom Spacers

	QSpacerItem* bottomSpacer = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
	layout->addItem( bottomSpacer, 4, 1 );

	connect( file_select, SIGNAL( clicked() ), SLOT( selectFile() ) );
}

QString SQLiteSetupPage::dbFile( void ) const
{
	return ( fileEdit->text() );
}

void SQLiteSetupPage::selectFile()
{
	QPointer<KFileDialog> dialog = new KFileDialog( KUrl(), "*.*|All Files", this );
	dialog->setObjectName( "dialog" );
	dialog->setCaption( i18n( "Select SQLite database file") );
	dialog->setModal( true );
	if ( dialog->exec() == QDialog::Accepted ) {
		fileEdit->setText( dialog->selectedFile() );
	}
	delete dialog;
}

