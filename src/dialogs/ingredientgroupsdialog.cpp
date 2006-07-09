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
	headerListView->setListView( list_view );
	headerListView->setSizePolicy( QSizePolicy( QSizePolicy::Ignored, QSizePolicy::MinimumExpanding ) );
	layout->addWidget(headerListView);

	QVBoxLayout *buttonLayout = new QVBoxLayout(this);
	QPushButton *addHeaderButton = new QPushButton( this );
	addHeaderButton->setText( "+" );
	addHeaderButton->setMinimumSize( QSize( 30, 30 ) );
	addHeaderButton->setMaximumSize( QSize( 30, 30 ) );
	addHeaderButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	addHeaderButton->setFlat( true );
	buttonLayout->addWidget(addHeaderButton);

	QSpacerItem* spacer_buttons = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Fixed );
	buttonLayout->addItem( spacer_buttons );

	QPushButton *removeHeaderButton = new QPushButton( this );
	removeHeaderButton->setText( "-" );
	removeHeaderButton->setMinimumSize( QSize( 30, 30 ) );
	removeHeaderButton->setMaximumSize( QSize( 30, 30 ) );
	removeHeaderButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	removeHeaderButton->setFlat( true );
	buttonLayout->addWidget(removeHeaderButton);

	QSpacerItem* spacer_below_buttons = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::MinimumExpanding );
	buttonLayout->addItem( spacer_below_buttons );

	layout->addLayout(buttonLayout);

	connect( addHeaderButton, SIGNAL( clicked() ), list_view, SLOT( createNew() ) );
	connect( removeHeaderButton, SIGNAL( clicked() ), list_view, SLOT( remove() ) );
}

void IngredientGroupsDialog::reload()
{
	( ( StdHeaderListView* ) headerListView->listView() ) ->reload();
}
