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

#include "createcategorydialog.h"

#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <kvbox.h>
#include <QVBoxLayout>

#include <kcombobox.h>
#include <klineedit.h>
#include <klocale.h>

CreateCategoryDialog::CreateCategoryDialog( QWidget *parent, const ElementList& categories )
	 : KDialog( parent)
{
	 setCaption(i18n("New Category" ));
	 setButtons(KDialog::Ok | KDialog::Cancel);
	 setDefaultButton(KDialog::Ok);
	 setModal( true );

	 KVBox *page = new KVBox( this );
	 setMainWidget( page );

	 box = new QGroupBox( page );
	 QVBoxLayout *boxLayout = new QVBoxLayout;
	 box->setLayout( boxLayout );
	 boxLayout->setAlignment( Qt::AlignTop );
	 box->setTitle( i18n( "New Category" ) );

	 elementEdit = new KLineEdit( box );
	 boxLayout->addWidget( elementEdit );

	 KHBox *subcatHBox = new KHBox( box );
	 ( void ) new QLabel( i18n( "Subcategory of:" ), subcatHBox );
	 categoryComboBox = new KComboBox( subcatHBox );
	 boxLayout->addWidget( subcatHBox );
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
	categoryComboBox->insertItem( categoryComboBox->count(), i18n( "**NONE**" ) );
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
