/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@users.sourceforge.net>            *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "createpropertydialog.h"

#include <klocale.h>
#include <QGridLayout>
#include <QLabel>
#include <KVBox>

CreatePropertyDialog::CreatePropertyDialog( QWidget *parent, UnitList* list )
		: KDialog( parent )
{
	setCaption( i18n( "New Property" ) );
	setButtons(KDialog::Ok | KDialog::Cancel);
	setDefaultButton( KDialog::Ok);
	setModal( true );
	// Initialize Internal Variables
	unitList = list; // Store the pointer to the unitList;

	// Initialize widgets
	KVBox *page = new KVBox( this );
	setMainWidget( page );
	box = new Q3GroupBox( page );
	box->setColumnLayout( 0, Qt::Vertical );
	box->layout() ->setSpacing( 6 );
	box->layout() ->setMargin( 11 );
	QGridLayout *gridLayout = new QGridLayout( box->layout() );
	gridLayout->setAlignment( Qt::AlignTop );
	box->setTitle( i18n( "New Property" ) );

	nameEditText = new QLabel( i18n( "Property name:" ), box );
	propertyNameEdit = new KLineEdit( box );
	propertyNameEdit->setMinimumWidth( 150 );
	gridLayout->addWidget( nameEditText, 0, 0 );
	gridLayout->addWidget( propertyNameEdit, 0, 1 );

	unitsText = new QLabel( i18n( "Units:" ), box );
	propertyUnits = new KLineEdit( box );
	propertyUnits->setMinimumWidth( 150 );
	gridLayout->addWidget( unitsText, 1, 0 );
	gridLayout->addWidget( propertyUnits, 1, 1 );

	adjustSize();
	resize( 400, size().height() );
	setFixedHeight( size().height() );

	propertyNameEdit->setFocus();
}


CreatePropertyDialog::~CreatePropertyDialog()
{}


QString CreatePropertyDialog::newPropertyName( void )
{
	return ( propertyNameEdit->text() );
}

QString CreatePropertyDialog::newUnitsName( void )
{
	return ( propertyUnits->text() );
}

