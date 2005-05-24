/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "createcategorydialog.h"

#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qhbox.h>

#include <kcombobox.h>
#include <klineedit.h>
#include <klocale.h>

CreateCategoryDialog::CreateCategoryDialog( QWidget *parent, const ElementList& categories )
		: QDialog( parent, 0, true )
{
	container = new QVBoxLayout( this, 5, 5 );
	box = new QGroupBox( this );
	box->setColumnLayout( 0, Qt::Vertical );
	box->layout() ->setSpacing( 6 );
	box->layout() ->setMargin( 11 );
	QVBoxLayout *boxLayout = new QVBoxLayout( box->layout() );
	boxLayout->setAlignment( Qt::AlignTop );
	box->setTitle( i18n( "New Category" ) );

	elementEdit = new KLineEdit( box );
	boxLayout->addWidget( elementEdit );

	QHBox *subcatHBox = new QHBox( box );
	( void ) new QLabel( i18n( "Subcategory of:" ), subcatHBox );
	categoryComboBox = new KComboBox( subcatHBox );
	boxLayout->addWidget( subcatHBox );
	loadCategories( categories );

	QHBoxLayout *button_hbox = new QHBoxLayout( 5 );
	okButton = new QPushButton( i18n( "&OK" ), this );
	cancelButton = new QPushButton( i18n( "&Cancel" ), this );
	button_hbox->addWidget( okButton );
	button_hbox->addWidget( cancelButton );
	boxLayout->addLayout( button_hbox );

	container->addWidget( box );

	adjustSize();
	setFixedSize( size() ); //we've got all the widgets put in, now let's keep it this size

	connect ( okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect ( cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
}


CreateCategoryDialog::~CreateCategoryDialog()
{}

void CreateCategoryDialog::loadCategories( const ElementList& categories )
{
	categoryComboBox->insertItem( i18n( "**NONE**" ) );
	for ( ElementList::const_iterator it = categories.begin(); it != categories.end(); ++it ) {
		categoryComboBox->insertItem( ( *it ).name );
		idMap.insert( ( *it ).name, ( *it ).id );
	}
}

QString CreateCategoryDialog::newCategoryName( void )
{
	return ( elementEdit->text() );
}

int CreateCategoryDialog::subcategory( void )
{
	if ( categoryComboBox->currentItem() == 0 ) {
		return -1;
	}
	else {
		return idMap[ categoryComboBox->currentText() ];
	}
}
