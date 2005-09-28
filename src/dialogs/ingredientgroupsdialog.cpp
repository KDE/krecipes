/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "ingredientgroupsdialog.h"

#include <qpushbutton.h>
#include <qlayout.h>
#include <qvbox.h>

#include <klocale.h>
#include <kdialog.h>

#include "widgets/krelistview.h"
#include "widgets/headerlistview.h"

IngredientGroupsDialog::IngredientGroupsDialog( RecipeDB *db, QWidget *parent, const char *name ) : QWidget(parent,name), database(db)
{
	QHBoxLayout* layout = new QHBoxLayout( this, KDialog::marginHint(), KDialog::spacingHint() );

	headerListView = new KreListView ( this, i18n( "Header list" ), true, 0 );
	StdHeaderListView *list_view = new StdHeaderListView( headerListView, database, true );
	list_view->reload();
	headerListView->setListView( list_view );
	headerListView->setSizePolicy( QSizePolicy( QSizePolicy::Ignored, QSizePolicy::MinimumExpanding ) );
	layout->addWidget(headerListView);

	QVBox *buttonBox = new QVBox(this);
	QPushButton *addHeaderButton = new QPushButton( buttonBox );
	addHeaderButton->setText( "+" );
	addHeaderButton->setMinimumSize( QSize( 30, 30 ) );
	addHeaderButton->setMaximumSize( QSize( 30, 30 ) );
	addHeaderButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	addHeaderButton->setFlat( true );

	QPushButton *removeHeaderButton = new QPushButton( buttonBox );
	removeHeaderButton->setText( "-" );
	removeHeaderButton->setMinimumSize( QSize( 30, 30 ) );
	removeHeaderButton->setMaximumSize( QSize( 30, 30 ) );
	removeHeaderButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	removeHeaderButton->setFlat( true );

	layout->addWidget(buttonBox);

	connect( addHeaderButton, SIGNAL( clicked() ), list_view, SLOT( createNew() ) );
	connect( removeHeaderButton, SIGNAL( clicked() ), list_view, SLOT( remove() ) );
}

