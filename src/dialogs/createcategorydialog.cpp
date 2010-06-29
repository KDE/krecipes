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

#include "createcategorydialog.h"

#include <QGroupBox>
#include <QFormLayout>

#include <kcombobox.h>
#include <klineedit.h>
#include <klocale.h>

CreateCategoryDialog::CreateCategoryDialog( QWidget *parent, const ElementList& categories )
	 : KDialog( parent)
{
	setCaption(i18nc("@title:window", "New Category" ));
	setButtons(KDialog::Ok | KDialog::Cancel);
	setDefaultButton(KDialog::Ok);
	setModal( true );

	box = new QGroupBox;
	setMainWidget( box );

	QFormLayout *boxLayout = new QFormLayout;
	box->setLayout( boxLayout );
	
	box->setTitle( i18nc("@title:group", "New Category" ) );

	elementEdit = new KLineEdit;
	boxLayout->addRow( i18nc( "@label:textbox Category name", "Name:" ), elementEdit );

	categoryComboBox = new KComboBox;
	boxLayout->addRow( i18nc("@label:textbox", "Subcategory of:" ), categoryComboBox );
	categoryComboBox->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
	loadCategories( categories );

	adjustSize();
	resize( 400, size().height() );
	setFixedHeight( size().height() );

	elementEdit->setFocus();
}


CreateCategoryDialog::~CreateCategoryDialog()
{}

void CreateCategoryDialog::loadCategories( const ElementList& categories )
{
	categoryComboBox->insertItem( categoryComboBox->count(), i18nc("@item:inlistbox No category", "**NONE**" ) );
	for ( ElementList::const_iterator it = categories.begin(); it != categories.end(); ++it ) {
		categoryComboBox->insertItem( categoryComboBox->count(), ( *it ).name );
		idMap.insert( ( *it ).name, ( *it ).id );
	}
}

QString CreateCategoryDialog::newCategoryName() const
{
	return elementEdit->text();
}

int CreateCategoryDialog::subcategory()
{
	if ( categoryComboBox->currentIndex() == 0 ) {
		return -1;
	}
	else {
		return idMap[ categoryComboBox->currentText() ];
	}
}
