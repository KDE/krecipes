/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "createcategorydialog.h"

#include <qpushbutton.h>
#include <q3groupbox.h>
#include <qlayout.h>
#include <qlabel.h>
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

	box = new Q3GroupBox( page );
	box->setColumnLayout( 0, Qt::Vertical );
	box->layout() ->setSpacing( 6 );
	box->layout() ->setMargin( 11 );
	QVBoxLayout *boxLayout = new QVBoxLayout( box->layout() );
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
	setFixedSize( size() ); //we've got all the widgets put in, now let's keep it this size

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

QString CreateCategoryDialog::newCategoryName( void )
{
	return ( elementEdit->text() );
}

int CreateCategoryDialog::subcategory( void )
{
	if ( categoryComboBox->currentIndex() == 0 ) {
		return -1;
	}
	else {
		return idMap[ categoryComboBox->currentText() ];
	}
}
