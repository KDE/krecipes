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

#include "ingredientsdialog.h"

#include "backends/recipedb.h"
#include "createelementdialog.h"
#include "dialogs/dependanciesdialog.h"
#include "widgets/ingredientlistview.h"
#include "dialogs/ingredientgroupsdialog.h"
#include "dialogs/editpropertiesdialog.h"
#include "actionshandlers/ingredientactionshandler.h"

#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kglobal.h>
#include <kconfig.h>

#include <q3header.h>
#include <KTabWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <KPushButton>
#include <QPointer>

IngredientsDialog::IngredientsDialog( QWidget* parent, RecipeDB *db ) : QWidget( parent )
{

	// Store pointer to database
	database = db;

	// Design dialog

	QHBoxLayout* page_layout = new QHBoxLayout( this );
	page_layout->setMargin( KDialog::marginHint() );
	page_layout->setSpacing( KDialog::spacingHint() );

	tabWidget = new KTabWidget( this );

	ingredientTab = new QWidget( tabWidget );

	layout = new QGridLayout( ingredientTab );
	layout->cellRect( 1, 1 );
	layout->setSpacing( 0 );

	QSpacerItem* spacer_left = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_left, 1, 0 );
	QSpacerItem* spacer_top = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacer_top, 0, 1 );

	ingredientListView = new KreListView ( ingredientTab, i18nc( "@title", "Ingredient list" ), true, 0 );
	StdIngredientListView *list_view = new StdIngredientListView( ingredientListView, database, true );
	ingredientActionsHandler = new IngredientActionsHandler( list_view, database );
	ingredientListView->setListView( list_view );
	layout->addWidget ( ingredientListView, 1, 1, 5, 1, 0 );
	ingredientListView->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

	QSpacerItem* spacer_rightIngredients = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_rightIngredients, 1, 2 );

	QVBoxLayout *buttonLayout = new QVBoxLayout();

	addIngredientButton = new KPushButton( ingredientTab );
	addIngredientButton->setIcon( KIcon( "list-add") );
	addIngredientButton->setMinimumSize( QSize( 30, 30 ) );
	addIngredientButton->setMaximumSize( QSize( 30, 30 ) );
	addIngredientButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	buttonLayout->addWidget( addIngredientButton );

	QSpacerItem* spacer_Ing_Buttons = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Maximum );
	buttonLayout->addItem( spacer_Ing_Buttons );

	removeIngredientButton = new KPushButton( ingredientTab );
	removeIngredientButton->setIcon( KIcon( "list-remove" ) );
	removeIngredientButton->setMinimumSize( QSize( 30, 30 ) );
	removeIngredientButton->setMaximumSize( QSize( 30, 30 ) );
	removeIngredientButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	buttonLayout->addWidget( removeIngredientButton );
	
	layout->addItem( buttonLayout, 1, 3 );

	QPushButton *propertyButton = new KPushButton( i18nc("@action:button", "Property Information"), ingredientTab );
	propertyButton->setIcon( KIcon( "document-properties") );
	layout->addWidget( propertyButton, 6, 1 );

	tabWidget->insertTab( -1, ingredientTab, i18nc( "@title:tab", "Ingredients" ) );

	groupsDialog = new IngredientGroupsDialog(database,tabWidget,"groupsDialog");
	tabWidget->insertTab( -1, groupsDialog, i18nc( "@title:tab", "Headers" ) );

	page_layout->addWidget( tabWidget );

	// Signals & Slots
	connect( addIngredientButton, SIGNAL( clicked() ), ingredientActionsHandler, SLOT( createNew() ) );
	connect( removeIngredientButton, SIGNAL( clicked() ), ingredientActionsHandler, SLOT( remove() ) );
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

ActionsHandlerBase * IngredientsDialog::getActionsHandler() const
{
	if ( tabWidget->currentWidget() == ingredientTab )
		return ingredientActionsHandler;
	else //if ( tabWidget->currentWidget() == groupsDialog )
		return groupsDialog->getActionsHandler();
}

void IngredientsDialog::addAction( KAction * action )
{
	ingredientActionsHandler->addAction( action );
	groupsDialog->addAction( action );
}

void IngredientsDialog::showPropertyEdit()
{
	Q3ListViewItem * ing_it = ingredientListView->listView() ->selectedItem(); // Find selected ingredient
	if ( ing_it ) {
		QPointer<EditPropertiesDialog> d = new EditPropertiesDialog( ing_it->text(1).toInt(),ing_it->text(0),database,this );
		d->exec();
		delete d;
	}
	else
		KMessageBox::information( this, i18nc( "@info", "No ingredient selected." ), QString::null );
}

#include "ingredientsdialog.moc"
