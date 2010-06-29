/***************************************************************************
*   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2004 Cyril Bosselut <bosselut@b1project.com>         *
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>            *
*   Copyright © 2010 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "createunitdialog.h"

#include <QFormLayout>
#include <QGroupBox>

#include <klocale.h>
#include <klineedit.h>
#include <kcombobox.h>
#include <KVBox>

CreateUnitDialog::CreateUnitDialog( QWidget *parent, const QString &name, const QString &plural,
	const QString &name_abbrev, const QString &plural_abbrev, Unit::Type type, bool newUnit )
		: KDialog( parent )
{
	setCaption(newUnit?i18nc( "@title:window", "New Unit" ):i18nc( "@title:window", "Unit" ));
	setButtons(KDialog::Ok | KDialog::Cancel);
	setDefaultButton(KDialog::Ok);
	setModal( true );

	QGroupBox * box = new QGroupBox;
	QFormLayout * layout = new QFormLayout;
	box->setLayout( layout );
	setMainWidget( box );

	box->setTitle( (newUnit)?i18nc( "@title:group", "New Unit" ):i18nc("@title:group", "Unit") );
	
	nameEdit = new KLineEdit( name, box );
	layout->addRow( i18nc("@label:textbox Single unit name",
		"Singular:" ), nameEdit );
	
	nameAbbrevEdit = new KLineEdit( name_abbrev, box );
	layout->addRow( i18nc( "@label:textbox Single unit abbreviation",
		"Singular Abbreviation:" ), nameAbbrevEdit );

	pluralEdit = new KLineEdit( plural, box );
	layout->addRow( i18nc("@label:textbox Plural unit name",
		"Plural:" ), pluralEdit );

	pluralAbbrevEdit = new KLineEdit( plural_abbrev, box );
	layout->addRow( i18nc( "@label:textbox Plural unit abbreviation",
		"Plural Abbreviation:" ), pluralAbbrevEdit );

	typeComboBox = new KComboBox( false, box );
	typeComboBox->insertItem( typeComboBox->count(), i18nc("@item:inlistbox Unit type other", "Other") );
	typeComboBox->insertItem( typeComboBox->count(), i18nc("@item:inlistbox Unit type mass", "Mass") );
	typeComboBox->insertItem( typeComboBox->count(), i18nc("@item:inlistbox Unit type volume", "Volume") );

	typeComboBox->setCurrentIndex( type );

	layout->addRow( i18nc("@label:textbox Unit Type", "Type:" ), typeComboBox );

	adjustSize();
	resize( 400, size().height() );
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
	new_unit.setNameAbbrev(nameAbbrevEdit->text());
	new_unit.setPluralAbbrev(pluralAbbrevEdit->text());

	new_unit.setType((Unit::Type)typeComboBox->currentIndex());

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
