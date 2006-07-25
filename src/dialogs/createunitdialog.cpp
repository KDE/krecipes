/***************************************************************************
*   Copyright (C) 2003-2004 by                                            *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "createunitdialog.h"

#include <klocale.h>
#include <qlabel.h>

CreateUnitDialog::CreateUnitDialog( QWidget *parent, const QString &name, const QString &plural )
		: KDialogBase( parent, "createElementDialog", true, i18n( "New Unit" ),
		    KDialogBase::Ok | KDialogBase::Cancel, KDialogBase::Ok )
{
	QVBox *page = makeVBoxMainWidget();

	box = new QGroupBox( page );
	box->setColumnLayout( 0, Qt::Vertical );
	box->layout() ->setSpacing( 6 );
	box->layout() ->setMargin( 11 );
	QGridLayout *gridLayout = new QGridLayout( box->layout() );
	gridLayout->setAlignment( Qt::AlignTop );

	box->setTitle( i18n( "New Unit" ) );

	QLabel *nameLabel = new QLabel( i18n( "Singular:" ), box );
	nameEdit = new KLineEdit( name, box );

	gridLayout->addWidget( nameLabel, 0, 0 );
	gridLayout->addWidget( nameEdit, 0, 1 );

	QLabel *nameAbbrevLabel = new QLabel( i18n( "Abbreviation:" ), box );
	nameAbbrevEdit = new KLineEdit( QString::null, box );

	gridLayout->addWidget( nameAbbrevLabel, 0, 2 );
	gridLayout->addWidget( nameAbbrevEdit, 0, 3 );

	QLabel *pluralLabel = new QLabel( i18n( "Plural:" ), box );
	pluralEdit = new KLineEdit( plural, box );

	gridLayout->addWidget( pluralLabel, 1, 0 );
	gridLayout->addWidget( pluralEdit, 1, 1 );

	QLabel *pluralAbbrevLabel = new QLabel( i18n( "Abbreviation:" ), box );
	pluralAbbrevEdit = new KLineEdit( QString::null, box );

	gridLayout->addWidget( pluralAbbrevLabel, 1, 2 );
	gridLayout->addWidget( pluralAbbrevEdit, 1, 3 );

	adjustSize();
	setFixedSize( size() ); //we've got all the widgets put in, now let's keep it this size

	if ( name.isEmpty() )
		nameEdit->setFocus();
	else if ( plural.isEmpty() )
		pluralEdit->setFocus();
}


CreateUnitDialog::~CreateUnitDialog()
{}

Unit CreateUnitDialog::newUnit( void )
{
	QString name = nameEdit->text();
	QString plural = pluralEdit->text();

	if ( name.isEmpty() )
		name = plural;
	if ( plural.isEmpty() )
		plural = name;

	Unit new_unit = Unit( name, plural );
	new_unit.name_abbrev = nameAbbrevEdit->text();
	new_unit.plural_abbrev = pluralAbbrevEdit->text();

	return new_unit;
}
