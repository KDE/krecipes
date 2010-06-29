/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2010 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "createpropertydialog.h"

#include <klocale.h>
#include <QGroupBox>
#include <QFormLayout>

CreatePropertyDialog::CreatePropertyDialog( QWidget *parent, UnitList* list )
		: KDialog( parent )
{
	setCaption( i18nc( "@title:window", "New Property" ) );
	setButtons(KDialog::Ok | KDialog::Cancel);
	setDefaultButton( KDialog::Ok);
	setModal( true );
	// Initialize Internal Variables
	unitList = list; // Store the pointer to the unitList;

	// Initialize widgets
	QGroupBox * box = new QGroupBox;
	setMainWidget( box );
	QFormLayout * layout = new QFormLayout;
	box->setLayout( layout );
	box->setTitle( i18nc( "@title:group", "New Property" ) );

	propertyNameEdit = new KLineEdit( box );
	propertyNameEdit->setMinimumWidth( 150 );
	layout->addRow( i18nc( "@label:textbox", "Property name:" ), propertyNameEdit );

	propertyUnits = new KLineEdit( box );
	propertyUnits->setMinimumWidth( 150 );
	layout->addRow( i18nc( "@label:textbox", "Units:" ), propertyUnits );

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

