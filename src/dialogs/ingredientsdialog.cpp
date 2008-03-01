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

#include "ingredientsdialog.h"

#include "backends/recipedb.h"
#include "createelementdialog.h"
#include "dialogs/dependanciesdialog.h"
#include "widgets/ingredientlistview.h"
#include "dialogs/ingredientgroupsdialog.h"
#include "dialogs/editpropertiesdialog.h"

#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kglobal.h>
#include <kconfig.h>

#include <q3header.h>
#include <qtabwidget.h>
#include <QGridLayout>
#include <QHBoxLayout>

IngredientsDialog::IngredientsDialog( QWidget* parent, RecipeDB *db ) : QWidget( parent )
{

	// Store pointer to database
	database = db;

	// Design dialog

	QHBoxLayout* page_layout = new QHBoxLayout( this );
   page_layout->setMargin( KDialog::marginHint() );
   page_layout->setSpacing( KDialog::spacingHint() );

	QTabWidget *tabWidget = new QTabWidget( this );

	QWidget *ingredientTab = new QWidget( tabWidget );

	layout = new QGridLayout( ingredientTab );
   layout->cellRect( 1, 1 );
   layout->setMargin( 0 );
   layout->setSpacing( 0 );

	QSpacerItem* spacer_left = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_left, 1, 0 );
	QSpacerItem* spacer_top = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacer_top, 0, 1 );

	ingredientListView = new KreListView ( ingredientTab, i18n( "Ingredient list" ), true, 0 );
	StdIngredientListView *list_view = new StdIngredientListView( ingredientListView, database, true );
	ingredientListView->setListView( list_view );
	layout->addWidget ( ingredientListView, 1, 1, 5, 1, 0 );
	ingredientListView->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

	QSpacerItem* spacer_rightIngredients = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_rightIngredients, 1, 2 );


	addIngredientButton = new QPushButton( ingredientTab );
	addIngredientButton->setText( "+" );
	layout->addWidget( addIngredientButton, 1, 3 );
	addIngredientButton->setMinimumSize( QSize( 30, 30 ) );
	addIngredientButton->setMaximumSize( QSize( 30, 30 ) );
	addIngredientButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	addIngredientButton->setFlat( true );

	removeIngredientButton = new QPushButton( ingredientTab );
	removeIngredientButton->setText( "-" );
	layout->addWidget( removeIngredientButton, 3, 3 );
	removeIngredientButton->setMinimumSize( QSize( 30, 30 ) );
	removeIngredientButton->setMaximumSize( QSize( 30, 30 ) );
	removeIngredientButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	removeIngredientButton->setFlat( true );

	QSpacerItem* spacer_Ing_Buttons = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacer_Ing_Buttons, 2, 3 );

	QPushButton *propertyButton = new QPushButton( i18n("Property Information"), ingredientTab );
	layout->addWidget( propertyButton, 6, 1 );

	tabWidget->insertTab( -1, ingredientTab, i18n( "Ingredients" ) );

	groupsDialog = new IngredientGroupsDialog(database,tabWidget,"groupsDialog");
	tabWidget->insertTab( -1, groupsDialog, i18n( "Headers" ) );

	page_layout->addWidget( tabWidget );

	// Signals & Slots
	connect( addIngredientButton, SIGNAL( clicked() ), list_view, SLOT( createNew() ) );
	connect( removeIngredientButton, SIGNAL( clicked() ), list_view, SLOT( remove
		         () ) );
	connect( propertyButton, SIGNAL( clicked() ), this, SLOT( showPropertyEdit() ) );

	DependanciesDialog d( this, ListInfo() );
}


IngredientsDialog::~IngredientsDialog()
{
}

void IngredientsDialog::reloadIngredientList( ReloadFlags flag )
{
	( ( StdIngredientListView* ) ingredientListView->listView() ) ->reload(flag);
}

void IngredientsDialog::reload( ReloadFlags flag )
{
	reloadIngredientList( flag );
	groupsDialog->reload( flag );
}

void IngredientsDialog::showPropertyEdit()
{
	Q3ListViewItem * ing_it = ingredientListView->listView() ->selectedItem(); // Find selected ingredient
	if ( ing_it ) {
		EditPropertiesDialog d(ing_it->text(1).toInt(),ing_it->text(0),database,this);
		d.exec();
	}
	else
		QMessageBox::information( this, QString::null, i18n( "No ingredient selected." ) );
}

#include "ingredientsdialog.moc"
