/***************************************************************************
*   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2004 Cyril Bosselut <bosselut@b1project.com>         *
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "createunitdialog.h"

#include <QLabel>
#include <QGridLayout>

#include <klocale.h>
#include <klineedit.h>
#include <kcombobox.h>
#include <KVBox>

CreateUnitDialog::CreateUnitDialog( QWidget *parent, const QString &name, const QString &plural, const QString &name_abbrev, const QString &plural_abbrev, bool newUnit )
		: KDialog( parent )
{
	setCaption(newUnit?i18nc( "@title:window", "New Unit" ):i18nc( "@title:window", "Unit" ));
	setButtons(KDialog::Ok | KDialog::Cancel);
	setDefaultButton(KDialog::Ok);
	setModal( true );

	KVBox *page = new KVBox( this );
	setMainWidget( page );

	box = new Q3GroupBox( page );
	box->setColumnLayout( 0, Qt::Vertical );
	box->layout() ->setSpacing( 6 );
	box->layout() ->setMargin( 11 );
	QGridLayout *gridLayout = new QGridLayout( box->layout() );
	gridLayout->setAlignment( Qt::AlignTop );

	box->setTitle( (newUnit)?i18nc( "@title:group", "New Unit" ):i18nc("@title:group", "Unit") );

	QLabel *nameLabel = new QLabel( i18nc("@label:textbox Single unit name",  "Singular:" ), box );
	nameEdit = new KLineEdit( name, box );

	gridLayout->addWidget( nameLabel, 0, 0 );
	gridLayout->addWidget( nameEdit, 0, 1 );

	QLabel *nameAbbrevLabel = new QLabel( i18nc( "@label:textbox Single unit abbreviation", "Abbreviation:" ), box );
	nameAbbrevEdit = new KLineEdit( name_abbrev, box );

	gridLayout->addWidget( nameAbbrevLabel, 0, 2 );
	gridLayout->addWidget( nameAbbrevEdit, 0, 3 );

	QLabel *pluralLabel = new QLabel( i18nc("@label:textbox Plural unit name", "Plural:" ), box );
	pluralEdit = new KLineEdit( plural, box );

	gridLayout->addWidget( pluralLabel, 1, 0 );
	gridLayout->addWidget( pluralEdit, 1, 1 );

	QLabel *pluralAbbrevLabel = new QLabel( i18nc( "@label:textbox Plural unit abbreviation", "Abbreviation:" ), box );
	pluralAbbrevEdit = new KLineEdit( plural_abbrev, box );

	gridLayout->addWidget( pluralAbbrevLabel, 1, 2 );
	gridLayout->addWidget( pluralAbbrevEdit, 1, 3 );

	QLabel *typeLabel = new QLabel( i18nc("@label:textbox Unit Type", "Type:" ), box );
	typeComboBox = new KComboBox( false, box );
	typeComboBox->insertItem( typeComboBox->count(), i18nc("@item:inlistbox Unit type other", "Other") );
	typeComboBox->insertItem( typeComboBox->count(), i18nc("@item:inlistbox Unit type mass", "Mass") );
	typeComboBox->insertItem( typeComboBox->count(), i18nc("@item:inlistbox Unit type volume", "Volume") );

	gridLayout->addWidget( typeLabel, 2, 0 );
	gridLayout->addWidget( typeComboBox, 2, 1, 1, 3, 0 );

	adjustSize();
	setFixedHeight( size().height() );

	connect( nameAbbrevEdit, SIGNAL(textChanged(const QString&)), SLOT(nameAbbrevTextChanged(const QString &)) );

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

	new_unit.type = (Unit::Type)typeComboBox->currentIndex();

	return new_unit;
}

void CreateUnitDialog::nameAbbrevTextChanged(const QString &newText)
{
	//appending
	if ( newText.left( newText.length()-1 ) == pluralAbbrevEdit->text() ) {
		pluralAbbrevEdit->setText( newText );
	}

	//truncating
	if ( newText.left( newText.length()-1 ) == pluralAbbrevEdit->text().left( newText.length()-1 ) ) {
		pluralAbbrevEdit->setText( newText );
	}
}

#include "createunitdialog.moc"
