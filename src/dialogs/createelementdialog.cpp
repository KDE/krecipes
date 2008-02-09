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

#include "createelementdialog.h"
#include <kvbox.h>
#include <klocale.h>
//Added by qt3to4:
#include <Q3VBoxLayout>

CreateElementDialog::CreateElementDialog( QWidget *parent, const QString &text )
		: KDialog( parent)
{
    setCaption( text );
    setModal( true );
    setButtons( KDialog::Ok|KDialog::Cancel );
    setDefaultButton( KDialog::Ok );

    KVBox *page = new KVBox( this );
    setMainWidget( page );
	box = new Q3GroupBox( page );
	box->setColumnLayout( 0, Qt::Vertical );
	box->layout() ->setSpacing( 6 );
	box->layout() ->setMargin( 11 );
	Q3VBoxLayout *boxLayout = new Q3VBoxLayout( box->layout() );
	boxLayout->setAlignment( Qt::AlignTop );
	box->setTitle( text );

	elementEdit = new KLineEdit( box );
	boxLayout->addWidget( elementEdit );

	adjustSize();
	setFixedSize( size() ); //we've got all the widgets put in, now let's keep it this size

	elementEdit->setFocus();
}


CreateElementDialog::~CreateElementDialog()
{}

QString CreateElementDialog::newElementName( void )
{
	return ( elementEdit->text() );
}

